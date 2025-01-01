#ifndef __I2C_LISTENER_H__
#define __I2C_LISTENER_H__

#include "config.h"

class I2C_Listener {
public:
    virtual void onNewI2CData( uint8_t reg, uint8_t *buffer, uint8_t dataSize ) = 0;
};

#endif