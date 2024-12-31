#include "ir-receiver.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "debug.h"

void
IR_Receiver::setupPin() {
    uint_fast32_t mask = ( ((uint_fast32_t)1) << ( uint_fast32_t ) mGPIO );
    gpio_config_t io_conf;

	io_conf.intr_type = GPIO_INTR_ANYEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = mask;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

	gpio_config( &io_conf );   
}

void 
IR_Receiver::begin() {
    gpio_isr_handler_add( mGPIO, IR_Receiver_Interrupt, this );
}

void 
IR_Receiver::handleInterrupt() {
    uint8_t level = gpio_get_level( mGPIO );
    switch( mState ) {
        case STATE_IDLE:
            if ( level ) {
                // we might be starting
                mWentHighTime = esp_timer_get_time();
                mBitsReceived = 0;
                mBits = 0;
            } else {
                mWentLowTime = esp_timer_get_time();

                uint16_t timeDiff = mWentHighTime - mWentLowTime;

                if ( timeDiff > 9000 ) {
                     mState = STATE_LOOKING_FOR_PULSE;
                }
            }
            break;
        case STATE_LOOKING_FOR_PULSE:
            if ( level ) {
                mWentHighTime = esp_timer_get_time();

                uint16_t timeDiff = mWentHighTime - mWentLowTime;

                 if ( timeDiff > 4000 ) {
                    mState = STATE_STARTED;
                } else {
                    mState = STATE_IDLE;
                }
            } else {
                // bail out
                mState = STATE_STARTED;
            }
            break;
        case STATE_STARTED:
            if ( level ) {
                mWentHighTime = esp_timer_get_time();

                uint16_t pulseWidth = mWentHighTime - mWentLowTime;

                if ( mBitsReceived < 32 ) {
                    if ( pulseWidth > 200 ) {
                        // 1
                        mBits = ( mBits >> 1 ) | 0x8000;
                    } else {
                        // 0
                        mBits = ( mBits >> 1 );
                    }

                    mBitsReceived++;
                }
            } else {
                mWentLowTime = esp_timer_get_time();

                if ( mBitsReceived == 32 ) {
                    // done
                    mState = STATE_IDLE;

                    AMP_DEBUG_INT_I( "Command received %08X", mBitsReceived );
                }
            }
            break;
    }
}

void 
IR_Receiver_Interrupt( void *arg ) {
    (( IR_Receiver * )arg)->handleInterrupt();
}
