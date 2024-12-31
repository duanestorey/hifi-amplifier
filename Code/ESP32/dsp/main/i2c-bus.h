#ifndef __I2CBUS_H__
#define __I2CBUS_H__

#define I2C_MS_TO_WAIT      50

#include "mutex.h"

class I2CBUS {
public:
    I2CBUS();

    bool writeRegisterByte( uint8_t address, uint8_t reg, uint8_t data );
    bool writeBytes( uint8_t address, uint8_t *data, uint8_t size );
    bool readRegisterByte( uint8_t address, uint8_t reg, uint8_t &data  );
    bool readRegisterBytes( uint8_t address, uint8_t reg, uint8_t dataSize, uint8_t *data  );
    void scanBus();
private:
    Mutex mMutex;
};

#endif