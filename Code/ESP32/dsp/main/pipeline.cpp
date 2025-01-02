#include "pipeline.h"
#include <limits>
#include <memory.h>
#include <esp_dsp.h>
#include "debug.h"
#include "dsp-custom.h"

Pipeline::Pipeline( Profile *profile ) : mSamples( 0 ), mInputLeft( 0 ), mInputRight( 0 ), mOutputLeft( 0 ), mOutputRight( 0 ), mOutputFloat( 0 ), 
    mOutput32s( 0 ), mOutput16s( 0 ), mProfile( profile ) {

    mAttenuation[ 0 ] = 0;
    mAttenuation[ 1 ] = 0;
    
    mGainFactor[ 0 ] = 1; 
    mGainFactor[ 1 ] = 1; 

    mHasAttenuation[ 0 ] = false;
    mHasAttenuation[ 1 ] = false;
}

Pipeline::~Pipeline() {
    destroy();
}

void Pipeline::setSamples( uint32_t samples ) {
    deallocate();
    
    mSamples = samples;
}

void 
Pipeline::resetAll() {
    mAttenuation[ 0 ] = 0;
    mAttenuation[ 1 ] = 0;
    mGainFactor[ 0 ] = 1; 
    mGainFactor[ 1 ] = 1; 

    mHasAttenuation[ 0 ] = false;
    mHasAttenuation[ 1 ] = false;

    for( Biquads::iterator i = mBiquads[ 0 ].begin(); i != mBiquads[ 0 ].end(); i++ ) {
        Biquad *biquad = (*i);
        delete biquad;
    }

    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        Biquad *biquad = (*i);
        delete biquad;
    }

    mBiquads[ 0 ].clear();
    mBiquads[ 1 ].clear();

    deallocate();
}

void 
Pipeline::setAttenuationLeft( float level ) {
    mAttenuation[ 0 ] = level;  
    mGainFactor[ 0 ] = 1.0f/pow( 10, level/20.0f );
    mHasAttenuation[ 0 ] = true;
}

void 
Pipeline::setAttenuationRight( float level ) {
    mAttenuation[ 1 ] = level;  
    mGainFactor[ 1 ] = 1.0f/pow( 10, level/20.0f );
    mHasAttenuation[ 1 ] = true;
}

void 
Pipeline::checkAllocateFloat() {
    if ( !mInputLeft ) {
        mInputLeft = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * mSamples );
    }
    
    if ( !mInputRight ) {
        mInputRight = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * mSamples );
    }
    
    if ( !mOutputLeft ) {
        mOutputLeft = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * mSamples );
    }

    if ( !mOutputRight ) {
        mOutputRight = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * mSamples );
    }

    if ( !mOutputFloat ) {
        mOutputFloat = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * 2 * mSamples );
    }
}

void 
Pipeline::checkAllocateSigned32() {
    if ( !mOutput32s ) {
        mOutput32s = (int32_t *)aligned_alloc( sizeof( int32_t ), sizeof( int32_t ) * 2 * mSamples );
    }
}

void 
Pipeline::checkAllocateSigned16() {
    if ( !mOutput16s ) {
        mOutput16s = (int16_t *)aligned_alloc( sizeof( int16_t ), sizeof( int16_t ) * 2 * mSamples );
    }
}

int32_t 
Pipeline::convertFloatToInt32( float f ) {
/*
    static int32_t max = std::numeric_limits<int32_t>::max();
    static int32_t min = std::numeric_limits<int32_t>::min();

    if ( f > max ) {
        return max;
    }

    if ( f < min ) {
        return min;
    }

    return std::round<int32_t>( f );
    */

    int result;
    asm("ROUND.S %0, %1, 0\t\n"
        : "=a" (result)
        : "f" (f));

    return result;
    
}

int16_t 
Pipeline::convertFloatToInt16( float f ) {
/*
    static int16_t max = std::numeric_limits<int16_t>::max();
    static int16_t min = std::numeric_limits<int16_t>::min();

    if ( f > max ) {
        return max;
    }

    if ( f < min ) {
        return min;
    }

    return std::round<int16_t>( f );
    */

    int result;
    asm("ROUND.S %0, %1, 0\t\n"
        : "=a" (result)
        : "f" (f));

    return result;
    
}

int16_t *
Pipeline::process( int16_t *data, uint32_t samples ) {
    if ( samples == 0 ) {
        samples = mSamples;
    }

    mProfile->startProfiling( "allocate" );

    checkAllocateFloat();
    checkAllocateSigned16();

   // AMP_DEBUG_I( "Allocation done" );

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    //AMP_DEBUG_I( "Converting" );

    // convert stereo inputs into two mono channels
    for ( uint32_t i = 0; i < samples*2; i++ ) {
       // mInputLeft[ i ] = (float)data[ i*2 ];
      //  mInputRight[ i ] = (float)data[ i*2 + 1 ];
        mOutputFloat[ i ] = (float)data[ i ];
    }

    mProfile->stopProfiling( "deinterleave" );

    //AMP_DEBUG_I( "Processing" );

    process( samples );

    mProfile->startProfiling( "output" );

    //AMP_DEBUG_I( "Converting back" );

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < samples*2; i++ ) {
       // mOutput16s[ i*2 ] = convertFloatToInt16( mOutputLeft[ i ] );
        //mOutput16s[ i*2 + 1 ] = convertFloatToInt16( mOutputRight[ i ] );
        mOutput16s[ i ] = convertFloatToInt16( mOutputFloat[ i ] );
    }

     mProfile->stopProfiling( "output" );

    return mOutput16s;
}


int32_t * 
Pipeline::process( int32_t *data, uint32_t samples ) {
    if ( samples == 0 ) {
        samples = mSamples;
    }

    mProfile->startProfiling( "allocate" );

    checkAllocateFloat();
    checkAllocateSigned32();

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    // convert stereo inputs into two mono channels
    for ( uint32_t i = 0; i < samples*2; i++ ) {
       // mInputLeft[ i ] = (float)data[ i*2 ];
       // mInputRight[ i ] = (float)data[ i*2 + 1 ];
        mOutputFloat[ i ] = (float)data[ i ];
    }

    mProfile->stopProfiling( "deinterleave" );

    process( samples );

    mProfile->startProfiling( "output" );

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < samples*2; i++ ) {
       // mOutput32s[ i*2 ] = convertFloatToInt32( mOutputLeft[ i ] );
      //  mOutput32s[ i*2 + 1 ] = convertFloatToInt32( mOutputRight[ i ] );
        mOutput32s[ i ] = convertFloatToInt32( mOutputFloat[ i ] );
    }

    mProfile->stopProfiling( "output" );

    return mOutput32s;
}

float * 
Pipeline::process( float *data, uint32_t samples ) {
    if ( samples == 0 ) {
        samples = mSamples;
    }

    mProfile->startProfiling( "allocate" );

    checkAllocateFloat();

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    // convert stereo inputs into two mono channels
    memcpy( mOutputFloat, data, sizeof( float ) * samples * 2 );
    /*
    for ( uint32_t i = 0; i < samples; i++ ) {
        mInputLeft[ i ] = data[ i*2 ];
        mInputRight[ i ] = data[ i*2 + 1 ];
    }
    */

    mProfile->stopProfiling( "deinterleave" );

    process( samples );

    mProfile->startProfiling( "output" );

    // convert two mono channels into one stereo channel for output
    /*
    for ( uint32_t i = 0; i < samples; i++ ) {
        mOutputFloat[ i*2 ] = mOutputLeft[ i ];
        mOutputFloat[ i*2 + 1 ] = mOutputRight[ i ];
    }
    */

    mProfile->stopProfiling( "output" );

    return mOutputFloat;
}

void 
Pipeline::process( uint32_t samples ) {
    bool first = true;
    for( Biquads::iterator i = mBiquads[ 0 ].begin(); i != mBiquads[ 0 ].end(); i++ ) {
       // dsps_biquad_f32_aes3( mInputLeft, mOutputLeft, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        mProfile->startProfiling( "biquadleft" );

        if ( first ) {
            dsps_biquad_f32_ae32( mInputLeft, mOutputLeft, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
            first = false;
        } else {
            dsps_biquad_f32_ae32( mOutputLeft, mOutputLeft, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        }

        mProfile->stopProfiling( "biquadleft" );
    }

    first = true;
    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        mProfile->startProfiling( "biquadright" );
       // dsps_biquad_f32_aes3( mInputRight, mOutputRight, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        if ( first ) {
            dsps_biquad_f32_ae32( mInputRight, mOutputRight, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
            first = false;
        } else {
            dsps_biquad_f32_ae32( mOutputRight, mOutputRight, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        }

        mProfile->stopProfiling( "biquadright" );

    }

    mProfile->startProfiling( "attenuate" );

    if ( mHasAttenuation[ 0 ] ) {
        if ( mBiquads[ 0 ].size() ) {
            dsps_mulc_f32_ae32( mOutputLeft, mOutputLeft, samples, mGainFactor[ 0 ], 1, 1 );
        } else {
            dsps_mulc_f32_ae32( mInputLeft, mOutputLeft, samples, mGainFactor[ 0 ], 1, 1 );
        }
    }

    if ( mHasAttenuation[ 1 ] ) {
        if ( mBiquads[ 1 ].size() ) {
            dsps_mulc_f32_ae32( mOutputRight, mOutputRight, samples, mGainFactor[ 1 ], 1, 1 );
        } else {
            dsps_mulc_f32_ae32( mInputRight, mOutputRight, samples, mGainFactor[ 1 ], 1, 1 );
        }
    }

    mProfile->stopProfiling( "attenuate" );
}

void 
Pipeline::deallocate() {
    if ( mInputLeft ) {
        free( mInputLeft );
        mInputLeft = 0;
    }

    if ( mInputRight ) {
        free( mInputRight );
        mInputRight = 0;
    }

    if ( mOutputLeft ) {
        free( mOutputLeft );
        mOutputLeft = 0;
    }

    if ( mOutputRight ) {
        free( mOutputRight );
        mOutputRight = 0;
    }    
    
    if ( mOutputFloat ) {
        free( mOutputFloat );
        mOutputFloat = 0;
    }   

    if ( mOutput32s ) {
        free( mOutput32s );
        mOutput32s = 0;
    } 
    
    if ( mOutput16s ) {
        free( mOutput16s );
        mOutput16s = 0;
    }    
}

void 
Pipeline::destroy() {
    deallocate();
}

void 
Pipeline::generate( uint32_t samplingRate ) {
    for( Biquads::iterator i = mBiquads[ 0 ].begin(); i != mBiquads[ 0 ].end(); i++ ) {
        (*i)->generate( samplingRate );
    }

    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        (*i)->generate( samplingRate );
    }
}

void 
Pipeline::addBiquadLeft( Biquad *biquad ) {
    mBiquads[ 0 ].push_back( biquad );
}

void 
Pipeline::addBiquadRight( Biquad *biquad ) {
    mBiquads[ 1 ].push_back( biquad );
}

void 
Pipeline::resetDelayLines() {
    for( Biquads::iterator i = mBiquads[ 0 ].begin(); i != mBiquads[ 0 ].end(); i++ ) {
        (*i)->resetDelayLine();
    }

    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        (*i)->resetDelayLine();
    }
}
