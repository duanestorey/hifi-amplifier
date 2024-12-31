#include "tmp100.h"
#include "debug.h"

TMP100::TMP100( uint8_t address, I2CBUS *bus ) : mAddress( address ), mI2C( bus ) {
    mI2C->writeRegisterByte( mAddress, 1, 0x60 );
}

float 
TMP100::readTemperature() {
	uint8_t data[2];

	mI2C->readRegisterBytes( mAddress, 0, 2, data );
	uint16_t ss = ((((uint16_t)data[0]) << 8) + data[1])>>4;
	float f = ss / 16.0;

	AMP_DEBUG_I( "Temperature is %0.2f", f );

    return f;
}