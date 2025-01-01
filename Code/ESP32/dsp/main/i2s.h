#ifndef __I2S_H__
#define __I2S_H__

#include "queue.h"
#include "driver/i2s_std.h"
#include "circ-buffer.h"

class I2S {
    public:
        I2S( Queue queue );
        void start( uint32_t samplingRate, uint8_t bitDepth, uint8_t slotDepth );
        void stop();

        void handleReceiveCallback( i2s_event_data_t *event );
        void writeData( uint8_t *data, size_t maxSize, size_t &dataSize );
        void readData( uint8_t *data, size_t maxSize, size_t &dataSize );

        uint32_t bytesWaiting() { return mBytesWaiting; }
    protected:
        Queue mQueue;
        uint32_t mSamplingRate;
        uint8_t mBitDepth;
        uint8_t mSlotDepth;

        uint32_t mBytesWaiting;

        i2s_chan_handle_t mTxHandle;
        i2s_chan_handle_t mRxHandle;

        bool mTransmitStarted;
        i2s_std_config_t mConfig;

        Circ_Buffer *mBuffer;
};

#endif