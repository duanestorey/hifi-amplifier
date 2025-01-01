#ifndef __CIRC_BUFFER_H__
#define __CIRC_BUFFER_H__

#include "config.h"

class Circ_Buffer {
    public:
        Circ_Buffer( uint32_t bufferSizeInBytes );
        ~Circ_Buffer();

        uint32_t dataReady() const;
        uint32_t writeSpaceAvail() const;
        uint8_t *getReadPointer();

        bool write( const uint8_t *source, uint32_t dataToWrite );
        bool read( uint32_t readSize, uint8_t *&sourcePtr1, uint32_t &dataSize1, uint8_t *&sourcePtr2, uint32_t &dataSize2 );
    protected:
        uint8_t mBufferSize;
        uint8_t mDataReady;

        uint8_t *mOriginPointer;
        uint8_t *mEndPointer;

        uint8_t *mReadPtr;
        uint8_t *mWritePtr;
};

#endif