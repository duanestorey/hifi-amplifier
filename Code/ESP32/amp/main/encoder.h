#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "esp_system.h"
#include "sdkconfig.h"
#include "pins.h"
#include "esp_log.h"
#include "debug.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "sdkconfig.h"

typedef enum {
    ENCODER_NONE,
    ENCODER_FORWARD,
    ENCODER_BACKWARD
} ENCODER_DIR;

typedef enum {
    ENCODER_WAITING,
    ENCODER_CCW,
    ENCODER_CW
} ENCODER_INT_DIR;

class Encoder {
public:
    Encoder( uint8_t pin1, uint8_t pin2, bool reverse );

    ENCODER_DIR process();
    uint8_t getState(); 

    uint32_t level() { return mLevel; }
protected:
    uint8_t mPin1;
    uint8_t mPin2;
    bool mReverse;
    uint8_t mLastState;
    uint8_t mClicks;
    ENCODER_INT_DIR mInternalDir;

    uint32_t mLevel;
    uint32_t mLastEmitLevel;
public:
     volatile uint8_t mISRstate;
};

#endif