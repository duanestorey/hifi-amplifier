#include "button.h"
#include "pins.h"
#include "debug.h"

Button::Button( uint8_t gpio, Queue *queue, uint16_t debounceMs ) : mGpio( gpio ), mDebounceTime( debounceMs ), mQueue( queue ), mThinking ( false ), mThinkingStartTime( 0 ), mPressed( false ) {

}

void
Button::startThinking() {
    mThinking = true;
    mThinkingStartTime = xTaskGetTickCount();
}

void 
Button::stopThinking() {
    mThinking = false;
    mThinkingStartTime = 0;
}


void
Button::tick() {
    if ( mThinking ) {
        uint8_t level = gpio_get_level( (gpio_num_t) mGpio );
        uint32_t totalMs = ( xTaskGetTickCount() - mThinkingStartTime ) * portTICK_PERIOD_MS;
        if ( level ) {
            // active high, means it's not pressed
            if ( mPressed ) {
                // Looks like a press, so let's wait
                if ( totalMs >= mDebounceTime ) {
                    unpressed();
                } else {
                    // Keep waiting
                        AMP_DEBUG_I( "Waiting..%lu", totalMs );
                }
            } else {
                // failed debounce
                stopThinking();
            }
        } else {
            // active high, means it's pressed
            if ( !mPressed ) {
                if ( totalMs >= mDebounceTime ) {
                    pressed();
                } else {
                    // Keep waiting
                    AMP_DEBUG_I( "Waiting..%lu", totalMs );
                }  
            } else {
                stopThinking();
            }
        }
    }
}


void 
Button::pressed() {
    stopThinking();

    mPressed = true;
    mQueue->add( Message::MSG_BUTTON_PRESSED, mGpio );
}

void 
Button::unpressed() {
    stopThinking();

    mPressed = false;
    mQueue->add( Message::MSG_BUTTON_RELEASED, mGpio );
}

void
Button::handleInterrupt() {
    uint8_t level = gpio_get_level( (gpio_num_t) mGpio );
    if ( level ) {
        // active high, means it's not pressed
        if ( mPressed ) {
            startThinking();
        } else {
            // it's not pressed
        }
    } else {
        // active high, means it's pressed
        if ( mPressed ) {
            // it's pressed, so we can do nothing
        } else {
            // start debounce
            startThinking();
        }
    }
}