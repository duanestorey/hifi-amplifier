#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "biquad.h"
#include <vector>
#include "profile.h"

typedef std::vector<Biquad *> Biquads;

class Pipeline {
    public:
        Pipeline( Profile *profile );
        ~Pipeline();

        void addBiquadLeft( Biquad *biquad );
        void addBiquadRight( Biquad *biquad );
        void resetDelayLines();
        float *process( float *data, uint32_t samples = 0 );
        int16_t *process( int16_t *data, uint32_t samples = 0 );
        int32_t *process( int32_t *data, uint32_t samples = 0 );

        void resetAll();
        void destroy();

        void generate( uint32_t samplingRate );
        void setSamples( uint32_t samples );
        uint8_t getBiquadCount() const { return mBiquads[0].size() + mBiquads[1].size(); }

        int32_t convertFloatToInt32( float f );
        int16_t convertFloatToInt16( float f );

        void setAttenuationLeft( float level );
        void setAttenuationRight( float level );
    protected:
        uint32_t mSamples;
        float *mInputLeft;
        float *mInputRight;
        float *mOutputLeft;
        float *mOutputRight;
        
        float *mOutputFloat;
        int32_t *mOutput32s;
        int16_t *mOutput16s;

        Biquads mBiquads[ 2 ];
        float mAttenuation[ 2 ];
        float mGainFactor[ 2 ];
        bool mHasAttenuation[ 2 ];

        Profile *mProfile;
    private:
        void process( uint32_t samples );

        void checkAllocateFloat();
        void checkAllocateSigned32();
        void checkAllocateSigned16();

        void deallocate();
};

#endif