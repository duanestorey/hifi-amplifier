#ifndef __TMP100_H__
#define __TMP100_H__

#include "i2c-bus.h"

class TMP100 {
public:
    TMP100( uint8_t address, I2CBUS *bus );
    virtual float readTemperature();
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;
};

#endif