#include "dsp.h"
#include "debug.h"

DSP::DSP() : mI2C( 0 ) {

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

void 
DSP::handleAudioThread() {
    AMP_DEBUG_I( "Starting audio thread" );
    Message msg;
    while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
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