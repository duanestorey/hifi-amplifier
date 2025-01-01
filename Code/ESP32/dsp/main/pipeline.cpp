#include "pipeline.h"
#include <limits>
#include <memory.h>
#include <esp_dsp.h>

Pipeline::Pipeline() : mSamples( 0 ), mInputLeft( 0 ), mInputRight( 0 ), mOutputLeft( 0 ), mOutputRight( 0 ), mOutputFloat( 0 ), 
    mOutput32s( 0 ), mHasAttenuation( false ) {

    mAttenuation[ 0 ] = 0;
    mAttenuation[ 1 ] = 0;
    
    mGainFactor[ 0 ] = 1; 
    mGainFactor[ 1 ] = 1; 
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

    mHasAttenuation = false;

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
Pipeline::setAttenuation( float leftLevel, float rightLevel ) {
    mAttenuation[ 0 ] = leftLevel;
    mAttenuation[ 1 ] = rightLevel;
    
    mGainFactor[ 0 ] = 1.0f/pow( 10, leftLevel/20.0f );
    mGainFactor[ 1 ] = 1.0f/pow( 10, rightLevel/20.0f );

    mHasAttenuation = true;
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
    static int32_t max = std::numeric_limits<int32_t>::max();
    static int32_t min = std::numeric_limits<int32_t>::min();

    if ( f > max ) {
        return max;
    } else if ( f < min ) {
        return min;
    } else return std::round<int32_t>( f );
}

int16_t 
Pipeline::convertFloatToInt16( float f ) {
    static int16_t max = std::numeric_limits<int16_t>::max();
    static int16_t min = std::numeric_limits<int16_t>::min();

    if ( f > max ) {
        return max;
    } else if ( f < min ) {
        return min;
    } else return std::round<int16_t>( f );
}

int16_t *
Pipeline::process( int16_t *data ) {
    checkAllocateFloat();
    checkAllocateSigned16();

    // convert stereo inputs into two mono channels
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mInputLeft[ i ] = (float)data[ i*2 ];
        mInputRight[ i ] = (float)data[ i*2 + 1 ];
    }

    process();

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mOutput16s[ i*2 ] = convertFloatToInt16( mOutputLeft[ i ] );
        mOutput16s[ i*2 + 1 ] = convertFloatToInt16( mOutputRight[ i ] );
    }

    return mOutput16s;
}

float * 
Pipeline::process( float *data ) {
    checkAllocateFloat();

    // convert stereo inputs into two mono channels
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mInputLeft[ i ] = data[ i*2 ];
        mInputRight[ i ] = data[ i*2 + 1 ];
    }

    process();

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mOutputFloat[ i*2 ] = mOutputLeft[ i ];
        mOutputFloat[ i*2 + 1 ] = mOutputRight[ i ];
    }

    return mOutputFloat;
}

int32_t * 
Pipeline::process( int32_t *data ) {
    checkAllocateFloat();
    checkAllocateSigned32();

    // convert stereo inputs into two mono channels
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mInputLeft[ i ] = (float)data[ i*2 ];
        mInputRight[ i ] = (float)data[ i*2 + 1 ];
    }

    process();

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < mSamples; i++ ) {
        mOutput32s[ i*2 ] = convertFloatToInt32( mOutputLeft[ i ] );
        mOutput32s[ i*2 + 1 ] = convertFloatToInt32( mOutputRight[ i ] );
    }

    return mOutput32s;
}

void 
Pipeline::process() {
    for( Biquads::iterator i = mBiquads[ 0 ].begin(); i != mBiquads[ 0 ].end(); i++ ) {
        dsps_biquad_f32_aes3( mInputLeft, mOutputLeft, mSamples, (*i)->getCoefficients(), (*i)->getDelayLine() );

        if ( i != mBiquads[ 0 ].end() ) {
            memcpy( mInputLeft, mOutputLeft, sizeof( float ) * mSamples );
        }
    }

    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        dsps_biquad_f32_aes3( mInputRight, mOutputRight, mSamples, (*i)->getCoefficients(), (*i)->getDelayLine() );

        if ( i != mBiquads[ 1 ].end() ) {
            memcpy( mInputRight, mOutputRight, sizeof( float ) * mSamples );
        }
    }

    if ( mHasAttenuation ) {
        memcpy( mInputLeft, mOutputLeft, sizeof( float ) * mSamples );  
        memcpy( mInputRight, mOutputRight, sizeof( float ) * mSamples );

        dsps_mulc_f32_ae32( mInputLeft, mOutputLeft, mSamples, mGainFactor[ 0 ], 1, 1 );
        dsps_mulc_f32_ae32( mInputRight, mOutputRight, mSamples, mGainFactor[ 1 ], 1, 1 );
    }
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
