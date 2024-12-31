#ifndef __LCD_H__
#define __LCD_H__

#define LCD_BACKLIGHT       0x08
#define LCD_NOBACKLIGHT     0x00
#define LCD_I2C_ADDR		(0x27 << 1 )

#include <string>

#include "i2c-bus.h"

class LCD {
public:
	enum {
		BACKLIGHT_OFF = 0,
		BACKLIGHT_ON = 0x08
	};

    enum {
		LCD_RS_BIT = 1,
		LCD_RW_BIT = 2,
		LCD_EN_BIT = 4
	};
	
	enum {
		LCD_COMMAND_CLEAR = 1,
		LCD_COMMAND_HOME = 2,
		LCD_COMMAND_ENTRYMODESET = 4,
		LCD_COMMAND_DISPLAYCONTROL = 8,
		LCD_COMMAND_FUNCTIONSET = 0x20,
		LCD_COMMAND_SETCGRAMADDR = 0x40,
		LCD_COMMAND_SETDDRAMADDR = 0x80
	};

    LCD( uint8_t addr, I2CBUS *bus );

    void sendCommand( uint8_t command );
	void enableBacklight( bool enable = true );
	void clearDisplay();
	void setCursor( uint8_t x, uint8_t y );
	void writeString( std::string s );
	void reset();
	void home();
	void update();
	void begin();

	void writeLine( uint8_t line, std::string s );
protected:
    uint8_t mAddr;
    I2CBUS *mI2C;
	uint8_t mBacklight;
	uint8_t mRowOffsets[4];

private:
	void write4Bits( uint8_t data );
	void write8Bits( uint8_t data );
	void sendData( uint8_t data, uint8_t mode );
	void command( uint8_t data );
	void pulseEnable( uint8_t data );
};

#endif