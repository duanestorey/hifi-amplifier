#include "encoder.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

Encoder::Encoder( uint8_t pin1, uint8_t pin2, bool reverse ) : mPin1( pin1 ), mPin2( pin2 ), mReverse( reverse ), mISRstate( -1 ) {
    mLastState = getState();
    mInternalDir = ENCODER_WAITING;
    mClicks = 0;

    mLevel = 0xf00000;
    mLastEmitLevel = mLevel;
}

uint8_t 
Encoder::getState() {
    uint8_t level1 = gpio_get_level( (gpio_num_t) mPin1 );
    uint8_t level2 = gpio_get_level( (gpio_num_t) mPin2 );

    return ( level1 << 1 ) | level2;
}

ENCODER_DIR 
Encoder::process() {
    ENCODER_DIR returnValue = ENCODER_NONE;
    uint8_t currentState = getState();

    if ( currentState == mLastState ) {
        return returnValue;
    }

    // forward
    // right -> 00 -> 01 -> 11 -> 10
    // left ->  00 -> 10 -> 11 -> 01
    switch( mLastState ) {
        case 0:
            // can switch to 01 or 10 
            if ( currentState == 1 ) {
                mLevel++;
            } else if ( currentState == 2 ) {        
                mLevel--;
            }
            break;
        case 1:
            // 01 can go to 11 or 00
            if ( currentState == 0 ) {
                mLevel--;
            } else if ( currentState == 3 ) {
                mLevel++;
            }
            break;
        case 2:
            // 10 can go to 11 or 00
            if ( currentState == 3 ) {
                mLevel--;
            } else if ( currentState == 0 ) {
                mLevel++;
            }        
            break;
        case 3:
             // 11 can go to 10 or 01
            if ( currentState == 2 ) {          
                mLevel++;
            } else if ( currentState == 1 ) {
                mLevel--;
            }           
            break;
    }

    mLastState = currentState;

    if ( mLevel % 4 == 0 ) {
        if ( mLevel != mLastEmitLevel ) {
            if ( mLevel > mLastEmitLevel ) {
                if ( !mReverse ) returnValue = ENCODER_FORWARD; else returnValue = ENCODER_BACKWARD;
            } else if ( mLevel < mLastEmitLevel ) {
                if ( !mReverse ) returnValue = ENCODER_BACKWARD; else returnValue = ENCODER_FORWARD;
            }
            mLastEmitLevel = mLevel;
        }
    }

    return returnValue;
}