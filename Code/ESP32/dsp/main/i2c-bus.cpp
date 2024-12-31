#include <stdio.h>
#include "i2c-bus.h"
#include "sdkconfig.h"
#include "config.h"
#include "debug.h"


static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *userData )
{
    ((I2CBUS *)userData)->handleReceiveData();
    return true;
}

I2CBUS::I2CBUS( uint8_t slaveAddr, Queue queue ) : mQueue( queue ), mSlaveAddr( slaveAddr ), mBusHandle( 0 ) {  
    AMP_DEBUG_I( "Starting I2C bus on addr %x", slaveAddr );
    i2c_slave_config_t conf = {};

    conf.i2c_port = I2C_NUM_0;
    conf.sda_io_num = (gpio_num_t)DSP_PIN_SDA;
    conf.scl_io_num = (gpio_num_t)DSP_PIN_SCL;
    conf.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    conf.send_buf_depth = 256;
    conf.clk_source = I2C_CLK_SRC_DEFAULT;
    conf.slave_addr = slaveAddr;

    ESP_ERROR_CHECK( i2c_new_slave_device( &conf, &mBusHandle ) );

    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ESP_ERROR_CHECK( i2c_slave_register_event_callbacks( mBusHandle, &cbs, this ) );
}

void 
I2CBUS::startReceive() {
    ESP_ERROR_CHECK( i2c_slave_receive( mBusHandle, mRecvBuffer, 1 ) );
}

void 
I2CBUS::handleReceiveData() {

}

