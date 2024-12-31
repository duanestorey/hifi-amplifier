#ifndef __STATE_H__
#define __STATE_H__

#include <stdio.h>
#include <stdlib.h>
#include "channelsel.h"

class AmplifierState {
public:
    AmplifierState() : mAudioType( AUDIO_DIGITAL ), mState( STATE_INIT ), mInput( INPUT_SPDIF_1 ), mSpeakerConfig( AUDIO_2_DOT_1 ), mCurrentAttenuation( 30 ), 
        mSamplingRate( 48000 ), mBitDepth( 24 ), mConnected( false ), mEnhancement( false ) {}

    enum {
        INPUT_UNDEFINED = 0,
        INPUT_STEREO_1 = ChannelSel::INPUT_STEREO_2,
        INPUT_STEREO_2 = ChannelSel::INPUT_STEREO_3,
        INPUT_STEREO_3 = ChannelSel::INPUT_STEREO_4,
        INPUT_SPDIF_1,
        INPUT_SPDIF_2,
        INPUT_SPDIF_3,
    };

    enum {
        AUDIO_ANALOG,
        AUDIO_PCM,
        AUDIO_DOLBY,
        AUDIO_DTS,
        AUDIO_DIGITAL
    } mAudioType;

    enum {
        STATE_INIT,
        STATE_PLAYING,
        STATE_MUTED,
        STATE_SLEEPING,
        STATE_UPDATING,
        STATE_ERROR_MINOR,
        STATE_ERROR_MAJOR
    } ;

    enum {
        AUDIO_2_CH,
        AUDIO_2_DOT_1
    };

    // Volume goes from 0-79
    uint8_t mState;
    uint8_t mInput;
    uint8_t mSpeakerConfig;
    uint8_t mCurrentAttenuation;
    uint32_t mSamplingRate;
    uint8_t mBitDepth;

    bool mConnected;
    bool mEnhancement;
};

#endif