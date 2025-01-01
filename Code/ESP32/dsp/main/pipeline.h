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
        void generate( uint32_t samplingRate );
        float *process( float *data );
        int32_t *process( int32_t *data );
    protected:
        uint32_t mSamples;
        float *mInputLeft;
        float *mInputRight;
        float *mOutputLeft;
        float *mOutputRight;
        
        float *mOutputFloat;
        int32_t *mOutput32s;

        Biquads mBiquads[ 2 ];
    private:
        void process();

        void allocate( uint32_t samples );
        void deallocate();
        inline int32_t convertFloatToInt32( float f );
};

#endif