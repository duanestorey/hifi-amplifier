#include "dac-pcm1681.h"
#include "debug.h"

#define PCM1681_REG_MUTE	7
#define PCM1681_REG_ENABLE	8
#define PCM1681_REG_FORMAT	9
#define PCM1681_REG_RESET 	10
#define PCM1681_REG_OVER	12
#define PCM1681_REG_DAMS	13

#include "tmp100.h"

DAC_PCM1681::DAC_PCM1681( uint8_t address, I2CBUS *bus ) : mAddress( address ), mI2C( bus ), mEnabled( false ) {
}

DAC_PCM1681::~DAC_PCM1681() {
}

void 
DAC_PCM1681::init() {
    AMP_DEBUG_I( "Starting PCM1681 DAC" );   
	mI2C->writeRegisterByte( mAddress, PCM1681_REG_RESET, 128 );

	vTaskDelay( 100 / portTICK_PERIOD_MS );

    // Set wide over-sampling bit and sharp roll-off filters
    mI2C->writeRegisterByte( mAddress, PCM1681_REG_OVER, 128 );

    // Set wide volume range, 0-63, 0.5db
    mI2C->writeRegisterByte( mAddress, PCM1681_REG_DAMS, 128 );
}

void 
DAC_PCM1681::setFormat( uint8_t format ) {
	uint8_t value = 0;
	if ( format == FORMAT_SONY ) {
		value = 5;
	} else if ( format == FORMAT_I2S ) {
		value = 4;
	} else {
		value = format;
	}

	mI2C->writeRegisterByte( mAddress, PCM1681_REG_FORMAT, value );  
}

void 
DAC_PCM1681::enable( bool state ) {
	if ( state ) {
		mI2C->writeRegisterByte( mAddress, PCM1681_REG_ENABLE, 0x00 );
	} else {
		mI2C->writeRegisterByte( mAddress, PCM1681_REG_ENABLE, 0xff );
	}

	mEnabled = state;
}

void 
DAC_PCM1681::setChannelAttenuation( int channel, int att ) {
	uint8_t reg = 0;
	switch( channel ) {
		case FRONT_LEFT:
			reg = 1;
			break;
		case FRONT_RIGHT:
			reg = 2;
			break;
		case REAR_LEFT:
			reg = 3;
			break;
		case REAR_RIGHT:
			reg = 4;
			break;
		case CENTER:
			reg = 5;
			break;
		case SUBWOOFER:
			reg = 6;
			break;
	}

	uint8_t level = 0xff;

	// Clamp the volume to the upper limit
	if ( att > 127 ) {
		att = 127;
	}

	level = level - att;
	mI2C->writeRegisterByte( mAddress, reg, level );
}

void 
DAC_PCM1681::setAttenuation( int att ) {
	setChannelAttenuation( FRONT_LEFT, att );
	setChannelAttenuation( FRONT_RIGHT, att );
	setChannelAttenuation( REAR_LEFT, att );
	setChannelAttenuation( REAR_RIGHT, att );
	setChannelAttenuation( CENTER, att );
	setChannelAttenuation( SUBWOOFER, att );
}

void 
DAC_PCM1681::setPrecision( uint8_t precision ) {

}

