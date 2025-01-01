#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "biquad.h"
#include <vector>

typedef std::vector<Biquad *> Biquads;

class Pipeline {
    public:
        Pipeline();
        ~Pipeline();

        void addBiquadLeft( Biquad *biquad );
        void addBiquadRight( Biquad *biquad );
        void resetDelayLines();
        float *process( float *data, uint32_t samples = 0 );
        int16_t *process( int16_t *data, uint32_t samples = 0 );
        int32_t *process( int32_t *data, uint32_t samples = 0 );

        void setAttenuation( float leftLevel, float rightLevel );
        void resetAll();
        void destroy();

        void generate( uint32_t samplingRate );
        void setSamples( uint32_t samples );
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
        void process( uint32_t samples );

        void checkAllocateFloat();
        void checkAllocateSigned32();
        void checkAllocateSigned16();

        void deallocate();


        inline int32_t convertFloatToInt32( float f );
        inline int16_t convertFloatToInt16( float f );
};

#endif