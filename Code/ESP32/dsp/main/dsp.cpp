#include "dsp.h"
#include "debug.h"

DSP::DSP() : mSamplingRate( 48000 ), mSamplesPerPayload( 0 ), mBytesPerPayload( 0 ), mBuffer( 0 ), mBitDepth( 16 ), mSlotDepth( 16 ), mMode( MODE_BYPASS ), mI2C( 0 ), mI2S( 0 ), mAudioStarted( false ) {

}

DSP::~DSP() {

}

void 
startAudioThread( void *param ) {
    // Proxy thread start to main amplifier thread
    ((DSP *)param)->handleAudioThread();
}

void
startGeneralThread( void *param ) {
    // Proxy thread start to main radio thread
    ((DSP *)param)->handleGeneralThread();
}

void 
DSP::fullReset() {
    stopAudio();

    mAudioStarted = false;

    mSamplingRate = 48000;
    mBitDepth = 16;
    mSlotDepth = 16;

    mMode = MODE_BYPASS;

    mPipeline->resetAll();
}

void
DSP::start() {
    xTaskCreate(
        startAudioThread,
        "Audio Thread",
        1000,
        (void *)this,
        2,
        NULL
    );   

    xTaskCreate(
        startGeneralThread,
        "General Thread",
        1000,
        (void *)this,
        1,
        NULL
    );
}

uint8_t 
DSP::sizePerSample() {
    switch( mBitDepth ) {
        case 16:
            return 2;
        case 24:
        case 32:
            return 4;
        default:
            return 0;
    }
}

void
DSP::processAudio() {
    while ( mI2S->bytesWaiting() >= mBytesPerPayload ) {
        size_t receivedSize = 0;
        size_t writtenSize = 0;
        
        mI2S->readData( mBuffer, mBytesPerPayload, receivedSize );
        if ( mMode == MODE_DSP ) {
            if ( mBytesPerPayload == receivedSize ) {
                uint8_t *processed = 0;
                switch( mBitDepth ) {
                    case 16:    
                        processed = (uint8_t *)mPipeline->process( (int16_t*)mBuffer );
                        mI2S->writeData( processed, mBytesPerPayload, writtenSize );
                        break;
                    case 24:
                    case 32:
                        processed = (uint8_t *)mPipeline->process( (int32_t*)mBuffer );
                        mI2S->writeData( processed, mBytesPerPayload, writtenSize );
                        break;
                }
            } else {
                // ERROR
                AMP_DEBUG_E( "Unexpected read size %d", receivedSize );
            }                  
        } else if ( mMode == MODE_BYPASS ) {
            mI2S->writeData( mBuffer, mBytesPerPayload, writtenSize );
        }
    }
}

void 
DSP::handleAudioThread() {
    AMP_DEBUG_I( "Starting audio thread" );
    Message msg;

    mI2S = new I2S( mAudioQueue );

    mPipeline = new Pipeline();
    mPipeline->addBiquadLeft( new Biquad( Biquad::LOWPASS, 2000, 0.707 ) );
    mPipeline->addBiquadLeft( new Biquad( Biquad::LOWPASS, 2000, 0.707 ) );
    mPipeline->addBiquadRight( new Biquad( Biquad::HIGHPASS, 2000, 0.5 ) );

    while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
        switch( msg.mMessageType ) {
            case Message::MSG_I2S_RECV:
                processAudio();
                break;
            case Message::MSG_AUDIO_START:
                startAudio();
                break;
            case Message::MSG_AUDIO_STOP:
                stopAudio();
                break;
            case Message::MSG_AUDIO_ADD_FILTER_LEFT:
                mPipeline->addBiquadLeft( (Biquad *)msg.mParam );
                break;
            case Message::MSG_AUDIO_ADD_FILTER_RIGHT:
                mPipeline->addBiquadRight( (Biquad *)msg.mParam );
                break;
            case Message::MSG_AUDIO_FULL_RESET:
                fullReset();
                break;
            default:
                break;
        }
    }
}

void
DSP::handleGeneralThread() {
    AMP_DEBUG_I( "Starting general thread" );
    Message msg;

    mI2C = new I2CBUS( getI2CAddress(), mGeneralQueue );
    
    while ( mGeneralQueue.waitForMessage( msg, 5 ) ) {
        switch( msg.mMessageType ) {
            case Message::MSG_I2C:
                mI2C->processData();
                break;
            default:
                break;
        }
    }
}

void 
DSP::startAudio() {
    mSamplesPerPayload = ( mSamplingRate * DSP_I2S_PACKET_SIZE_MS ) / 1000;
    mBytesPerPayload = 2 * sizePerSample() * mSamplesPerPayload;

    mPipeline->setSamples( mSamplesPerPayload );

    mBuffer = (uint8_t *)aligned_alloc( sizePerSample(), sizePerSample() * 2 * mSamplesPerPayload );

    mI2S->start( mSamplingRate, mBitDepth, mSlotDepth );
}

void 
DSP::stopAudio() {
    mI2S->stop();
    mPipeline->destroy();

    if ( mBuffer ) {
        free( mBuffer );
        mBuffer = 0;
    }
}

void 
DSP::onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize ) {
    // this will be on the general thread
    switch( reg ) {
        case REGISTER_RESET:
            if ( dataSize == 1 && !mAudioStarted ) {
                uint8_t command = buffer[ 0 ];
                if ( command == 1 ) {
                    // perform full reset
                    mAudioQueue.add( Message::MSG_AUDIO_FULL_RESET );
                }
            }
            break;
        case REGISTER_SET_MODE:
            if ( dataSize == 1 && !mAudioStarted ) {
                uint8_t command = ( buffer[ 0 ] & 0x0f );
                if ( command == 1 ) {
                    // filtering mode
                    mMode = MODE_DSP;
                } else {
                    mMode = MODE_BYPASS;
                }

                uint8_t outputFormat = ( buffer[ 0 ] & 0xf0 ) >> 4;
                switch( outputFormat ) {
                    case 0:
                        // left and right processed individually, default
                        break;
                    case 1:
                        // left and right combined, in left channel, no attenuation
                        break;
                    case 2:
                        // left and right combined, in left channel, 6db decrease
                        break;
                }
            }
            break;
        case REGISTER_SET_FREQ_DEPTH:
            if ( dataSize == 1 && !mAudioStarted ) {
                uint8_t bitDepth = ( buffer[ 0 ] & 0x0f );
                switch( bitDepth ) {
                    case 0:
                        mBitDepth = 16;
                        mSlotDepth = 16;
                        break;
                    case 1:
                        // might be for 24 bit on 24 bit frames
                        mBitDepth = 24;
                        mSlotDepth = 24;
                        break;
                    case 2:
                        // might be for 24 bit on 32 bit frames
                        mBitDepth = 24;
                        mSlotDepth = 32;
                        break;
                    case 3:
                        mBitDepth = 32;
                        mSlotDepth = 32;
                        break;
                }

                uint8_t samplingRate = ( buffer[ 0 ] & 0xf0 ) >> 4;
                switch( samplingRate ) {
                    case 0:
                        mSamplingRate = 48000;
                        break;
                    case 1:
                        mSamplingRate = 44100;
                        break;
                    case 2:
                        mSamplingRate = 96000;
                        break;
                    case 3:
                        mSamplingRate = 192000;
                        break;
                }
            }
            break;
        case REGISTER_ADD_FILTER:
            if ( dataSize >= 9 && !mAudioStarted ) {
                uint8_t filterType = buffer[ 0 ] & 0x1f;
                uint8_t filterLoc = ( buffer[ 0 ] & 0xe0 ) >> 5;

                float cutoffFreq = *((float *)&buffer[1]);
                float qFactor = *((float *)&buffer[5]);
                float gain = 1.0;

                if ( dataSize == 13 ) {
                    gain = *((float *)&buffer[9]);
                }

                if ( filterLoc & 0b001 ) {  
                    // add right
                    mAudioQueue.add( Message::MSG_AUDIO_ADD_FILTER_RIGHT, (uint32_t)new Biquad( filterType, cutoffFreq, qFactor, gain ) );
                } 
                
                if ( filterLoc & 0b010 ) {
                    // add left
                    mAudioQueue.add( Message::MSG_AUDIO_ADD_FILTER_LEFT, (uint32_t)new Biquad( filterType, cutoffFreq, qFactor, gain ) );
                }
            }
            break;
        case REGISTER_START:
            if ( dataSize == 1 ) {
                uint8_t command = ( buffer[ 0 ] & 0x0f );
                if ( command == 1 ) {
                    // start
                    if ( !mAudioStarted ) {
                        mAudioStarted = false;
                        mAudioQueue.add( Message::MSG_AUDIO_START );
                    }
                    
                } else if ( command == 0 ) {
                    // standby
                    if ( mAudioStarted ) {
                        mAudioStarted = false;
                        mAudioQueue.add( Message::MSG_AUDIO_STOP );
                    }
                    
                }
            }
 
            break;
        case REGISTER_VERSION:
            break;
    }
}

uint8_t 
DSP::getI2CAddress() {
    uint8_t addr = DSP_I2C_BASE_ADDR & 0xf8;
    uint8_t addr0 = ( gpio_get_level( DSP_PIN_ADDR_0 ) );
    uint8_t addr1 = ( gpio_get_level( DSP_PIN_ADDR_1 ) << 1 );
    uint8_t addr2 = ( gpio_get_level( DSP_PIN_ADDR_2 ) << 2 );

    return addr | addr0 | addr1 | addr2;
}