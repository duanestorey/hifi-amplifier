#ifndef __CS8416_H__
#define __CS8416_H__

#include "i2c-bus.h"

#define SPDIF_TRUNK             4
#define SPDIF_PDUR              8
#define SPDIF_FSWCLK            64
#define SPDIF_RUN               0b10000000
#define SPDIF_ERROR_ZERO        4
#define SPDIF_MASTER            0b10000000
#define SPDIF_FORMAT_I2S        0b00000101
#define SPDIF_FORMAT_LJ         0x0

class CS8416 {
public:
    enum {
        ADDR_CONTROL_0 = 0x00,
        ADDR_CONTROL_1 = 0x01,
        ADDR_CONTROL_2 = 0x02,
        ADDR_CONTROL_3 = 0x03,
        ADDR_CONTROL_4 = 0x04,
        ADDR_SERIAL_DATA_FORMAT = 0x05,
        ADDR_RECEIVER_ERROR_MASK = 0x06,
        ADDR_INTERRUPT_MASK = 0x07,
        ADDR_INTERRUPT_MODE_MSB = 0x08,
        ADDR_INTERRUPT_MODE_LSB = 0x09,
        ADDR_RECEIVER_CHANNEL_STATUS = 0x0a,
        ADDR_AUDIO_DATA_FORMAT = 0x0b,
        ADDR_RECEIVER_ERROR = 0x0c,
        ADDR_INTERRUPT_STATUS = 0x0d,
        ADDR_CLOCK_RATIO = 0x18,
        ADDR_CHANNEL_A_BYTE_3 = 0x1c,
        ADDR_CHANNEL_A_BYTE_4 = 0x1d,
        ADDR_ID_VERSION = 0x7f
    } I2C_ADDR;

    enum {
        SPDIF_INPUT_1 = 0x0,
        SPDIF_INPUT_2 = 0x1,
        SPDIF_INPUT_3 = 0x2
    } SPDIF_INPUTS;

    enum {
        SPDIF_FORMAT_PCM =      0b01000000,
        SPDIF_FORMAT_IEC61937 = 0b00100000,
        SPDIF_FORMAT_DTS_LD =   0b00010000,
        SPDIF_FORMAT_DTS_CD =   0b00001000
    } SPDIF_FORMATS;

    CS8416( uint8_t address, I2CBUS *bus ) : mAddress( address ), mBus( bus ), mRunning( false ), mInput( SPDIF_INPUT_1 ), mCurrentFormat( 0 ), mChipVersion( 0 ) {}

    void init();
    void run( bool enabled = true );
    uint8_t getFormat();

    void setInput( uint8_t input );
    uint8_t getInput() { return mInput; }

    bool isHighFrequency();
    bool isSilence();
    bool hasLoopLock();
    bool isValidPcm();
    bool isValidStream();

    uint8_t getErrorInfo();

    uint8_t getVersion();
    uint32_t getSamplingRate();
    uint8_t getBitDepth();
private:
    void updateRunCommand();
protected:
    uint8_t mAddress;
    I2CBUS *mBus;
    bool mRunning;
    uint8_t mInput;
    uint8_t mCurrentFormat;
    uint8_t mChipVersion;
};

#endif