/*
 * I2C.h
 *
 *  Created on: Jan. 10, 2023
 *      Author: duane
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include "main.h"
#include <memory>

typedef uint8_t I2C_ADDR;

class I2C_Device;

class I2C {
protected:
	I2C_HandleTypeDef mI2C;
private:
	uint8_t mBuffer[16] = {0};

public:
	I2C() {}
	I2C( I2C_HandleTypeDef bus );
	virtual ~I2C();

	void setBusData( I2C_HandleTypeDef bus ) { mI2C = bus; }
	uint8_t readByte( I2C_ADDR addr );
	bool writeByte( I2C_ADDR addr, uint8_t );

	bool writeData( I2C_ADDR addr, uint8_t *data, uint8_t size );

	I2C_Device * makeDevice( I2C_ADDR addr );
};

#endif /* SRC_I2C_H_ */
