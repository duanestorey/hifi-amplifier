#include "i2s.h"
#include "config.h"
#include "debug.h"
#include <memory.h>

I2S::I2S( Queue &queue ) : mQueue( queue ) , mSamplingRate( 0 ), mPreloadedAmount( 0 ), mBitDepth( 16 ), mSlotDepth( 16 ),  
     mTransmitStarted( false ), mBuffer( 0 ) {
    memset( &mConfig, 0, sizeof( mConfig ) );
}

void 
I2S::handleReceiveCallback( i2s_event_data_t *event ) {
  //  AMP_DEBUG_INT_I( "In I2S callback %d", event->size );
    if ( event->size ) {
     //   mBuffer->write( (uint8_t *)event->dma_buf, event->size );
        mQueue.addFromISR( Message( Message::MSG_I2S_RECV, (int32_t) event->dma_buf, event->size ) );
    }
}

bool i2s_recv_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *context ) {
    I2S *i2s = (I2S *)context;
    i2s->handleReceiveCallback( event );
    return true;
}

bool 
I2S::readData( uint32_t readSize, uint8_t *&dataPtr1, size_t &dataSize1, uint8_t *&dataPtr2, size_t &dataSize2  ) {
    if ( mBuffer->dataReady() >= readSize ) {
        mBuffer->read( readSize, dataPtr1, dataSize1, dataPtr2, dataSize2 );
        return true;
    } else {
        dataPtr1 = dataPtr2 = 0;
        dataSize1 = dataSize2 = 0;

        return false;
    }  
}

void 
I2S::writeData( uint8_t *data, size_t maxSize, size_t &dataSize ) {
    if ( mTxHandle ) {
        if ( !mTransmitStarted ) {
            AMP_DEBUG_I( "Preloading data" );
            i2s_channel_preload_data( mTxHandle, data, maxSize, &dataSize );
            mPreloadedAmount += dataSize;

            uint32_t minPreloadSize = ( mSamplingRate * DSP_I2S_PRELOAD_SIZE_MS / 1000 ) * ( mSlotDepth / 8 ) * 2;

            if ( mPreloadedAmount >= minPreloadSize ) {
                i2s_channel_enable( mTxHandle );
                AMP_DEBUG_I( "Starting I2S output" );
                mTransmitStarted = true;
            }
        } else {    
            // AMP_DEBUG_I( "Normal i2S write data" );
            i2s_channel_write( mTxHandle, data, maxSize, &dataSize, DSP_I2S_MAX_TIMEOUT );

            if ( maxSize != dataSize ) {
                AMP_DEBUG_E( "Data size mismatch for I2s write" );
            }
        }   
    } else {
        dataSize = 0;
    }
}

void 
I2S::start( uint32_t samplingRate, uint8_t bitDepth, uint8_t slotDepth ) {
    mSamplingRate = samplingRate;
    mBitDepth = bitDepth;
    mSlotDepth = slotDepth;

    AMP_DEBUG_I( "Starting I2S at %lu Hz %d/%d", samplingRate, bitDepth, slotDepth );

    uint32_t circularSizeInBytes = ( ( mSamplingRate * 2 * mSlotDepth / 8 ) * DSP_I2S_PRELOAD_SIZE_MS ) / 1000;
   // mBuffer = new Circ_Buffer( circularSizeInBytes );
    AMP_DEBUG_I( "Creating circular buffer of size %lu", circularSizeInBytes );

    /* Allocate a pair of I2S channel */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG( I2S_NUM_AUTO, I2S_ROLE_SLAVE );
    /* Allocate for TX and RX channel at the same time, then they will work in full-duplex mode */

     AMP_DEBUG_I( "Creating I2S channels" );
    i2s_new_channel( &chan_cfg, &mTxHandle, &mRxHandle );

    /* Set the configurations for BOTH TWO channels, since TX and RX channel have to be same in full-duplex mode */
    
    mConfig = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(samplingRate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG( I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO ),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
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
    } else if ( mBitDepth == 24 ) {
        mConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_24BIT;
    } else if ( mBitDepth == 32 ) {
        mConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    }

    if ( mSlotDepth == 16 ) {
        //mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT;
    } else if ( mSlotDepth == 24 ) {
        mConfig.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_192;
      //  mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_24BIT;
    } else if ( mSlotDepth == 32 ) {
       // mConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT;
    }

    AMP_DEBUG_I( "Init I2S TX" ); 
    i2s_channel_init_std_mode( mTxHandle, &mConfig );

    AMP_DEBUG_I( "Init I2S RX" ); 
    i2s_channel_init_std_mode( mRxHandle, &mConfig );

    i2s_event_callbacks_t i2s_callbacks = {
        .on_recv = i2s_recv_callback,
        .on_recv_q_ovf = 0,
        .on_sent = 0,
        .on_send_q_ovf = 0
    };

    AMP_DEBUG_I( "Register callbacks" ); 
    i2s_channel_register_event_callback( mRxHandle, &i2s_callbacks, (void *)this );

    AMP_DEBUG_I( "Enable RX" ); 
    i2s_channel_enable( mRxHandle );

    uint8_t *buffer = new uint8_t[ 1024 ];
    size_t read;
   // i2s_channel_read( mRxHandle, buffer, 1024, &read, 1000 );
  //  AMP_DEBUG_I( "Just read %d bytes from I2S", read );
}

void 
I2S::stop() {
    if ( mTxHandle ) {
        i2s_channel_disable( mTxHandle );
        i2s_del_channel( mTxHandle );
        mTxHandle = 0;
    }

    if ( mRxHandle ) {
        i2s_channel_disable( mRxHandle );
        i2s_del_channel( mRxHandle );
        mRxHandle = 0;
    }

    if ( mBuffer ) {
        delete mBuffer;
        mBuffer = 0;
    }

    memset( &mConfig, 0, sizeof( mConfig ) );
}