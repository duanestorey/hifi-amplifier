#include "dsp.h"
#include "debug.h"
#include "timer.h"
#include <memory.h>
#include "dsps_tone_gen.h"

DSP::DSP() : mSamplingRate( 48000 ), mSamplesPerPayload( 0 ), mBytesPerPayload( 0 ), mBitDepth( 32 ), mSlotDepth( 32 ), 
    mMode( MODE_BYPASS ), mI2C( 0 ), mI2S( 0 ), mTimer( 0 ), mProfile( 0 ), mAudioStarted( false ), mTimerID( 0 ), mCpuUsage( 0 ) {

    mTimer = new Timer();
    mProfile = new Profile();
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
startTimerThread( void *param ) {
    // Proxy thread start to main radio thread
    ((DSP *)param)->handleTimerThread();
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
        startGeneralThread,
        "General Thread",
        8192,
        (void *)this,
        1,
        NULL
    );

    xTaskCreate(
        startAudioThread,
        "Audio Thread",
        8192,
        (void *)this,
        2,
        NULL
    );   

    while ( true ) {
        vTaskDelay( 10 / portTICK_PERIOD_MS );
    }
}

uint8_t 
DSP::sizePerSample() {
    switch( mSlotDepth ) {
        case 16:
            return 4;
        case 24:
            return 6;
        case 32:
            return 8;
        default:
            return 0;
    }
}

void 
DSP::writeSplitData( uint8_t *bufferPtr1, uint32_t writeSize1, uint8_t *bufferPtr2, uint32_t writeSize2, uint8_t bytesPerSample ) {
    size_t writtenSize1 = 0;
    size_t writtenSize2 = 0;

    uint8_t *processed = (uint8_t *)mPipeline->process( (int16_t*)bufferPtr1, writeSize1/bytesPerSample );
    mI2S->writeData( processed, writeSize1, writtenSize1 );

    if ( writeSize2 ) {
        processed = (uint8_t *)mPipeline->process( (int16_t*)bufferPtr2, writeSize2/bytesPerSample );
        mI2S->writeData( processed, writeSize2, writtenSize2 );
    }
}

void
DSP::processAudio() {
    AMP_DEBUG_I( "Processing audio" );
    while ( mI2S->bytesWaiting() >= mBytesPerPayload ) {
        size_t receivedSize1 = 0;
        size_t receivedSize2 = 0;
        size_t writtenSize1 = 0;
        size_t writtenSize2 = 0;
        uint8_t *bufferPtr1 = 0;
        uint8_t *bufferPtr2 = 0;

        mI2S->readData( mBytesPerPayload, bufferPtr1, receivedSize1, bufferPtr2, receivedSize2 );
        if ( mMode == MODE_DSP ) {
            if ( mBytesPerPayload == ( receivedSize1 + receivedSize2 ) ) {
                switch( mBitDepth ) {
                    case 16:    
                        writeSplitData( bufferPtr1, receivedSize1, bufferPtr2, receivedSize2, 4 );
                        break;
                    case 24:    
                        writeSplitData( bufferPtr1, receivedSize1, bufferPtr2, receivedSize2, mSlotDepth/4 );
                        break;
                    case 32:
                        writeSplitData( bufferPtr1, receivedSize1, bufferPtr2, receivedSize2, 8 );
                        break;
                }
            } else {
                // ERROR
                AMP_DEBUG_E( "Unexpected read size %d", receivedSize1 + receivedSize2 );
            }                  
        } else if ( mMode == MODE_BYPASS ) {
            if ( receivedSize1 ) {
                mI2S->writeData( bufferPtr1, receivedSize1, writtenSize1 );
            }

            if ( receivedSize2 ) {
                mI2S->writeData( bufferPtr2, receivedSize2, writtenSize2 );
            }
        }
    }
}

void 
DSP::handleTimerThread() {
    AMP_DEBUG_I( "Starting timer thread" );
    while( true ) {
        mTimer->processTick();

        vTaskDelay( 10 / portTICK_PERIOD_MS );
    }     
}

void 
DSP::handleAudioThread() {
    AMP_DEBUG_I( "Starting audio thread" );
    Message msg;

    AMP_DEBUG_I( "Setting up pipeline" );
    mPipeline = new Pipeline( mProfile );

    mSamplingRate = 48000;
    uint32_t packets = 40;
    uint32_t packetSamples = mSamplingRate/packets;
    float *tone = (float*)malloc( packetSamples * sizeof( float ) );
    for ( int i = 0 ; i < packetSamples; i++ ) {
        tone[ i ] = 1000.0f * ( sin ( 2 * M_PI * 50 * i / mSamplingRate ) +  sin ( 2 * M_PI * 5000 * i / mSamplingRate ) );
    }

    int32_t *buffer = (int32_t *)aligned_alloc( sizeof( int32_t ), 2 * sizeof( int32_t ) * packetSamples );
    for ( int i = 0 ; i < packetSamples; i++ ) {
        buffer[i*2] = (float)tone[ i ];
        buffer[i*2+1] = (float)tone[ i ];
    }

    AMP_DEBUG_I( "Setting up timer" );
    mTimerID = mTimer->setTimer( 1000, mAudioQueue, true );

    AMP_DEBUG_I( "Adding Biquad filters" );

    mPipeline->addBiquad( Pipeline::CHANNEL_LEFT, new Biquad( Biquad::LOWPASS, 1400, 0.707 ) );
    mPipeline->addBiquad( Pipeline::CHANNEL_LEFT, new Biquad( Biquad::LOWPASS, 1400, 0.707  ) );
    mPipeline->addBiquad( Pipeline::CHANNEL_RIGHT, new Biquad( Biquad::HIGHPASS, 1400, 0.707  ) );
    mPipeline->addBiquad( Pipeline::CHANNEL_RIGHT, new Biquad( Biquad::HIGHPASS, 1400, 0.707  ) );

    mPipeline->setSource( Pipeline::CHANNEL_LEFT, Pipeline::SOURCE_LEFT );
    mPipeline->setSource( Pipeline::CHANNEL_RIGHT, Pipeline::SOURCE_RIGHT );
    
    // drop tweeter
    // mPipeline->setAttenuation( Pipeline::CHANNEL_LEFT, 3 );
    // mPipeline->setAttenuation( Pipeline::CHANNEL_RIGHT, 3 );
   // mPipeline->setAttenuationRight( 3 );

    mPipeline->generate( mSamplingRate );

    AMP_DEBUG_I( "Setting up I2S" );
    mI2S = new I2S( mAudioQueue );
  //  mI2S->start( mSamplingRate, mBitDepth, mSlotDepth );

    AMP_DEBUG_I( "Waiting for messages on Audio Thread" );
    uint32_t count = 0;

    while ( true ) {
        while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
            switch( msg.mMessageType ) {
                case Message::MSG_I2S_RECV:
                    { 
                        //AMP_DEBUG_I( "I2S message received %lu", msg.mParam2 );
                        int32_t *outBuf = 0;
                        int32_t *inBuf = (int32_t *)msg.mParam;
                        int32_t samples = msg.mParam2/(2*sizeof( int32_t ) );

                        outBuf = mPipeline->process( inBuf, samples );
                        size_t outSize = 0;
                        mI2S->writeData( (uint8_t *)outBuf, msg.mParam2, outSize );
                        count++;

                        
                        if ( count % 400 == 0 ) 
                        {
                        
                            AMP_DEBUG_I( "Received %lu bytes", msg.mParam2 );
                            /*
                            
                            std::stringstream s;

                             for ( int i = 0 ; i < samples*2; i++ ) {
                                s << ( inBuf[ i ] >> 16) << " ";
                            }

                            AMP_DEBUG_I( "Actual input data - %s\n\n", s.str().c_str() );

                             s.str( "" );
                            
                            for ( int i = 0 ; i < samples; i++ ) {
                                s << ( outBuf[ 2*i ] >> 16 ) << " ";
                            }

                            AMP_DEBUG_I( "Filtered woofer data - %s\n\n", s.str().c_str() );
                            
                            s.str( "" );
                            
                            for ( int i = 0 ; i < samples; i++ ) {
                                s << ( outBuf[ 2*i + 1] >> 16 ) << " ";
                            }

                            AMP_DEBUG_I( "Filtered tweeter data - %s\n\n", s.str().c_str() );
                            */
                        }
                    }
                   // processAudio();
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
                case Message::MSG_TIMER:   
                {
                    AMP_DEBUG_I( "Timer message received" );
                    mProfile->reset();
                

                    int32_t *outBuf = 0;
                    for ( int i = 0; i < packets; i++ ) {
                        mPipeline->resetDelayLines();
                        outBuf = mPipeline->process( buffer, packetSamples );
                    }

                    {
                        uint32_t totalCycles = mProfile->getTotalCyles();
                        mCpuUsage = round( 255*totalCycles/1000000.0f );
                        AMP_DEBUG_I( "Processed 1 second of audio in %lu microseconds", totalCycles );
                        AMP_DEBUG_I( "Processing %d biquads at %lu Hz, CPU usage roughly: %0.2f%%, %d", mPipeline->getBiquadCount(), mSamplingRate, totalCycles/10000.0f, mCpuUsage );
                        
                    }

                    {
                        std::stringstream s;
                        s.precision( 3 );
                        s.setf(std::ios::fixed);
                        
                        for ( int i = 0 ; i < packetSamples; i++ ) {
                            s << outBuf[ 2*i ] << ",";
                        }

                        AMP_DEBUG_I( "Filtered woofer data - %s\n\n", s.str().c_str() );
                        
                        s.str( "" );
                        
                        for ( int i = 0 ; i < packetSamples; i++ ) {
                            s << outBuf[ 2*i + 1] << ",";
                        }

                        AMP_DEBUG_I( "Filtered tweeter data - %s\n\n", s.str().c_str() );
                    }

                    mProfile->dump();
                    

                  //  mProfile->tick();
                }
                    break;
                default:
                    break;
            }
        }

        mTimer->processTick();
    }
}

void
DSP::handleGeneralThread() {
    AMP_DEBUG_I( "Starting general thread" );
    Message msg;

    AMP_DEBUG_I( "Starting up I2C" );
    mI2C = new I2CBUS( getI2CAddress(), mGeneralQueue );
    
    AMP_DEBUG_I( "Waiting for messages on General Thread" );
    while ( true ) {
        while ( mGeneralQueue.waitForMessage( msg, 50 ) ) {
            switch( msg.mMessageType ) {
                case Message::MSG_I2C:
                    mI2C->processData();
                    break;
                default:
                    break;
            }
        }
    }
}

void 
DSP::startAudio() {
    mSamplesPerPayload = ( mSamplingRate * DSP_I2S_PACKET_SIZE_MS ) / 1000;
    mBytesPerPayload = sizePerSample() * mSamplesPerPayload;

    // set the size of the pipeline, in samples but representing DSP_I2S_PACKET_SIZE_MS milliseconds
   // mPipeline->setSamples( mSamplesPerPayload );

    // generate filter coefficients, which change based on the sampling rate
    // will need to regenerate this when the sampling rate changes
    mPipeline->generate( mSamplingRate );
  //  mI2S->start( mSamplingRate, mBitDepth, mSlotDepth );

    mTimerID = mTimer->setTimer( 1000, mAudioQueue, true );
}

void 
DSP::stopAudio() {
    if ( mTimerID ) { 
        mTimer->cancelTimer( mTimerID );
        mTimerID = 0;
    }

  //  mI2S->stop();
    mPipeline->destroy();
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
                        mPipeline->setLeftSource( Pipeline::SOURCE_LEFT );
                        mPipeline->setLeftSource( Pipeline::SOURCE_RIGHT );
                        break;
                    case 1:
                        // both output channels have the left channel as source
                        mPipeline->setLeftSource( Pipeline::SOURCE_LEFT );
                        mPipeline->setLeftSource( Pipeline::SOURCE_LEFT );
                        break;
                    case 2:
                        // both output channels have the right channel as source
                        mPipeline->setLeftSource( Pipeline::SOURCE_RIGHT );
                        mPipeline->setLeftSource( Pipeline::SOURCE_RIGHT );
                        break;
                    case 8:
                        // left and right combined, in left channel, no attenuation
                        mPipeline->setLeftSource( Pipeline::SOURCE_COMBINED );
                        mPipeline->setLeftSource( Pipeline::SOURCE_COMBINED );
                        break;
                    case 9:
                        // left and right combined, in left channel, 6db decrease
                        mPipeline->setLeftSource( Pipeline::SOURCE_COMBINED_6DB );
                        mPipeline->setLeftSource( Pipeline::SOURCE_COMBINED_6DB );
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
        case REGISTER_CPU:
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