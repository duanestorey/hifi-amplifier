#include <stdio.h>
#include "i2c-bus.h"
#include "sdkconfig.h"
#include "config.h"
#include "debug.h"
#include "driver/i2c_slave.h"

static bool i2c_slave_request_cb(i2c_slave_dev_handle_t channel, const i2c_slave_request_event_data_t *edata, void *userData ) {
   // ((I2CBUS *)userData)->handleReceiveData( edata ); 
    return false;
}

static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *userData )
{
    ((I2CBUS *)userData)->handleReceiveData( edata ); 
    return false;
}

I2CBUS::I2CBUS( uint8_t slaveAddr, Queue queue ) : mQueue( queue ), mSlaveAddr( slaveAddr ), mBusHandle( 0 ) {  
    AMP_DEBUG_I( "Starting I2C bus on addr %x", slaveAddr );
    i2c_slave_config_t conf = {};

    conf.i2c_port = I2C_NUM_0;
    conf.sda_io_num = DSP_PIN_SDA;
    conf.scl_io_num = DSP_PIN_SCL;
    conf.addr_bit_len = I2C_ADDR_BIT_LEN_7;
    conf.send_buf_depth = 100;
    conf.receive_buf_depth = 100,
    conf.clk_source = I2C_CLK_SRC_DEFAULT;
    conf.slave_addr = slaveAddr;

    ESP_ERROR_CHECK( i2c_new_slave_device( &conf, &mBusHandle ) );
}

void 
I2CBUS::processData() {
    if ( mRecvBufferData.length >= 1 ) {
        uint8_t reg = mRecvBufferData.buffer[ 0 ];

        for ( Listeners::iterator i = mListeners.begin(); i != mListeners.end(); i++ ) {
            (*i)->onNewI2CData( reg, &mRecvBufferData.buffer[ 1 ], mRecvBufferData.length - 1 );
        }
    }
}

void 
I2CBUS::startReceive() {
    i2c_slave_event_callbacks_t cbs;
    cbs.on_receive = i2c_slave_receive_cb;
    cbs.on_request = i2c_slave_request_cb;

    ESP_ERROR_CHECK( i2c_slave_register_event_callbacks( mBusHandle, &cbs, this ) );
}

void 
I2CBUS::handleReceiveData( const i2c_slave_rx_done_event_data_t *data ) {
    mRecvBufferData = *data;

    mQueue.addFromISR( Message::MSG_I2C );
}

bool 
I2CBUS::writeBytes( uint8_t *data, uint32_t bytesToWrite ) {
    uint32_t bytesWritten = 0;

    i2c_slave_write( mBusHandle, data, bytesToWrite, &bytesWritten, 100 );

    return ( bytesToWrite == bytesWritten );
}
