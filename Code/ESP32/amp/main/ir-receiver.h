#ifndef __IR_RECEIVER_H__
#define __IR_RECEIVER_H__

#include "queue.h"
#include "pins.h"

class IR_Receiver {
public:
    enum {
        STATE_IDLE,
        STATE_LOOKING_FOR_PULSE,
        STATE_STARTED
    };

    IR_Receiver( PIN gpio, Queue *queue ) : mGPIO( gpio ), mQueue( queue ), mDecodeStarted( false ), mWentLowTime( 0 ), mWentHighTime( 0 ), 
        mState( STATE_IDLE ), mBitsReceived( 0 ), mBits( 0 ), mLastDecodedBits( 0 ) {}

    virtual void begin();
    virtual void handleInterrupt();
protected:
    void setupPin();

    PIN mGPIO;
    Queue *mQueue;

    bool mDecodeStarted;
    int64_t mWentLowTime;
    int64_t mWentHighTime;
    uint8_t mState;
    uint8_t mBitsReceived;
    uint32_t mBits;
    uint32_t mLastDecodedBits;
};

void IR_Receiver_Interrupt( void *arg  );

#endif

