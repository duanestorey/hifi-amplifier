#include "amplifier.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


#include "esp_system.h"
#include "esp_log.h"
#include "driver/ledc.h"

#include "pins.h"
#include "esp_log.h"
#include "debug.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include "sdkconfig.h"
#include "dac-pcm5142.h"
#include "cs8416.h"
#include "channelsel-ax2358.h"
#include "button.h"
#include "http-server.h"

#include "esp_netif_ip_addr.h"
#include "esp_mac.h"
#include "netdb.h"
#include "mdns.h"

Amplifier::Amplifier() : mWifiEnabled( false ), mWifiConnectionAttempts( 0 ), mUpdatingFromNTP( false ), mPoweredOn( true ), mDigitalAudioStarted( false ), mDisplayQueue( 3 ), mTimerID( 0 ), mButtonTimerID( 0 ), mReconnectTimerID( 0 ), mLCD( 0 ),
    mDAC( 0 ), mChannelSel( 0 ), mWebServer( 0 ), mSPDIF( 0 ), mMicroprocessorTemp( 0 ), mVolumeEncoder( 15, 13, true ), mInputEncoder( 4, 16, false ), mAudioTimerID( 0 ), mSpdifTimerID( 0 ), mPendingVolumeChange( false ), mPendingVolume( 0 ),
    mPowerButton( 0 ), mVolumeButton( 0 ), mInputButton( 0 ) {
}

void 
Amplifier::taskDelayInMs( uint32_t ms ) {
    vTaskDelay( ms / portTICK_PERIOD_MS );
}

AmplifierState 
Amplifier::getCurrentState() {
    ScopedLock lock( mStateMutex );

    AmplifierState stateCopy = mState;
    return stateCopy;
}

void 
Amplifier::updateConnectedStatus( bool connected, bool doActualUpdate ) {
    {
        ScopedLock lock( mStateMutex );
        mState.mConnected = connected;
    }

    if ( doActualUpdate ) {
        asyncUpdateDisplay();
    }   

    if ( connected ) {

        esp_err_t err = mdns_init();
        if (err) {
            printf("MDNS Init failed: %d\n", err);
            return;
        }

        //set hostname
        mdns_hostname_set( "amp" ) ;

        mdns_instance_name_set( "Hifi Audio Amplifier" );
        mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);

        mWebServer->start();
    } else {
        mWebServer->stop();
    }
}

void
Amplifier::_handleIRInterrupt( const rmt_rx_done_event_data_t *edata ) {
    mAmplifierQueue.add( Message::MSG_IR_CODE_RECEIVER, (uint32_t)edata );
}

bool
rmt_rx_done_callback( rmt_channel_handle_t rx_chan, const rmt_rx_done_event_data_t *edata, void *user_ctx ) {
    (( Amplifier *)user_ctx)->_handleIRInterrupt( edata );
    return false;
}

void 
Amplifier::setupRemoteReceiver() {
    AMP_DEBUG_I( "Setting up IR receiver" );

    mIRChannel = NULL;

    rmt_rx_channel_config_t rx_chan_config;
    memset( &rx_chan_config, 0, sizeof( rx_chan_config ) );

    rx_chan_config.clk_src = RMT_CLK_SRC_DEFAULT;   // select source clock
    rx_chan_config.resolution_hz = 1 * 1000 * 1000; // 1 MHz tick resolution, i.e., 1 tick = 1 µs
    rx_chan_config.mem_block_symbols = 64;          // memory block size, 64 * 4 = 256 Bytes
    rx_chan_config.gpio_num = GPIO_NUM_26;              // GPIO number
    rx_chan_config.flags.invert_in = false;        // do not invert input signal
    rx_chan_config.flags.with_dma = false;          // do not need DMA backend

    ESP_ERROR_CHECK( rmt_new_rx_channel( &rx_chan_config, &mIRChannel ) );   

    rmt_rx_event_callbacks_t cbs = {
                .on_recv_done = rmt_rx_done_callback
            }; 

    ESP_ERROR_CHECK( rmt_rx_register_event_callbacks( mIRChannel, &cbs, this ) );

    rmt_enable( mIRChannel );

    doActualRemoteReceive();
}

void 
Amplifier::init() {
    nvs_flash_init(); 

    configurePins();
    setupPWM();

    taskDelayInMs( 10 );

    activateButtonLight( true );

    mTimerID = mTimer.setTimer( 60000, mAmplifierQueue, true );
    mButtonTimerID = mTimer.setTimer( 10, mAmplifierQueue, true );

    // I2C current map
    /*
        Current
        -------
        LCD                             0x27
        Channel Selector AX2358         0x4a
        Microprocessor Temp Sensor      0x48   

        PCM 5142 FL/FR                  0x4c
        CS8416                          0x10

        Deprecated
        ------------
        Dolby Decoder STA310            0x60 
        PCM 1681 DAC                    0x4c

        Future  
        -------

    */

    mLCD = new LCD( 0x27, &mI2C );
    mMicroprocessorTemp = new TMP100( 0x48, &mI2C );
    mChannelSel = new ChannelSel_AX2358( 0x4a, &mI2C );

    // PCM5142
    mDAC = new DAC_PCM5142( 0x4c, &mI2C );

    // CS8416
    mSPDIF = new CS8416( 0x10, &mI2C );

    mAudioTimerID = mTimer.setTimer( 1000, mAudioQueue, true );

    // Set up buttons
    mPowerButton = new Button( PIN_BUTTON_POWER, &mAmplifierQueue );
    mVolumeButton = new Button( PIN_BUTTON_VOLUME, &mAmplifierQueue );
    mInputButton = new Button( PIN_BUTTON_INPUT, &mAmplifierQueue );

    mWebServer = new HTTP_Server( &mAmplifierQueue );

    setupRemoteReceiver();
}

void 
Amplifier::asyncUpdateDisplay() {
    mDisplayQueue.add( Message::MSG_DISPLAY_SHOULD_UPDATE );
}

void 
Amplifier::handleDisplayThread() {
    AMP_DEBUG_I( "Starting Display Thread" );
    Message msg;

    // Startup
    mLCD->begin();

    // Let the audio task know the display is up and ready, should it care
    mAmplifierQueue.add( Message::MSG_DISPLAY_DONE_INIT );

    while( true ) {
        while ( mDisplayQueue.waitForMessage( msg, 10 ) ) {
            AMP_DEBUG_I( "Processing Display Queue Message" );

            switch( msg.mMessageType ) {
                case Message::MSG_DISPLAY_SHOULD_UPDATE:
                    updateDisplay();
                    break;
                default:
                    break;
            }
        } 
    } 
}

void
Amplifier::updateDisplay() {
    AmplifierState state = getCurrentState();
    char s[32];
    char d[14];

    sprintf( d, "Vol %ddB", ( -state.mCurrentAttenuation ) );

    if ( state.mConnected ) {
        sprintf( s, "%-12s%8s", d, "Wifi" );
    } else {
        sprintf( s, "%-20s", d );
    }
    mLCD->writeLine( 0, std::string( s ) );

    switch( state.mState ) {
        case AmplifierState::STATE_INIT:
            sprintf( s, "%-12s", "Starting");
            break;
        case AmplifierState::STATE_PLAYING:
            sprintf( s, "%-12s%7.1fC", "Playing", mMicroprocessorTemp->readTemperature() );
            break;
        case AmplifierState::STATE_MUTED:
            sprintf( s, "%-12s", "Muted" );
            break;
        case AmplifierState::STATE_SLEEPING:
            sprintf( s, "%-12s", "Sleeping" );
            break;
        case AmplifierState::STATE_UPDATING:
            sprintf( s, "%-12s", "Updating" );
            break;
        case AmplifierState::STATE_ERROR_MINOR:
            sprintf( s, "%-12s", "Minor Error" );
            break;
        case AmplifierState::STATE_ERROR_MAJOR:
            sprintf( s, "%-12s", "Major Error" );
            break;
    }
    mLCD->writeLine( 1, s );

    char rate[15] = {0};
    if ( state.mAudioType == AmplifierState::AUDIO_ANALOG ) {
        strcpy( rate, "" );
    } else if ( state.mAudioType == AmplifierState::AUDIO_DIGITAL ) {
        if ( state.mSamplingRate == 44100 ) {
            sprintf( rate, "44.1k/%d", state.mBitDepth );
        } else {
            sprintf( rate, "%luk/%d", state.mSamplingRate / 1000, state.mBitDepth );
        }   
    }

    char left[11] = {0};
    switch( state.mAudioType ) {
        case AmplifierState::AUDIO_ANALOG:
            if ( state.mEnhancement ) {
                strcpy( left, "Analog/Enh" );
            }  else {
                strcpy( left, "Analog" );
            }
            sprintf( s, "%-10s%10s", left, rate );
            break;
        case AmplifierState::AUDIO_DOLBY:
            sprintf( s, "%-10s%10s", "AC3", rate );
            break;
        case AmplifierState::AUDIO_DTS:
            sprintf( s, "%-10s%10s", "DTS", rate );
            break;
        case AmplifierState::AUDIO_PCM:
            sprintf( s, "%-10s%10s", "PCM", rate );
            break;
        case AmplifierState::AUDIO_DIGITAL:
            sprintf( s, "%-10s%10s", "Digital", rate );
            break;
    }
    mLCD->writeLine( 2, std::string( s ) );

    char input[24];
    char audioType[10];
    switch( state.mSpeakerConfig ) {
        case AmplifierState::AUDIO_2_CH:
            sprintf( audioType, "2.0" );
            break;
        case AmplifierState::AUDIO_2_DOT_1:
            strcpy( audioType, "2.1" );
            break;
    }

    switch( state.mInput ) {
        case AmplifierState::INPUT_STEREO_1:
            sprintf( input, "%-12s%8s", "TV", audioType );
            break;
        case AmplifierState::INPUT_STEREO_2:
            sprintf( input, "%-12s%8s", "Game", audioType );
            break;
        case AmplifierState::INPUT_STEREO_3:
            sprintf( input, "%-12s%8s", "Vinyl", audioType );
            break;
        case AmplifierState::INPUT_SPDIF_1:
            sprintf( input, "%-12s%8s", "HDMI", audioType );
            break;    
        case AmplifierState::INPUT_SPDIF_2:
            sprintf( input, "%-12s%8s", "Blueray", audioType );
            break;    
        case AmplifierState::INPUT_SPDIF_3:
            sprintf( input, "%-12s%8s", "SPDIF", audioType );
            break;           
    }

    mLCD->writeLine( 3, input );
}

void 
Amplifier::changeAmplifierState( uint8_t newState ) {
    {
        ScopedLock lock( mStateMutex );
        mState.mState = newState;
    }

    asyncUpdateDisplay();
}


void 
Amplifier::handleTimerThread() {
    AMP_DEBUG_I( "Starting Timer Thread on Core %d", xPortGetCoreID() );
    while( true ) {
        mTimer.processTick();

        taskDelayInMs( 10 );
    } 
}

void
Amplifier::updateInput( uint8_t newInput ) {
    AMP_DEBUG_I( "Trying to set input to %d", newInput );
    {
        ScopedLock lock( mStateMutex );

        mState.mInput = newInput;  
        if ( newInput == AmplifierState::INPUT_SPDIF_1 || newInput == AmplifierState::INPUT_SPDIF_2 || newInput == AmplifierState::INPUT_SPDIF_3 ) {
            mState.mAudioType = AmplifierState::AUDIO_DIGITAL;
        } else {
            mState.mAudioType = AmplifierState::AUDIO_ANALOG;
        }
    }

    mAudioQueue.add( Message::MSG_INPUT_SET, mState.mInput );

    asyncUpdateDisplay();
}

void 
Amplifier::doActualRemoteReceive() {       
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // the shortest duration for NEC signal is 560 µs, 1250 ns < 560 µs, valid signal is not treated as noise
        .signal_range_max_ns = 12000000, // the longest duration for NEC signal is 9000 µs, 12000000 ns > 9000 µs, the receive does not stop early
    };

    rmt_receive( mIRChannel, &mIRBuffer[ 0 ], sizeof( mIRBuffer ), &receive_config );               
}

void 
Amplifier::_handleNecRemoteCommand( uint8_t address, uint8_t command ) {
    AMP_DEBUG_I( "Handling NEC Remote addr-command [%d %d]", address, command );
    switch( address ) {
        case 4:
            switch( command ) {
                case 2: 
                    // volume up
                    mAmplifierQueue.add( Message::MSG_VOLUME_UP );
                    break;
                case 3:
                    // volume down
                    mAmplifierQueue.add( Message::MSG_VOLUME_DOWN );
                    break;
            }
            break;
    }
}

void 
Amplifier::handleAmplifierThread() {
    AMP_DEBUG_I( "Starting Amplifier Thread on Core %lu",( uint32_t )xPortGetCoreID() );

    Message msg;
    while( true ) {
        while ( mAmplifierQueue.waitForMessage( msg, 20 ) ) {
           // AMP_DEBUG_I( "Processing Amplifier Queue Message" );

            switch( msg.mMessageType ) {
                case Message::MSG_IR_CODE_RECEIVER:  
                    {
                        const rmt_rx_done_event_data_t *edata = (rmt_rx_done_event_data_t *)msg.mParam;
                        if ( edata->num_symbols == 34 ) {
                            uint8_t data[4] = {0};
                            uint8_t pos = 0;

                            for ( uint8_t i = 0; i < 4 ; i++ ) {
                                for( uint8_t x = 0 ; x < 8; x++ ) {
                                   // AMP_DEBUG_I( "%02d 0: ", edata->received_symbols[ pos + 1 ].duration0 );
                                   // AMP_DEBUG_I( "%02d 1: ", edata->received_symbols[ pos + 1 ].duration1 );

                                    if ( edata->received_symbols[ pos + 1 ].duration1 > 700 ) {
                                        // 1
                                        data[ i ] = ( data[ i ] >> 1 ) | 0x80;
                                    } else {
                                        // 0
                                        data[ i ] = data [ i ] >> 1;
                                    }
                                    pos++;
                                }
                            }

                            AMP_DEBUG_I( "Decoded %#02x %#02x %#02x %#02x", data[ 0 ], data[ 1 ] , data[ 2 ], data[ 3 ] );
                            AMP_DEBUG_I( "Decoded %#02x %#02x %#02x %#02x", data[ 0 ], (uint8_t)~data[ 1 ] , data[ 2 ], (uint8_t)~data[ 3 ] );

                            if ( data[ 0 ] == (uint8_t)~data[ 1 ] && data[ 2 ] == (uint8_t)~data[ 3 ] ) {
                                AMP_DEBUG_I( "Valid address %0x, valid command %0x", data[ 0 ], data[ 2 ] );

                                _handleNecRemoteCommand( data[ 0 ], data[ 2 ] );
                            }
                        }

                        AMP_DEBUG_I( "IR remote control code received [%d]", edata->num_symbols );
                        doActualRemoteReceive();
                    }
                    break;
                case Message::MSG_POWEROFF:
                    activateButtonLight( false );

                    mPoweredOn = false;

                    gpio_set_level( PIN_RELAY, 0 );
                    mLCD->enableBacklight( false );
                    mAudioQueue.add( Message::MSG_AUDIO_SHUTDOWN );

                    break;
                case Message::MSG_POWERON:
                    activateButtonLight( true ); 

                    mPoweredOn = true;

                    mLCD->enableBacklight( true );
                    gpio_set_level( PIN_RELAY, 1 );
                    taskDelayInMs( 1000 );

                    mAudioQueue.add( Message::MSG_AUDIO_RESTART );
                    
                    break;
                case Message::MSG_DISPLAY_SHOULD_UPDATE:
                    // need to update the display
                    break;
                case Message::MSG_DISPLAY_DONE_INIT:
                    // We are done display startup, let's start wifi now
                    mRadioQueue.add( Message::MSG_WIFI_INIT );
                    break;
                case Message::MSG_TIMER:
                    if ( msg.mParam == mTimerID ) {
                        AMP_DEBUG_I( "In Periodic Timer Event, Temp is %0.2f", mMicroprocessorTemp->readTemperature() );
                        asyncUpdateDisplay();
                    } else if ( msg.mParam == mButtonTimerID ) {
                        mVolumeButton->tick();
                        mInputButton->tick();
                        mPowerButton->tick();
                    }
                    break;
                case Message::MSG_DECODER_IRQ:
                    handleDecoderIRQ();
                    break;    
                case Message::MSG_VOLUME_UP:
                    {
                        ScopedLock lock( mStateMutex );

                        uint8_t increase = msg.mParam;
                        if ( increase == 0 ) increase = 1;

                        if ( ( mState.mCurrentAttenuation - increase ) > 0 ) {
                            mState.mCurrentAttenuation = mState.mCurrentAttenuation - increase;
                        } else {
                            mState.mCurrentAttenuation = 0;
                        }
                        
                        asyncUpdateDisplay();

                        mAudioQueue.add( Message::MSG_VOLUME_SET, mState.mCurrentAttenuation );
                    }
                    break;
                case Message::MSG_VOLUME_DOWN:
                    {
                        ScopedLock lock( mStateMutex );

                        uint8_t decrease = msg.mParam;
                        if ( decrease == 0 ) decrease = 1;

                        if ( ( mState.mCurrentAttenuation + decrease ) <= 79 ) {
                            mState.mCurrentAttenuation = mState.mCurrentAttenuation + decrease;
                        } else {
                            mState.mCurrentAttenuation = 79;
                        }

                        asyncUpdateDisplay();

                        mAudioQueue.add( Message::MSG_VOLUME_SET, mState.mCurrentAttenuation );
                    }  
                    break;
                case Message::MSG_INPUT_UP:
                    switch( mState.mInput ) {
                        case AmplifierState::INPUT_STEREO_1:
                            updateInput( AmplifierState::INPUT_STEREO_2 );
                            break;
                        case AmplifierState::INPUT_STEREO_2:
                            updateInput( AmplifierState::INPUT_STEREO_3 );
                            break;
                        case AmplifierState::INPUT_STEREO_3:
                            updateInput( AmplifierState::INPUT_SPDIF_1 );
                            break;
                        case AmplifierState::INPUT_SPDIF_1:
                            updateInput( AmplifierState::INPUT_SPDIF_2 );
                            break;  
                        case AmplifierState::INPUT_SPDIF_2:
                            updateInput( AmplifierState::INPUT_SPDIF_3 );
                            break;   
                        case AmplifierState::INPUT_SPDIF_3:
                            updateInput( AmplifierState::INPUT_STEREO_1 );
                            break;     
                        /*
                        case AmplifierState::INPUT_6CH:
                            updateInput( AmplifierState::INPUT_STEREO_1 );
                            break;
                        */
                        default:
                            break;
                    }
                    break;
                case Message::MSG_INPUT_DOWN:
                    switch( mState.mInput ) {
                        case AmplifierState::INPUT_STEREO_1:
                            updateInput( AmplifierState::INPUT_SPDIF_3 );
                            break;
                        case AmplifierState::INPUT_STEREO_2:
                            updateInput( AmplifierState::INPUT_STEREO_1 );
                            break;
                        case AmplifierState::INPUT_STEREO_3:
                            updateInput( AmplifierState::INPUT_STEREO_2 );
                            break;
                        case AmplifierState::INPUT_SPDIF_1:
                            updateInput( AmplifierState::INPUT_STEREO_3 );
                            break;
                        case AmplifierState::INPUT_SPDIF_2:
                            updateInput( AmplifierState::INPUT_SPDIF_1 );
                            break;  
                        case AmplifierState::INPUT_SPDIF_3:
                            updateInput( AmplifierState::INPUT_SPDIF_2 );
                            break;          
                        /*
                        case AmplifierState::INPUT_6CH:
                            updateInput( AmplifierState::INPUT_STEREO_4 );
                            break;
                        */
                        default:
                            break;
                    }     
                    break;
                case Message::MSG_INPUT_SET:
                    updateInput( msg.mParam );
                    break;
                case Message::MSG_BUTTON_PRESSED:
                    AMP_DEBUG_I(  "Button pressed! %lu", msg.mParam );
                    switch( msg.mParam ) {
                        case PIN_BUTTON_POWER:
                            handlePowerButtonPress();
                            break;
                        case PIN_BUTTON_VOLUME:
                            handleVolumeButtonPress();
                            break;
                        case PIN_BUTTON_INPUT:
                            handleInputButtonPress();
                            break;
                        default:
                            break;
                    }
                    break;  
                case Message::MSG_BUTTON_RELEASED:
                    AMP_DEBUG_I(  "Button released! %lu", msg.mParam );    
                    break;          
                default:
                    break;
            }
        } 
    } 
}   

void 
Amplifier::handleDecoderIRQ() {
    AMP_DEBUG_I( "Decoder Interrupt" );  
  //  mDolbyDecoder->handleInterrupt( mAudioQueue );
}

void
Amplifier::handlePowerButtonPress() {
    AMP_DEBUG_I( "Power button pressed" );

    if ( mPoweredOn ) {
        mAmplifierQueue.add( Message::MSG_POWEROFF );
    } else {
        mAmplifierQueue.add( Message::MSG_POWERON );
    }
}

void 
Amplifier::handleInputButtonPress() {
    AMP_DEBUG_I( "Input button pressed" );

    {
        ScopedLock lock( mStateMutex );

        mState.mEnhancement = !mState.mEnhancement;

        mAudioQueue.add( Message::MSG_AUDIO_SET_ENHANCEMENT, mState.mEnhancement );
    }
}

void 
Amplifier::handleVolumeButtonPress() {
    AMP_DEBUG_I( "Volume button pressed" );
}


void 
Amplifier::startDigitalAudio() {
    taskDelayInMs( 10 );

    // Dolby Decoder is muted, but running, so it's outputting zeros and a clock to the DAC
    AMP_DEBUG_I( "Starting SPDIF/DAC initialization" );

    // enable CS8416
    gpio_set_level( PIN_CS8416_RESET, 1 );
    taskDelayInMs( 10 );

    mSpdifTimerID = mTimer.setTimer( 1000, mAudioQueue, true );

    // Initialize SPDIF stream, but don't run

    AMP_DEBUG_I( "Starting SPDIF initialization" );

    mSPDIF->init();

    AMP_DEBUG_I( "Starting DAC" );
    mDAC->init();
    mDAC->setFormat( DAC::FORMAT_I2S );
    mDAC->setAttenuation( 0 );

    // set the proper input channel
    AmplifierState state = getCurrentState();
    mSPDIF->setInput( state.mInput );

    AMP_DEBUG_I( "Telling SPDIF to run" );
    // start decoding
    mSPDIF->run();

    taskDelayInMs( 10 );

    AMP_DEBUG_I( "Enabling the DAC" );
    mDAC->enable( true );

    mDigitalAudioStarted = true;
}

void 
Amplifier::stopDigitalAudio() {
    AMP_DEBUG_I( "Stopping SPDIF/DAC" );

    mTimer.cancelTimer( mSpdifTimerID );
    mSpdifTimerID = 0;

    mDAC->enable( false );
    mSPDIF->run( false );

    // disable CS8416
    gpio_set_level( PIN_CS8416_RESET, 0 );

    mDigitalAudioStarted = false;
}

void 
Amplifier::handleAudioThread() {
    AMP_DEBUG_I( "Starting Audio Thread" );

    // enable reset of the CS8416
    gpio_set_level( PIN_CS8416_RESET, 0 );

    taskDelayInMs( 10 );

    // Channel Selector is now muted - need to umuted when audio is ready to go
    mChannelSel->init();
    mChannelSel->mute( true );

    AMP_DEBUG_I( "Relay active" );

    gpio_set_level( PIN_RELAY, 1 );

    taskDelayInMs( 500 );

    AMP_DEBUG_I( "Decoder set into active mode" );   

     // Initial volume, 0-79
    AmplifierState state = getCurrentState();

    AMP_DEBUG_I( "Setting involumeput" );

    mChannelSel->setAttenuation( state.mCurrentAttenuation );
    mChannelSel->setEnhancement( state.mEnhancement );

    AMP_DEBUG_I( "Setting input" );
  
    if ( state.mInput == AmplifierState::INPUT_SPDIF_1 || state.mInput == AmplifierState::INPUT_SPDIF_2 || state.mInput == AmplifierState::INPUT_SPDIF_3 ) {
        mSPDIF->setInput( state.mInput );
        mChannelSel->setInput( ChannelSel::INPUT_STEREO_1 ); 

        startDigitalAudio();
    } else {
        mChannelSel->setInput( state.mInput ); 
    }

    mI2C.scanBus();

    asyncUpdateDisplay();

    mChannelSel->mute( false );
    AMP_DEBUG_I( "Channel selector un-muted" );
    
    // Set to playing status
    changeAmplifierState( AmplifierState::STATE_PLAYING );

    Message msg;
    while( true ) {
        while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
            switch( msg.mMessageType ) {
                case Message::MSG_AUDIO_RESTART:
                    {
                        AmplifierState state = getCurrentState();
                        if ( state.mInput == AmplifierState::INPUT_SPDIF_1 || state.mInput == AmplifierState::INPUT_SPDIF_2 || state.mInput == AmplifierState::INPUT_SPDIF_3 ) {
                            startDigitalAudio();
                        }
                    }
                    break;
                case Message::MSG_AUDIO_SHUTDOWN:
                    {
                        AmplifierState state = getCurrentState();
                        if ( state.mInput == AmplifierState::INPUT_SPDIF_1 || state.mInput == AmplifierState::INPUT_SPDIF_2 || state.mInput == AmplifierState::INPUT_SPDIF_3 ) {
                            stopDigitalAudio();
                        }
                    }
                    break;
                case Message::MSG_VOLUME_SET:
                    AMP_DEBUG_I( "Setting pending audio volume to %lu", msg.mParam );
                    mPendingVolumeChange = true;
                    mPendingVolume = msg.mParam;
                    
                    break;
                case Message::MSG_INPUT_SET:
                    AMP_DEBUG_I( "Setting audio input to %lu", msg.mParam );

                    if ( msg.mParam != AmplifierState::INPUT_SPDIF_1 && msg.mParam != AmplifierState::INPUT_SPDIF_2 && msg.mParam != AmplifierState::INPUT_SPDIF_3 ) {
                        // We are in Dolby, but moving away, so we need to shut down the digital audio
                        AMP_DEBUG_I( "Stopping Digital Audio" );
                        stopDigitalAudio();

                        // set it to proper value
                        mChannelSel->setInput( msg.mParam );
                    } else if ( msg.mParam == AmplifierState::INPUT_SPDIF_1 || msg.mParam == AmplifierState::INPUT_SPDIF_2 || msg.mParam == AmplifierState::INPUT_SPDIF_3  ) {
                        // We need to start up the Dolby digital decoder
                        AMP_DEBUG_I( "Starting Digital Audio" );
                        mSPDIF->setInput( msg.mParam );
                        
                        startDigitalAudio();

                        // All SPDIF goes into the stereo 1 input for now
                        mChannelSel->setInput( ChannelSel::INPUT_STEREO_1 );
                    }
                    
                    break;
                case Message::MSG_TIMER: 
                    if ( msg.mParam == mSpdifTimerID ) {
                          if ( mDigitalAudioStarted  ) {
                            // let's scan the SPDIF and see what the sampling rate is
                            if ( mSPDIF->hasLoopLock() ) {
                                AMP_DEBUG_I( "Has loop lock, free memory is %lu", esp_get_free_heap_size() );

                                bool changed = false;

                                AmplifierState state = getCurrentState();
                                uint16_t samplingRate = mSPDIF->getSamplingRate();
                                uint16_t bitDepth = mSPDIF->getBitDepth();

                                {
                                    ScopedLock lock( mStateMutex );    

                                    AMP_DEBUG_I( "Sampling rate is [%d]", samplingRate );
                                    if ( samplingRate != state.mSamplingRate ) {
                                        mState.mSamplingRate = samplingRate;
                                        changed = true;
                                    }

                                   
                                    AMP_DEBUG_I( "Bit depth is [%d]", bitDepth );
                                    if ( bitDepth != state.mBitDepth ) {
                                    
                                        mState.mBitDepth = bitDepth;
                                        changed = true;
                                    }
                                }

                                // update the display from the display thread
                                if ( changed ) {
                                    asyncUpdateDisplay();
                                }
                            } else {
                                AMP_DEBUG_I( "SPDIF has lost sync, error byte is [%#02x]", mSPDIF->getErrorInfo() );
                            }
                        }                  
                    } else if ( msg.mParam == mAudioTimerID ) {
                        // we are on the audio timer thread
                        if ( mPendingVolumeChange ) {
                            AMP_DEBUG_I( "Actually setting pending audio volume to %lu", mPendingVolume );
                            bool result = mChannelSel->setAttenuation( mPendingVolume );
                            if ( result ) {
                                // if it failed, we will try again shortly
                                mPendingVolumeChange = false;
                            }
                        }
                    } 
                    
                    break;
                case Message::MSG_AUDIO_SAMPLING_RATE_CHANGE:
                    {
                        ScopedLock lock( mStateMutex );
                        mState.mSamplingRate = msg.mParam;
                    }

                    asyncUpdateDisplay();
                    break;
                case Message::MSG_AUDIO_SET_ENHANCEMENT:
                    AMP_DEBUG_I( "Actually setting enhancement to %lu", msg.mParam );
                    mChannelSel->setEnhancement( msg.mParam );
                    
                    asyncUpdateDisplay();
                    break;
                default:
                    break;
            }
        } 
    } 
}

static void wifi_event_handler( void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data ){
    (( Amplifier *)event_handler_arg)->_handleWifiCallback( event_id );
}

void
Amplifier::_handleWifiCallback( int32_t event_id ) {
    mRadioQueue.add( Message::MSG_WIFI_UPDATE, event_id );
}

void
Amplifier::handleWifiCallback( int32_t event_id ) {
    AMP_DEBUG_I( "Processing Wifi Callback" ); 
    switch( event_id ) {
        case WIFI_EVENT_STA_START:
            AMP_DEBUG_I( "Wifi Starting Up" );
            break;
        case WIFI_EVENT_STA_CONNECTED:
            AMP_DEBUG_I( "Wifi Connection Established" );

            updateConnectedStatus( true );
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            AMP_DEBUG_I( "Wifi Disconnected" );
            if ( mWifiEnabled ) {
                AMP_DEBUG_I( "Setting Reconnection Timer" );
                mReconnectTimerID = mTimer.setTimer( 10000, mRadioQueue, false );
            }
            updateConnectedStatus( false );
            break;
        case IP_EVENT_STA_GOT_IP:
            AMP_DEBUG_I( "Wifi Received IP Address" );
            updateTimeFromNTP();
            break;
    }
}

void NTP_Callback(struct timeval *tv) {
    AMP_DEBUG_I( "Received NTP Callback" );  
}

 void 
 Amplifier::updateTimeFromNTP() {
    AMP_DEBUG_I( "Attemping To Update Time From NTP Server" );
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG( "pool.ntp.org" );

    esp_netif_sntp_init( &config );
    sntp_set_time_sync_notification_cb( &NTP_Callback );

    setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
    tzset();

    mUpdatingFromNTP = true;
    esp_netif_sntp_start();
 }

void 
Amplifier::setupWifi() {
    esp_netif_init();
    esp_event_loop_create_default(); 
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, (void*)this );
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, (void*)this );//creating event handler register for ip event
    wifi_config_t wifi_config;
    memset( &wifi_config, 0, sizeof( wifi_config ) );

    strcpy( (char*)wifi_config.sta.ssid, (char*)"The Grey Havens (LR)" );
    strcpy( (char*)wifi_config.sta.password, (char*)"brazil1234!" );

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config( WIFI_IF_STA, &wifi_config );

    mWifiEnabled = true;

    esp_wifi_start();

   // Bluetooth::instance().init();

    // Bluetooth
    /*
    uint32_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init( &bt_cfg );
    ret = esp_bt_controller_enable( ESP_BT_MODE_BLE );
    ret = esp_bluedroid_init();
    ret = esp_bluedroid_enable();
    ret = esp_ble_gatts_register_callback(gatts_callback);
    ret = esp_ble_gap_register_callback(gap_callback);
    ret = esp_ble_gatts_app_register( 0x01 );
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(512);
    */
}

void 
Amplifier::attemptWifiConnect() {
    AMP_DEBUG_I( "Attempting to connect Wifi" );

    mWifiConnectionAttempts = 0;

    AMP_DEBUG_I( "Calling wifi connect" );
    esp_wifi_connect();
    AMP_DEBUG_I( "Wifi connect done" );
}

void
Amplifier::handleRadioThread() {
    AMP_DEBUG_I( "Starting Radio Thread on Core %lu", (uint32_t)xPortGetCoreID() );

    setupWifi();
    attemptWifiConnect();

    Message msg;
    while( true ) {
        if ( mUpdatingFromNTP ) {
            if ( SNTP_SYNC_STATUS_COMPLETED == sntp_get_sync_status() ) {
                mUpdatingFromNTP = false;

                time_t now;
                struct tm timeinfo;
                char strftime_buf[64];

                time(&now);
                localtime_r(&now, &timeinfo);
                strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
                
                AMP_DEBUG_I("The current date/time is: %s", strftime_buf  );  

                esp_netif_sntp_deinit();
            }
        }

        while ( mRadioQueue.waitForMessage( msg, 10 ) ) {
            AMP_DEBUG_I( "Processing Radio Queue Message" );
            switch( msg.mMessageType ) {
                case Message::MSG_WIFI_INIT:
                   
                    break;
                case Message::MSG_WIFI_UPDATE:
                    handleWifiCallback( msg.mParam );
                    break;
                case Message::MSG_TIMER:
                    if ( msg.mParam == mReconnectTimerID ) {
                        AMP_DEBUG_I( "In Reconnection Timer Event" );
                        attemptWifiConnect();
                        mReconnectTimerID = 0;
                    } else if ( msg.mParam == mTimerID ) {
                        AMP_DEBUG_I( "In Periodic Timer Event" );
                    } else {
                        AMP_DEBUG_I( "Unknown timer event %lu", msg.mParam );
                    }
                   
                    break;
                default:
                    break;
            }
        } 
    } 
}

void 
Decoder_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleDecoderISR();
}

void 
Power_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handlePowerButtonISR();
}

void 
Volume_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleVolumeButtonISR();
}

void 
Volume_Button_Encoder_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleVolumeButtonEncoderISR();
}

void 
Input_Button_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleInputButtonISR();
}

void
Input_Button_Encoder_ISR( void *arg ) {
    (( Amplifier * )arg)->_handleInputButtonEncoderISR();
}

void 
Amplifier::_handleDecoderISR() {
    mAmplifierQueue.addFromISR( Message::MSG_DECODER_IRQ );
}

void 
Amplifier::_handlePowerButtonISR() {
    if ( mPowerButton ) {
        mPowerButton->handleInterrupt();
    }
}

void
Amplifier::_handleVolumeButtonISR() {
   if ( mVolumeButton ) {
        mVolumeButton->handleInterrupt();
    }
}

void 
Amplifier::_handleInputButtonISR() {
   if ( mInputButton ) {
        mInputButton->handleInterrupt();
    }
}

void 
Amplifier::_handleVolumeButtonEncoderISR() {
    ENCODER_DIR direction = mVolumeEncoder.process();
    switch( direction ) {
        case ENCODER_FORWARD:
            mAmplifierQueue.addFromISR( Message::MSG_VOLUME_UP );
            break;
        case ENCODER_BACKWARD:
            mAmplifierQueue.addFromISR( Message::MSG_VOLUME_DOWN );
            break;
        default:
            break;
    }
}

void 
Amplifier::_handleInputButtonEncoderISR() {
    ENCODER_DIR direction = mInputEncoder.process();
    switch( direction ) {
        case ENCODER_FORWARD:
            mAmplifierQueue.addFromISR( Message::MSG_INPUT_UP );
            break;
        case ENCODER_BACKWARD:
            mAmplifierQueue.addFromISR( Message::MSG_INPUT_DOWN );
            break;
        default:
            break;
    }
}

void 
Amplifier::setupPWM() {
    ledc_timer_config_t backlight_config = {};
    backlight_config.speed_mode         = LEDC_HIGH_SPEED_MODE;
    backlight_config.duty_resolution    = LEDC_TIMER_10_BIT;
    backlight_config.timer_num          = LEDC_TIMER_0;
    backlight_config.freq_hz            = 200;
    backlight_config.clk_cfg            = LEDC_USE_REF_TICK;

    ledc_channel_config_t ledc_channel = {};
    ledc_channel.gpio_num   = PIN_LED_FRONT_STANDBY;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_0;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;
    ledc_channel.flags.output_invert = 0;

    ledc_timer_config( &backlight_config );
    ledc_channel_config( &ledc_channel );
    
    ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 50 );
	ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0 );

    ledc_channel.gpio_num   = PIN_LED_ACTIVE;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.channel    = LEDC_CHANNEL_1;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;
    ledc_channel.flags.output_invert = 0;

    ledc_channel_config( &ledc_channel );

    ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 1023 );
	ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1 );
}

void 
Amplifier::activateButtonLight( bool activate ) {
    if ( activate ) {
        ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 700 );
	    ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1 );
    } else {
        ledc_set_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0 );
	    ledc_update_duty( LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1 );
    }

}

void
Amplifier::configurePins() {
    AMP_DEBUG_I( "Configuring pins" );
    gpio_install_isr_service( 0 );

    // Configure LEDs
   // configureOnePin( PIN_LED_ACTIVE, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    
    // The standby power LED indicator
 //   configureOnePin( PIN_LED_FRONT_STANDBY, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_DISABLE );

    AMP_DEBUG_I( "Activating standby" );
   // gpio_set_level( PIN_LED_FRONT_STANDBY, 1 );

    configureOnePin( PIN_LED_FRONT_POWER, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_LED_FRONT_POWER, 1 );

    // Configure Output Triggers
    configureOnePin( PIN_RELAY, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_RELAY, 0 );

    // Configure Buttons
    configureOnePin( PIN_BUTTON_VOLUME, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_BUTTON_VOLUME, Volume_Button_ISR, this );

    configureOnePin( GPIO_NUM_15, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_15, Volume_Button_Encoder_ISR, this );

    configureOnePin( GPIO_NUM_13, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_13, Volume_Button_Encoder_ISR, this );

    configureOnePin( PIN_BUTTON_INPUT, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_BUTTON_INPUT, Input_Button_ISR, this );

    configureOnePin( GPIO_NUM_4, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_4, Input_Button_Encoder_ISR, this );

    configureOnePin( GPIO_NUM_16, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( GPIO_NUM_16, Input_Button_Encoder_ISR, this );

    configureOnePin( PIN_BUTTON_POWER, GPIO_INTR_ANYEDGE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_BUTTON_POWER, Power_Button_ISR, this );

    // Configure Others
    configureOnePin( PIN_BACKLIGHT, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_BACKLIGHT, 1 );

    // Decoder reset
    configureOnePin( PIN_DECODER_RESET, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_DECODER_RESET, 0 );

    // SPDIF reset
    configureOnePin( PIN_CS8416_RESET, GPIO_INTR_DISABLE, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ENABLE, GPIO_PULLUP_DISABLE );
    gpio_set_level( PIN_CS8416_RESET, 0 );

    // Setting up decoder
    configureOnePin( PIN_DECODER_IRQ, GPIO_INTR_DISABLE, GPIO_MODE_INPUT, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_ENABLE );
    gpio_isr_handler_add( PIN_DECODER_IRQ, Decoder_ISR, this );
    gpio_set_intr_type( PIN_DECODER_IRQ, GPIO_INTR_NEGEDGE );
}

void 
Amplifier::configureOnePin( PIN pin, gpio_int_type_t interrupts, gpio_mode_t mode, gpio_pulldown_t pulldown, gpio_pullup_t pullup ) {
    uint_fast32_t mask = ( ((uint_fast32_t)1) << ( uint_fast32_t ) pin );
    gpio_config_t io_conf;

	io_conf.intr_type = interrupts;
	io_conf.mode = mode;
	io_conf.pin_bit_mask = mask;
	io_conf.pull_down_en = pulldown;
	io_conf.pull_up_en = pullup;

	gpio_config( &io_conf );   
}
