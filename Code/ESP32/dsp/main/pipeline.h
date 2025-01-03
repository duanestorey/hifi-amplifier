#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "biquad.h"
#include <vector>
#include "profile.h"

typedef std::vector<Biquad *> Biquads;

struct ChannelInfo {
    ChannelInfo() {}
    ChannelInfo( uint8_t source ) : mSource( source ) { reset(); }
    void reset() { mAttenuation = 0; mGainFactor = 1; mHasAttenuation = false; }
    
    uint8_t mSource;

    float mAttenuation;
    float mGainFactor;
    bool mHasAttenuation;
    Biquads mBiquads;
};

class Pipeline {
    public:
        enum {
            SOURCE_LEFT = 0,
            SOURCE_RIGHT = 1,
            SOURCE_COMBINED = 2,
            SOURCE_COMBINED_6DB = 3
        };

        enum {
            CHANNEL_LEFT = 0,
            CHANNEL_RIGHT = 1
        };

        Pipeline( Profile *profile );
        ~Pipeline();

        void addBiquad( uint8_t which, Biquad *biquad );
        void addBiquadLeft( Biquad *biquad );
        void addBiquadRight( Biquad *biquad );

        void setAttenuation( uint8_t which, float level );
        void setAttenuationLeft( float level );
        void setAttenuationRight( float level );

        void setSource( uint8_t which, uint8_t source );
        void setLeftSource( uint8_t source );
        void setRightSource( uint8_t source );

        void resetDelayLines();

        float *process( float *data, uint32_t samples );
        int16_t *process( int16_t *data, uint32_t samples );
        int32_t *process( int32_t *data, uint32_t samples );

        void resetAll();
        void destroy();

        void generate( uint32_t samplingRate );
        uint8_t getBiquadCount() const { return mChannels[ CHANNEL_LEFT ].mBiquads.size() + mChannels[ CHANNEL_RIGHT ].mBiquads.size(); }
        inline void dsps_biquad_f32_skip_transposed2( const float *input, float *output, uint32_t samples, float *coeff, float *delay );
    protected:
        ChannelInfo mChannels[2];

        uint32_t mSamples;
        
        float *mOutputFloat;
        int32_t *mOutput32s;
        int16_t *mOutput16s;

        Profile *mProfile;
    private:
        void processInternal( uint32_t samples );

        void checkAllocate( uint32_t samples );
        void deallocate();

        inline int32_t convertFloatToInt32( float f );
        inline int16_t convertFloatToInt16( float f );
        inline float convertInt32ToFloat( int32_t i );
        inline float convertInt32ToFloatHalf( int32_t i );

        void buildChannelsInt16( int16_t *data, uint32_t samples );
        void buildChannelsInt32( int32_t *data, uint32_t samples );
        void buildChannelsFloat( float *data, uint32_t samples );
};

#endif