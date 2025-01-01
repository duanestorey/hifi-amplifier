#include "i2s.h"
#include "config.h"
#include <memory.h>

I2S::I2S( Queue queue ) : mQueue( queue ), mSamplingRate( 0 ), mBitDepth( 16 ), mBytesWaiting( 0 ), mTxHandle( 0 ), mRxHandle( 0 ), mTransmitStarted( false ) {
    memset( &mConfig, 0, sizeof( mConfig ) );
}

void 
I2S::handleReceiveCallback( i2s_event_data_t *event ) {
    if ( event->size ) {
        mBytesWaiting += event->size;
        mQueue.addFromISR( Message::MSG_I2S_RECV );
    }
}

bool i2s_recv_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *context ) {
    I2S *i2s = (I2S *)context;
    i2s->handleReceiveCallback( event );
    return true;
}

void 
I2S::readData( uint8_t *data, size_t maxSize, size_t &dataSize ) {
    i2s_channel_read( mRxHandle, data, maxSize, &dataSize, DSP_I2S_MAX_TIMEOUT );
}

void 
I2S::writeData( uint8_t *data, size_t maxSize, size_t &dataSize ) {
    if ( !mTransmitStarted ) {
        i2s_channel_preload_data( mTxHandle, data, maxSize, &dataSize );
        i2s_channel_enable( mTxHandle );
    } else {
        i2s_channel_write( mTxHandle, data, maxSize, &dataSize, DSP_I2S_MAX_TIMEOUT );
    }
}

void 
I2S::start( uint32_t samplingRate ) {
    mSamplingRate = samplingRate;

    /* Allocate a pair of I2S channel */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG( I2S_NUM_AUTO, I2S_ROLE_SLAVE );
    /* Allocate for TX and RX channel at the same time, then they will work in full-duplex mode */
    i2s_new_channel( &chan_cfg, &mTxHandle, &mRxHandle );

    /* Set the configurations for BOTH TWO channels, since TX and RX channel have to be same in full-duplex mode */
    mConfig = {
        .clk_cfg = {
            .sample_rate_hz = samplingRate,
            .clk_src = I2S_CLK_SRC_EXTERNAL,
            .ext_clk_freq_hz = 24576000,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,  
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG( I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO ),
        .gpio_cfg = {
            .mclk = DSP_PIN_MCLK,
            .bclk = DSP_PIN_BCLK,
            .ws = DSP_PIN_LRCLK,
            .dout = DSP_PIN_DOUT,
            .din = DSP_PIN_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    if ( mBitDepth == 16 ) {
        mConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
        mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT;
    } else if ( mBitDepth == 24 ) {
        mConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_24BIT;
        mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT;
    } else if ( mBitDepth == 32 ) {
        mConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
        mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT;
    }

    i2s_channel_init_std_mode( mTxHandle, &mConfig );
    i2s_channel_init_std_mode( mRxHandle, &mConfig );

    i2s_event_callbacks_t i2s_callbacks = {
        .on_recv = i2s_recv_callback,
        .on_recv_q_ovf = 0,
        .on_sent = 0,
        .on_send_q_ovf = 0
    };

    i2s_channel_register_event_callback( mRxHandle, &i2s_callbacks, (void *)this );

    i2s_channel_enable( mRxHandle );
}