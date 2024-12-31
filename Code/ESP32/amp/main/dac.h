#ifndef __DECODER_H__
#define __DECODER_H__

#include <string>

class DAC {
public:
    enum {
		FRONT_LEFT = 0,
		FRONT_RIGHT = 1,
		REAR_LEFT = 2,
		REAR_RIGHT = 3,
		CENTER = 4,
		SUBWOOFER = 5
	} CHANNEL;

	enum {
		FORMAT_SONY = 0,
		FORMAT_I2S = 1,
		FORMAT_LEFT_JUSTIFIED = 2
	} FORMAT;

	enum {
		PRECISION_16_BIT = 0,
		PRECISION_24_BIT = 1,
		PRECISION_32_BIT = 2
	} PRECISION;

    virtual ~DAC() {}

	// the name for this DAC
	virtual std::string name() = 0;
	virtual void init() = 0;
    virtual void setFormat( uint8_t format ) = 0;
    virtual void enable( bool state ) = 0;
	virtual void setPrecision( uint8_t precision ) = 0;

	virtual void setChannelAttenuation( int channel, int att ) = 0;
	virtual void setAttenuation( int att ) = 0;
};

#endif