#include "biquad.h"
#include <memory.h>
#include <esp_dsp.h>
#include "debug.h"

Biquad::Biquad( uint8_t filterType, float cutoffFreq, float Q, float gain ) : mFilterType( filterType ), mCutoffFreq( cutoffFreq ), mQ( Q ), mGain( gain ) {
    resetAll();
}

void 
Biquad::generate( uint32_t samplingRate ) {
    float cutoff = mCutoffFreq / samplingRate;

    actualGenerate( mFilterType, cutoff, mQ, mGain );
    resetDelayLine();

    AMP_DEBUG_I( "Generated biquad with coefficients %0.10f, %0.10f, %0.10f, %0.10f, %0.10f", mCoeffs[0], mCoeffs[1], mCoeffs[2], mCoeffs[3], mCoeffs[4]);
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
Biquad::generatePeaking( float cutoff, float Q, float gain ) {
    float omega = 2*M_PI*cutoff;
    float alpha = sin( omega ) / ( 2*Q );
    float A = sqrt( pow( (float)10.0, gain/(float)20.0 ) );

    float a0 = 1 + alpha/A;

    mCoeffs[0] = (1 + A*alpha)/a0;
    mCoeffs[1] = (-2*cos( omega ))/a0;
    mCoeffs[2] = (1 - A*alpha)/a0;

    mCoeffs[3] = (-2*cos( omega ))/a0;
    mCoeffs[4] = (1 - alpha/A)/a0;
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
            generatePeaking( cutoff, Q, gain );
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