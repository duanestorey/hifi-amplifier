#include "dsp.h"
#include "debug.h"

DSP::DSP() : mSamplingRate( 48000 ), mBitDepth( 16 ), mMode( MODE_BYPASS ), mI2C( 0 ), mI2S( 0 ) {

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
DSP::handleAudioThread() {
    AMP_DEBUG_I( "Starting audio thread" );
    Message msg;

    mI2S = new I2S( mAudioQueue );
    mI2S->start( mSamplingRate );
  

    uint32_t samplesPerPayload = ( mSamplingRate * DSP_I2S_PACKET_SIZE_MS ) / 1000;

    size_t bytesPerPayload = 2 * sizePerSample() * samplesPerPayload;
    uint8_t *buffer = (uint8_t *)aligned_alloc( sizePerSample(), sizePerSample() * 2 * samplesPerPayload );
    size_t receivedSize = 0;
    size_t writtenSize = 0;
    uint8_t *processed = 0;

    mPipeline = new Pipeline( samplesPerPayload );
    mPipeline->addBiquadLeft( new Biquad( Biquad::LOWPASS, 2000, 0.707 ) );
    mPipeline->addBiquadLeft( new Biquad( Biquad::LOWPASS, 2000, 0.707 ) );
    mPipeline->addBiquadRight( new Biquad( Biquad::HIGHPASS, 2000, 0.5 ) );

    while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
        switch( msg.mMessageType ) {
            case Message::MSG_I2S_RECV:
                while ( mI2S->bytesWaiting() >= bytesPerPayload ) {
                    mI2S->readData( buffer, bytesPerPayload, receivedSize );
                    if ( mMode == MODE_DSP ) {
                        if ( bytesPerPayload == receivedSize ) {
                            switch( mBitDepth ) {
                                case 16:    
                                    processed = (uint8_t *)mPipeline->process( (int16_t*)buffer );
                                    mI2S->writeData( processed, bytesPerPayload, writtenSize );
                                    break;
                                case 24:
                                case 32:
                                    processed = (uint8_t *)mPipeline->process( (int32_t*)buffer );
                                    mI2S->writeData( processed, bytesPerPayload, writtenSize );
                                    break;
                            }
                        } else {
                            // ERROR
                            AMP_DEBUG_E( "Unexpected read size %d", receivedSize );
                        }                  
                    } else if ( mMode == MODE_BYPASS ) {
                        mI2S->writeData( buffer, bytesPerPayload, writtenSize );
                    }
                }
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
DSP::onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize ) {
    // this will be on the general thread
    switch( reg ) {
        case REGISTER_RESET:
            break;
        case REGISTER_SET_MODE:
            break;
        case REGISTER_SET_FREQ:
            break;
        case REGISTER_ADD_BIQUAD:
            break;
        case REGISTER_START:
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