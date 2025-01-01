#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "driver/gpio.h"

#define DSP_I2C_BUFFER_SIZE		128
#define DSP_I2C_BASE_ADDR		0b1010000

#define DSP_PIN_SDA				(gpio_num_t)5
#define DSP_PIN_SCL				(gpio_num_t)6
#define DSP_PIN_ADDR_0			(gpio_num_t)45
#define DSP_PIN_ADDR_1			(gpio_num_t)46
#define DSP_PIN_ADDR_2			(gpio_num_t)9
#define DSP_PIN_ACTIVE			(gpio_num_t)7



#endif