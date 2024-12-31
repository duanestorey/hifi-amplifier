#ifndef __CHANNELSEL_AX2358_H__
#define __CHANNELSEL_AX2358_H__

#define         AX2358_CMD_STEREO_1         0b11001011        
#define         AX2358_CMD_STEREO_2         0b11001010       
#define         AX2358_CMD_STEREO_3         0b11001001       
#define         AX2358_CMD_STEREO_4         0b11001000        
#define         AX2358_CMD_6CH              0b11001111       

#define         AX2358_VOL_ALL_1            0b11100000
#define         AX2358_VOL_ALL_10           0b11010000
#define         AX2358_VOL_CH1_1            0b10010000
#define         AX2358_VOL_CH1_10           0b10000000
#define         AX2358_VOL_CH2_1            0b01010000
#define         AX2358_VOL_CH2_10           0b01000000
#define         AX2358_VOL_CH3_1            0b00010000
#define         AX2358_VOL_CH3_10           0b00000000
#define         AX2358_VOL_CH4_1            0b00110000
#define         AX2358_VOL_CH4_10           0b00100000
#define         AX2358_VOL_CH5_1            0b01110000
#define         AX2358_VOL_CH5_10           0b01100000
#define         AX2358_VOL_CH6_1            0b10110000
#define         AX2358_VOL_CH6_10           0b10100000
#define         AX2358_VOL_CHAN_CLEAR       0b11000100

#define         AX2358_MUTE_FL              0b11110000
#define         AX2358_MUTE_FR              0b11110010
#define         AX2358_MUTE_CT              0b11110100
#define         AX2358_MUTE_SW              0b11110110
#define         AX2358_MUTE_RL              0b11111000
#define         AX2358_MUTE_RR              0b11111010

#define         AX2358_MUTE_ALL             0b11111111
#define         AX2358_MUTE_NONE            0b11111110

#define         AX2358_SURR_ENH_ON          0b11000000
#define         AX2358_SURR_ENH_OFF         0b11000001

#define         AX2358_MIX_6DB_ON           0b11000010
#define         AX2358_MIX_6DB_OFF          0b11000011

#include "channelsel.h"
#include "i2c-bus.h"

class ChannelSel_AX2358 : public ChannelSel {
public:
    ChannelSel_AX2358( uint8_t addr, I2CBUS *bus ) : mAddress( addr ), mI2C ( bus ), mVolume( 20 ) {}
    virtual void init();
    virtual void setInput( uint8_t input );
    virtual void mute( bool mute );
    virtual void muteChannel( uint8_t channel, bool mute );
    virtual void setEnhancement( bool enhancementOn );

    virtual bool setAttenuation( uint8_t att );
    virtual bool setChannelAttenuation(  uint8_t channel, uint8_t att );
protected:
    uint8_t mAddress;
    I2CBUS *mI2C;

    // 0 - 79
    uint8_t mVolume;
private:
    void writeChipValue( uint8_t value );
};

#endif