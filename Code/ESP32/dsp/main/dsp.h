#ifndef __DSP_H__
#define __DSP_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "queue.h"

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
};

#endif