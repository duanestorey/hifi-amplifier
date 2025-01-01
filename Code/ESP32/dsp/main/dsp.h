#ifndef __DSP_H__
#define __DSP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "queue.h"
#include "i2c-bus.h"
#include "i2c-listener.h"
#include "i2s.h"
#include "pipeline.h"

class DSP : public I2C_Listener {
    public:
        enum {
            REGISTER_RESET = 1,
            REGISTER_SET_MODE = 2,
            REGISTER_SET_FREQ = 3,
            REGISTER_ADD_BIQUAD = 4,
            REGISTER_START = 10,
            REGISTER_VERSION = 255
        };

        enum {
            MODE_BYPASS = 1,
            MODE_DSP = 2
        };

        DSP();
        ~DSP();

        void start();

        void handleAudioThread();
        void handleGeneralThread();
    protected:
        virtual void onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize );

        uint32_t mSamplingRate;
        uint8_t mBitDepth;
        uint8_t mMode;

        Queue mGeneralQueue;
        Queue mAudioQueue;
        I2CBUS *mI2C;
        I2S *mI2S;
        Pipeline *mPipeline;
    private:
        uint8_t getI2CAddress();
        uint8_t sizePerSample();
};

#endif