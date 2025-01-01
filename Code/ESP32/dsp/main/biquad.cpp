#include "biquad.h"
#include <memory.h>
#include <esp_dsp.h>

Biquad::Biquad( uint8_t filterType, float cutoffFreq, float Q, float gain ) : mFilterType( filterType ), mCutoffFreq( cutoffFreq ), mQ( Q ), mGain( gain ) {
    resetAll();
}

void 
Biquad::generate( uint32_t samplingRate ) {
    float cutoff = mCutoffFreq / samplingRate;

    actualGenerate( mFilterType, cutoff, mQ, mGain );
}

void 
Biquad::resetAll() {
    mCoeffs[ 0 ] = mCoeffs[ 1 ] =  mCoeffs[ 2 ] =  mCoeffs[ 3 ] =  mCoeffs[ 4 ] = mCoeffs[ 5 ] = 0;
    
    resetDelayLine();
}

void 
Biquad::resetDelayLine() {
    mDelayLine[ 0 ] = mDelayLine[ 1 ] = 0;
}

float *
Biquad::getCoefficients() {
    return mCoeffs;
}

float *
Biquad::getDelayLine() {
    return mDelayLine;
}

void 
Biquad::actualGenerate( uint8_t filterType, float cutoff, float Q, float gain ) {
    switch( filterType ) {
        case LOWPASS:
            dsps_biquad_gen_lpf_f32( mCoeffs, cutoff, Q );
            break;
        case HIGHPASS:
            dsps_biquad_gen_hpf_f32( mCoeffs, cutoff, Q );
            break;
        case BANDPASS:
            dsps_biquad_gen_bpf_f32( mCoeffs, cutoff, Q );
            break;
        case NOTCH:
            dsps_biquad_gen_notch_f32( mCoeffs, cutoff, gain, Q );
            break;
        case ALLPASS360:
            dsps_biquad_gen_allpass360_f32( mCoeffs, cutoff, Q );
            break;
        case ALLPASS180:
            dsps_biquad_gen_allpass180_f32( mCoeffs, cutoff, Q );
            break;
        case PEAKING:
            dsps_biquad_gen_peakingEQ_f32( mCoeffs, cutoff, Q );
            break;
        case LOWSHELF:
            dsps_biquad_gen_lowShelf_f32( mCoeffs, cutoff, gain, Q );
            break; 
        case HIGHSHELF:
            dsps_biquad_gen_highShelf_f32( mCoeffs, cutoff, gain, Q );
            break;         
    }

    resetDelayLine();
}