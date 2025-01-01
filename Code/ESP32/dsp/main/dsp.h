#ifndef __DSP_H__
#define __DSP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "queue.h"
#include "i2c-bus.h"

class DSP {
    public:
        DSP();
        ~DSP();

        void start();

        void handleAudioThread();
        void handleGeneralThread();
    protected:
        Queue mGeneralQueue;
        Queue mAudioQueue;
        I2CBUS *mI2C;
    private:
        uint8_t getI2CAddress();
};

#endif