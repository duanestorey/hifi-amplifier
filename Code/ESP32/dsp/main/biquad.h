#ifndef __BIQUAD_H__
#define __BIQUAD_H__

#include "config.h"
#include <math.h>

class Biquad {
    public:
        enum {
            LOWPASS = 0,
            HIGHPASS = 1,
            BANDPASS = 2,
            NOTCH = 3,
            ALLPASS360 = 4,
            ALLPASS180 = 5,
            PEAKING = 6,
            LOWSHELF = 7,
            HIGHSHELF = 8
        };

        Biquad( uint8_t filterType, float cutoffFreq, float Q, float gain = 0 );

        float *getCoefficients();
        float *getDelayLine();

        void generate( uint32_t samplingRate );
        void resetAll();
        void resetDelayLine();
    private:
        void actualGenerate( uint8_t filterType, float cutoff, float Q, float gain = 0 );
    protected:
        uint8_t mFilterType;
        float mCutoffFreq;
        float mQ;
        float mGain;

        float mCoeffs[ 6 ];
        float mDelayLine[ 2 ];
};

#endif