#ifndef __I2CBUS_H__
#define __I2CBUS_H__

#include "config.h"
#include "driver/i2c_slave.h"
#include "esp_attr.h"
#include "queue.h"


class I2CBUS {
public:
    I2CBUS( uint8_t slaveAddr, Queue queue  );

    bool writeBytes( uint8_t address, uint8_t *data, uint8_t size );
    void handleReceiveData( const i2c_slave_rx_done_event_data_t *data );
    void processData();
protected:
    Queue mQueue;
    uint8_t mSlaveAddr;
    i2c_slave_dev_handle_t mBusHandle;
    uint8_t mRecvBuffer[ DSP_I2C_BUFFER_SIZE ];
    i2c_slave_rx_done_event_data_t mRecvBufferData;
private:
    void startReceive();
};

#endif