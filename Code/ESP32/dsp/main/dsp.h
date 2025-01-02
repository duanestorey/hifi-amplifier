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
#include "timer.h"
#include "profile.h"

class DSP : public I2C_Listener {
    public:
        enum {
            REGISTER_RESET = 1,
            REGISTER_SET_MODE = 2,
            REGISTER_SET_FREQ_DEPTH = 3,
            REGISTER_ADD_FILTER = 4,
            REGISTER_START = 10,
            REGISTER_VERSION = 100,
            REGISTER_CPU = 101
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
        void handleTimerThread();
        void fullReset();
    protected:
        virtual void onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize );

        uint32_t mSamplingRate;
        uint32_t mSamplesPerPayload;
        uint32_t mBytesPerPayload;

        uint8_t mBitDepth;
        uint8_t mSlotDepth;
        uint8_t mMode;

        Queue mGeneralQueue;
        Queue mAudioQueue;
        I2CBUS *mI2C;
        I2S *mI2S;
        Timer *mTimer;
        Profile *mProfile;
        Pipeline *mPipeline;

        volatile bool mAudioStarted;
        uint32_t mTimerID;
        uint8_t mCpuUsage;
    private:
        uint8_t getI2CAddress();
        uint8_t sizePerSample();

        void startAudio();
        void stopAudio();
        void processAudio();
        void writeSplitData( uint8_t *bufferPtr1, uint32_t writeSize1, uint8_t *bufferPtr2, uint32_t writeSize2, uint8_t bytesPerSample );
};

#endif