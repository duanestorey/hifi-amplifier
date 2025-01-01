#ifndef __I2CBUS_H__
#define __I2CBUS_H__

#include "config.h"
#include "driver/i2c_slave.h"
#include "esp_attr.h"
#include "queue.h"
#include "i2c-listener.h"
#include <vector>

typedef std::vector<I2C_Listener *> Listeners;

class I2CBUS {
public:
    I2CBUS( uint8_t slaveAddr, Queue queue  );

    bool writeBytes( uint8_t *data, uint32_t bytesToWrite );
    void handleReceiveData( const i2c_slave_rx_done_event_data_t *data );
    void processData();
    void startReceive();
    void addEventListener( I2C_Listener *listener ) { mListeners.push_back( listener ); }
protected:
    Queue mQueue;
    uint8_t mSlaveAddr;
    i2c_slave_dev_handle_t mBusHandle;
    uint8_t mRecvBuffer[ DSP_I2C_BUFFER_SIZE ];
    i2c_slave_rx_done_event_data_t mRecvBufferData;
    Listeners mListeners;
private:

};

#endif