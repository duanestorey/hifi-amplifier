#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "driver/gpio.h"

#define DSP_VERSION_MAJOR		1
#define DSP_VERSION_MINOR		0		

#define DSP_I2C_BUFFER_SIZE		128
#define DSP_I2C_BASE_ADDR		0b1010000

#define DSP_I2S_PACKET_SIZE_MS	10
#define DSP_I2S_MAX_TIMEOUT		50

#define DSP_PIN_SDA				(gpio_num_t)5
#define DSP_PIN_SCL				(gpio_num_t)6
#define DSP_PIN_ADDR_0			(gpio_num_t)45
#define DSP_PIN_ADDR_1			(gpio_num_t)46
#define DSP_PIN_ADDR_2			(gpio_num_t)9
#define DSP_PIN_ACTIVE			(gpio_num_t)7

#define DSP_PIN_DIN				(gpio_num_t)10
#define DSP_PIN_DOUT			(gpio_num_t)4
#define DSP_PIN_MCLK			(gpio_num_t)11
#define DSP_PIN_BCLK			(gpio_num_t)20
#define DSP_PIN_LRCLK			(gpio_num_t)13


#endif