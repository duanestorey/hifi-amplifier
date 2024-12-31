#include "lcd.h"
#include "rom/ets_sys.h"
#include "debug.h"
#include <string.h>

LCD::LCD( uint8_t addr, I2CBUS *bus ) : mAddr( addr ), mI2C( bus ), mBacklight( BACKLIGHT_ON ) {
	mRowOffsets[0] = 0;
	mRowOffsets[1] = 0x40;
	mRowOffsets[2] = 20;
	mRowOffsets[3] = 0x40 + 20;
}

void 
LCD::sendCommand( uint8_t command ) {
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (command&0xF0);
	data_l = ((command<<4)&0xF0);
	data_t[0] = data_u|0x0C;	// en=1, rs=0
	data_t[1] = data_u|0x08;	// en=0, rs=0
	data_t[2] = data_l|0x0C;	// en=1, rs=0
	data_t[3] = data_l|0x08;	// en=0, rs=0

	mI2C->writeBytes( mAddr, data_t, 4 );
}

void 
LCD::enableBacklight( bool enable ) {
	uint8_t value = 0;
	if ( enable ) {
		mBacklight = BACKLIGHT_ON;
		command( LCD_COMMAND_DISPLAYCONTROL | 0x04 );
		
	} else {
		mBacklight = false;
		command( LCD_COMMAND_DISPLAYCONTROL );
	}

	vTaskDelay( 10 / portTICK_PERIOD_MS );
}

void 
LCD::clearDisplay() {
	command( LCD_COMMAND_CLEAR );
	vTaskDelay( 10 / portTICK_PERIOD_MS );
}

void 
LCD::setCursor( uint8_t x, uint8_t y ) {
	command( LCD_COMMAND_SETDDRAMADDR | ( x + mRowOffsets[ y ] ) );
}

void 
LCD::writeString( std::string s ) {
	if ( s.length() == 0 ) return;

	char data[ 32 ];
	strcpy( data, s.c_str() );

	uint8_t *buffer = (uint8_t *)data;

	while ( *buffer ) {
		sendData( *buffer, LCD_RS_BIT );
		buffer++;
	}
}

void 
LCD::reset() {
}

void 
LCD::home() {
	command( LCD_COMMAND_HOME );
	vTaskDelay( 10 / portTICK_PERIOD_MS );
}

void 
LCD::update() {
}

void 
LCD::begin() {
    AMP_DEBUG_I( "Beginning LCD Setup" );
  //  vTaskDelay( 50 / portTICK_PERIOD_MS );
	write8Bits( mBacklight );
    vTaskDelay( 500 / portTICK_PERIOD_MS );

    AMP_DEBUG_I( "LCD Backlight Setup" );

    // Elevate the priority here temporarily to try and prevent a context switch
   // UBaseType_t currentPriority = uxTaskPriorityGet( xTaskGetCurrentTaskHandle() );
    //vTaskPrioritySet( xTaskGetCurrentTaskHandle(), 3 );
	{
        write4Bits( 0x30 );
        ets_delay_us( 4500 );
        write4Bits( 0x30 );
        ets_delay_us( 4500 );
        write4Bits( 0x30 );
        ets_delay_us( 150 );
        write4Bits( 0x20 );
    }
   // vTaskPrioritySet( xTaskGetCurrentTaskHandle(), currentPriority );

	// lcd 2 line
	command( LCD_COMMAND_FUNCTIONSET |  0x08 );

	// turn display on
	command( LCD_COMMAND_DISPLAYCONTROL | 0x04 );

	clearDisplay();

	home();

    AMP_DEBUG_I( "LCD Setup Done" );
}

// Private
void 
LCD::write4Bits( uint8_t data ) {
	write8Bits( data );
	pulseEnable( data );
}

void 
LCD::write8Bits( uint8_t data ) {
	data = data | mBacklight;
	mI2C->writeBytes( mAddr, &data, 1 );
}

void 
LCD::sendData( uint8_t data, uint8_t mode ) {
	uint8_t highNibble = data & 0xf0;
	uint8_t lowNibble = ( data << 4 ) & 0xf0;

	write4Bits( (highNibble ) | mode );
	write4Bits( (lowNibble) | mode );
}

void 
LCD::command( uint8_t data ) {
	sendData( data, 0 );
}

void 
LCD::writeLine( uint8_t line, std::string s ) {
	char data[ 30 ] = {0};

	setCursor( 0, line );
	sprintf( data, "%-20s", s.c_str() );

	writeString( data );
}

void 
LCD::pulseEnable( uint8_t data ) {
    // Elevate the priority here temporarily to try and prevent a context switch
  //  UBaseType_t currentPriority = uxTaskPriorityGet( xTaskGetCurrentTaskHandle() );
  //  vTaskPrioritySet( xTaskGetCurrentTaskHandle(), 3 );
	{
		write8Bits( data | LCD_EN_BIT );
		ets_delay_us(1);
		write8Bits( data & ~LCD_EN_BIT );
		ets_delay_us(50);
	}
  //  vTaskPrioritySet( xTaskGetCurrentTaskHandle(), currentPriority );
}