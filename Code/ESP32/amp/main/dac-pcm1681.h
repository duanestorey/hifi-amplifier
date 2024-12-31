#ifndef __DAC_PCM1681_H__
#define __DAC_PCM1681_H__

#include "dac.h"
#include "i2c-bus.h"

class DAC_PCM1681 : public DAC {
public:
    DAC_PCM1681( uint8_t address, I2CBUS *bus );
    virtual ~DAC_PCM1681();

	// the name for this DAC
	virtual std::string name() { return "PCM1681"; };
	virtual void init();
    virtual void setFormat( uint8_t format );
    virtual void enable( bool state );

	virtual void setChannelAttenuation( int channel, int att );
	virtual void setAttenuation( int att );
    virtual void setPrecision( uint8_t precision );
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;
    bool mEnabled;
};

#endif