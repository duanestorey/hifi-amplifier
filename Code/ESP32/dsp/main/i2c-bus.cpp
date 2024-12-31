#include <stdio.h>
#include "i2c-bus.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "debug.h"

#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0  
#define I2C_MASTER_NUM              0    
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_FREQ_HZ          100000

#define I2C_WRITE_REQ               0
#define I2C_READ_REQ                0

#define ACK_VAL                     0x0         
#define NACK_VAL                    0x1             

I2CBUS::I2CBUS() {  
    AMP_DEBUG_I( "Stargting I2C bus" );
    i2c_config_t conf = {};

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    esp_err_t err = i2c_param_config( I2C_NUM_0, &conf );
    ESP_ERROR_CHECK( i2c_driver_install( I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) );
}

void 
I2CBUS::scanBus() {
    int devices_found = 0;
    for(int address = 1; address < 127; address++) {
        // create and execute the command link
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        if(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS) == ESP_OK) {
            printf("-> found device with address 0x%02x\r\n", address);
            devices_found++;
        }
        i2c_cmd_link_delete(cmd);
    }
    if(devices_found == 0) printf("\r\n-> no devices found\r\n");
    printf("\r\n...scan completed!\r\n");
}

bool
I2CBUS::writeBytes( uint8_t address, uint8_t *data, uint8_t size ) {
   esp_err_t err = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start( cmd );
    i2c_master_write_byte( cmd, (address << 1) | I2C_MASTER_WRITE, true );
    i2c_master_write( cmd, data, size, true );
    i2c_master_stop( cmd );
    if ( ( err = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MS_TO_WAIT / portTICK_PERIOD_MS ) ) != ESP_OK ) {
        AMP_DEBUG_W( "Issue while sending I2C data to address %d", (int)address );
    }
    
    i2c_cmd_link_delete(cmd);

    return ( err == ESP_OK );
}

bool 
I2CBUS::writeRegisterByte( uint8_t address, uint8_t reg, uint8_t data ) {
    esp_err_t err = ESP_OK;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start( cmd );
    i2c_master_write_byte( cmd, (address << 1) | I2C_MASTER_WRITE, true );
    i2c_master_write_byte( cmd, reg, true );
    i2c_master_write_byte( cmd, data, true );
    i2c_master_stop( cmd );
    if ( ( err = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MS_TO_WAIT / portTICK_PERIOD_MS ) ) != ESP_OK ) {
        AMP_DEBUG_W( "Issue while sending I2C data to address %d",(int)address );
    }
    
    i2c_cmd_link_delete(cmd);

    return ( err == ESP_OK );
}

bool 
I2CBUS::readRegisterByte( uint8_t address, uint8_t reg, uint8_t &data  ) {
    esp_err_t err = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start( cmd );
    i2c_master_write_byte( cmd, (address << 1), true );
    i2c_master_write_byte( cmd, reg, true );
    i2c_master_start( cmd );
    i2c_master_write_byte( cmd, (address << 1) | I2C_MASTER_READ, true );
    i2c_master_read_byte( cmd, &data, I2C_MASTER_LAST_NACK );
    i2c_master_stop( cmd );

    if ( ( err = i2c_master_cmd_begin( I2C_NUM_0, cmd, I2C_MS_TO_WAIT / portTICK_PERIOD_MS ) ) != ESP_OK ) {
        AMP_DEBUG_W( "Issue while readig I2C data at address %d",(int)address );
    }

    i2c_cmd_link_delete(cmd);

    return ( err == ESP_OK );
}

bool 
I2CBUS::readRegisterBytes( uint8_t address, uint8_t reg, uint8_t dataSize, uint8_t *data  ) {
    esp_err_t err = i2c_master_write_read_device( I2C_NUM_0, address, &reg, 1, data, dataSize, I2C_MS_TO_WAIT / portTICK_PERIOD_MS );
    if ( err != ESP_OK ) {
        AMP_DEBUG_W( "Issue while reading I2C data to address %d", (int)address );
    }

    return ( err == ESP_OK );
}