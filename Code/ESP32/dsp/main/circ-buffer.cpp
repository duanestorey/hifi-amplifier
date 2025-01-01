#include "circ-buffer.h"
#include <memory.h>

Circ_Buffer::Circ_Buffer( uint32_t bufferSizeInBytes ) : mBufferSize( bufferSizeInBytes ), mDataReady( 0 ), mOriginPointer( 0 ), mReadPtr( 0 ), mWritePtr( 0 ) {
    mOriginPointer = (uint8_t *)aligned_alloc( 32, bufferSizeInBytes );
    mEndPointer = mOriginPointer + bufferSizeInBytes;

    mReadPtr = mOriginPointer;
    mWritePtr = mOriginPointer;
}

Circ_Buffer::~Circ_Buffer() {
    if ( mOriginPointer ) {
        free( mOriginPointer );
        mOriginPointer = 0;
    }
}

bool
Circ_Buffer::read( uint32_t readSize, uint8_t *&sourcePtr1, uint32_t &dataSize1, uint8_t *&sourcePtr2, uint32_t &dataSize2 ) {
    if ( readSize <= dataReady() ) {
        if ( ( mReadPtr + readSize ) < mEndPointer ) {
            sourcePtr1 = mReadPtr;
            dataSize1 = readSize;

            sourcePtr2 = 0;
            dataSize2 = 0;

            mReadPtr += readSize;
        } else {
            // needs two reads
            uint32_t dataSize1 = mEndPointer - mReadPtr;
            uint32_t dataSize2 = readSize - dataSize1;
            sourcePtr1 = mReadPtr;
            sourcePtr2 = mOriginPointer;

            mReadPtr = mOriginPointer + dataSize2;
        }

        mDataReady -= readSize;
        return true;
    } else {
        return false;
    }
}

bool
Circ_Buffer::write( const uint8_t *source, uint32_t dataToWrite ) {     
    if ( dataToWrite <= writeSpaceAvail() ) {
        if ( ( mWritePtr + dataToWrite ) < mEndPointer ) {
            // can do it in one write
            memcpy( mWritePtr, source, dataToWrite );
            mWritePtr += dataToWrite;
        } else {
            // needs two writes
            uint32_t firstWrite = mEndPointer - mWritePtr;
            uint32_t secondWrite = dataToWrite - firstWrite;

            memcpy( mWritePtr, source, firstWrite );
            memcpy( mOriginPointer, &source[ firstWrite ], secondWrite );
            mWritePtr = mOriginPointer + secondWrite;
        }

        mDataReady += dataToWrite;
        return true;
    } else {
        return false;
    }
}

uint32_t 
Circ_Buffer::writeSpaceAvail() const {
    return mBufferSize - mDataReady;
}

uint32_t
Circ_Buffer::dataReady() const {
    return mDataReady;
}

uint8_t *
Circ_Buffer::getReadPointer() {
    return mReadPtr;
}