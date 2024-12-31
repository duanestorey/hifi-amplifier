#ifndef __CHANNELSEL_H__
#define __CHANNELSEL_H__

#include <stdio.h>
#include <stdlib.h>

class ChannelSel {
public:
    enum {
        INPUT_STEREO_1 = 1,
        INPUT_STEREO_2 = 2,
        INPUT_STEREO_3 = 3,
        INPUT_STEREO_4 = 4, 
        INPUT_6CH = 5
    };

    enum {
        CHANNEL_FRONT_LEFT = 0,
        CHANNEL_FRONT_RIGHT = 1,
        CHANNEL_REAR_LEFT = 2,
        CHANNEL_REAR_RIGHT = 3,
        CHANNEL_CENTER = 4,
        CHANNEL_SUBWOOFER = 5,
        CHANNEL_ALL = 6
    };

    virtual void init() = 0;
    virtual void setInput( uint8_t input ) = 0;
    virtual void mute( bool mute ) = 0;
    virtual void muteChannel( uint8_t channel, bool mute ) = 0;
    virtual void setEnhancement( bool enhancementOn ) = 0;

    virtual bool setAttenuation( uint8_t att ) = 0;
    virtual bool setChannelAttenuation( uint8_t channel, uint8_t att ) = 0;
};

#endif