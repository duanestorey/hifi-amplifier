#include "cs8416.h"
#include "debug.h"
#include "state.h"

void 
CS8416::init() {
    mChipVersion = getVersion();

    // set higher update rate phase detector for sigma delta DAC, and truncation
    // this won't work for > 96kHz, should disable PDUR for that
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_0, SPDIF_PDUR );

    // no auto clock switching, no muted zeros, active high interrupts, error switch to zeros,
    // master clock is 256*fs
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_1, SPDIF_ERROR_ZERO );

    // DETCI?, de-emphasis off (turn it on after?), GPIO0 is sample rate
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_2, 8 );

    // GPIO1 is non-audio detection, GPIO2 is GND
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_3, ( 0b1001 << 4 ) | 0b0000 );

    // output as a I2S master, 24 bit audio, left justified data, 128*fs was 0b11000000 

    // Left justified
    //mBus->writeRegisterByte( mAddress, CS8416::ADDR_SERIAL_DATA_FORMAT, SPDIF_MASTER | SPDIF_FORMAT_LJ );

    // I2S 
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_SERIAL_DATA_FORMAT, SPDIF_MASTER | SPDIF_FORMAT_I2S );

    // Error mask - give us information about PLL locking, PCM validity, and confidence
    mBus->writeRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR_MASK, 0b00011100 );
}

void 
CS8416::setInput( uint8_t input ) {
    if ( input == AmplifierState::INPUT_SPDIF_1 || input == AmplifierState::INPUT_SPDIF_2 || input == AmplifierState::INPUT_SPDIF_2 ) {
        switch( input) {
            case AmplifierState::INPUT_SPDIF_1:
                mInput = CS8416::SPDIF_INPUT_1;
                break;
            case AmplifierState::INPUT_SPDIF_2:
                mInput = CS8416::SPDIF_INPUT_2;
                break;
            case AmplifierState::INPUT_SPDIF_3:
                mInput = CS8416::SPDIF_INPUT_3;
                break;
        }

        // update input via control register
        updateRunCommand();
    }
}

void 
CS8416::updateRunCommand() {
    if ( mRunning ) {
        // enable running, add input selection
        uint8_t control = SPDIF_RUN | ( mInput << 3 );
        mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, control );
    } else {
        // disable running, add input selection
        uint8_t control = 0b00000000 | ( mInput << 3 );
        mBus->writeRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, control );
    }
}

void
CS8416::run( bool enable ) {
    if ( enable == mRunning ) {
        // already running or not running
        return;
    }

    mRunning = enable;

    updateRunCommand();
}

uint8_t 
CS8416::getBitDepth() {
    uint8_t value = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_CHANNEL_A_BYTE_4, value );
    AMP_DEBUG_I( "SPDIF raw channel 4 data is %#02x", value );

    if ( value & 0x01 ) {
        // max is 24 bit
        switch( ( value & 0x0e ) >> 1 ) {
            case 4:
                return 23;
            case 2:
                return 22;
            case 6:
                return 21;
            case 1:
                return 20;
            case 5:
                return 24;
            default:    
                // unknown
                return 0;
        }
    } else {
        // max is 20 bit
        switch( ( value & 0x0e ) >> 1 ) {
            case 4:
                return 19;
            case 2:
                return 18;
            case 6:
                return 17;
            case 1:
                return 16;
            case 5:
                return 20;
            default:    
                // unknown
                return 0;      
        }
    }
}


uint32_t 
CS8416::getSamplingRate() {
    uint8_t value = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_CHANNEL_A_BYTE_3, value );
    switch( value & 0x0f ) {
        case 0:
            return 44100;
        case 2:
            return 48000;
        case 3:
            return 32000;
        case 4:
            return 22050;
        case 5:
            return 0;
        case 6:
            return 24000;
        case 9:
            return 768000;
        case 10:
            return 96000;
        case 12:
            return 32000;
        case 14:
            return 192000;    
        default:
            return 0;
    }
}

uint8_t 
CS8416::getVersion() {
    uint8_t version = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_ID_VERSION, version );

    return ( version & 0x0f );
}

uint8_t 
CS8416::getFormat() {
    uint8_t format = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_CONTROL_4, format );

    mCurrentFormat = format;

    return mCurrentFormat & 0b01111000;
}

bool 
CS8416::isHighFrequency() {
    getFormat();

    return ( mCurrentFormat & 1 );
}

bool 
CS8416::isSilence() {
    getFormat();

    return ( mCurrentFormat & 2 );
}

uint8_t
CS8416::getErrorInfo() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );
    return status; 
}

bool 
CS8416::hasLoopLock() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns status of PLL locking
    return ( ( status & 16 ) == 0 );
}

bool 
CS8416::isValidPcm() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns the PCM validity bit being zeor
    return ( ( status & 8 ) == 0 );
}

bool 
CS8416::isValidStream() {
    uint8_t status = 0;
    mBus->readRegisterByte( mAddress, CS8416::ADDR_RECEIVER_ERROR, status );

    // returns the confidence bit, basically PLL is locked and there is no bi-phase error
    return ( ( status & 4 ) == 0 );
}

