#include "channelsel-ax2358.h"
#include "debug.h"

#define AX2358_MAX_VOL      79

void 
ChannelSel_AX2358::writeChipValue( uint8_t value ) {
    uint8_t data[2];
    data[0] = value;
    data[1] = value;

    mI2C->writeBytes( mAddress, &value, 1 );

    AMP_DEBUG_I( "Writing two bytes data to CHANNELSEL %d", (int)value );
}

void 
ChannelSel_AX2358::init() {
    // Need to clear registers
    writeChipValue( AX2358_VOL_CHAN_CLEAR ); 
    
    // Let's mute the output
  //  mute( true );

    // Set the volume levels to 20 to start
    setAttenuation( mVolume );

    // Enable 6db reduction for combined channels
    writeChipValue( AX2358_MIX_6DB_ON );
}

void 
ChannelSel_AX2358::setEnhancement( bool enhancementOn ) {
    if ( enhancementOn ) {
        // Enable surround enhancement
        writeChipValue( AX2358_SURR_ENH_ON ); 
    } else {
        // Enable surround enhancement
        writeChipValue( AX2358_SURR_ENH_OFF ); 
    }
}

void 
ChannelSel_AX2358::setInput( uint8_t input ) {
    switch( input ) {
        case ChannelSel::INPUT_STEREO_1:
            writeChipValue( AX2358_CMD_STEREO_1 );
            break;
        case ChannelSel::INPUT_STEREO_2:
            writeChipValue( AX2358_CMD_STEREO_2 );
            break;
        case ChannelSel::INPUT_STEREO_3:
            writeChipValue( AX2358_CMD_STEREO_3 );
            break;
        case ChannelSel::INPUT_STEREO_4:
            writeChipValue( AX2358_CMD_STEREO_4 );
            break;
        case ChannelSel::INPUT_6CH:
            writeChipValue( AX2358_CMD_6CH );
            break;
        default:
            break;
    }
    
}

void 
ChannelSel_AX2358::mute( bool mute ) {
    if ( mute ) {
        writeChipValue( AX2358_MUTE_ALL ); 
    } else {
        writeChipValue( AX2358_MUTE_NONE ); 
    }
}

void
ChannelSel_AX2358::muteChannel( uint8_t channel, bool mute ) {
    uint8_t muteBit = ( mute ? 1 : 0 );
    switch( channel ) {
        case ChannelSel::CHANNEL_FRONT_LEFT:    
            writeChipValue( AX2358_MUTE_FL | muteBit ); 
            break;
        case ChannelSel::CHANNEL_FRONT_RIGHT:   
            writeChipValue( AX2358_MUTE_FR | muteBit ); 
            break;
        case ChannelSel::CHANNEL_CENTER:
            writeChipValue( AX2358_MUTE_CT | muteBit ); 
            break;
        case ChannelSel::CHANNEL_SUBWOOFER:
            writeChipValue( AX2358_MUTE_SW | muteBit ); 
            break;
        case ChannelSel::CHANNEL_REAR_LEFT:
            writeChipValue( AX2358_MUTE_RL | muteBit ); 
            break;
        case ChannelSel::CHANNEL_REAR_RIGHT:
            writeChipValue( AX2358_MUTE_RR | muteBit ); 
            break;
        case ChannelSel::CHANNEL_ALL:
            this->mute( mute );
            break;
    }
}

bool
ChannelSel_AX2358::setAttenuation( uint8_t att ) {
    setChannelAttenuation( ChannelSel::CHANNEL_ALL, att );

    return true;
}

bool 
ChannelSel_AX2358::setChannelAttenuation( uint8_t channel, uint8_t att ) {
    uint8_t volume = AX2358_MAX_VOL;
    if ( att > volume ) {
        volume = 0;
    } else {
        volume = volume - att;
    }

    uint8_t negVol = 79-volume; 
    uint8_t onesDigit = negVol % 10;
    uint8_t tensDigit = negVol / 10;

    uint8_t data[2] = {0};

    switch( channel ) {
        case ChannelSel::CHANNEL_FRONT_LEFT:
            data[ 0 ] = AX2358_VOL_CH1_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH1_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_FRONT_RIGHT:
            data[ 0 ] = AX2358_VOL_CH2_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH2_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_REAR_LEFT:
            data[ 0 ] = AX2358_VOL_CH3_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH3_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_REAR_RIGHT:
            data[ 0 ] = AX2358_VOL_CH4_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH4_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_CENTER:
            data[ 0 ] = AX2358_VOL_CH5_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH5_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_SUBWOOFER:
            data[ 0 ] = AX2358_VOL_CH6_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_CH6_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
            break;
        case ChannelSel::CHANNEL_ALL:
            data[ 0 ] = AX2358_VOL_ALL_10 | tensDigit;
            data[ 1 ] = AX2358_VOL_ALL_1 | onesDigit;
            return mI2C->writeBytes( mAddress, data, 2 );
        default:
            return false;
            break;
    }   
}