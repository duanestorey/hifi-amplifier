#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "biquad.h"
#include <vector>

typedef std::vector<Biquad *> Biquads;

class Pipeline {
    public:
        Pipeline( uint32_t samples );
        ~Pipeline();

        void addBiquadLeft( Biquad *biquad );
        void addBiquadRight( Biquad *biquad );
        void resetDelayLines();
        float *process( float *data );
        int16_t *process( int16_t *data );
        int32_t *process( int32_t *data );

        void setAttenuation( float leftLevel, float rightLevel );
        void resetAll();

        void generate( uint32_t samplingRate );
        void modifySamples( uint32_t samples );
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
        bool mHasAttenuation;
    private:
        void process();

        void allocate();
        void deallocate();
        inline int32_t convertFloatToInt32( float f );
        inline int16_t convertFloatToInt16( float f );
};

#endif