#ifndef __DSP_H__
#define __DSP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "queue.h"
#include "i2c-bus.h"
#include "i2c-listener.h"

class DSP : public I2C_Listener {
    public:
        enum {
            REGISTER_RESET = 1,
            REGISTER_SET_FREQ = 2,
            REGISTER_ADD_BIQUAD = 3,
            REGISTER_START = 10,
            REGISTER_VERSION = 255
        };

        DSP();
        ~DSP();

        void start();

        void handleAudioThread();
        void handleGeneralThread();
    protected:
        virtual void onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize );

        Queue mGeneralQueue;
        Queue mAudioQueue;
        I2CBUS *mI2C;
    private:
        uint8_t getI2CAddress();
};

#endif