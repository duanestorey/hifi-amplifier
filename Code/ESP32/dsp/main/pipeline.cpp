#include "pipeline.h"
#include <limits>
#include <esp_dsp.h>

Pipeline::Pipeline( uint32_t samples ) : mSamples( samples ), mInputLeft( 0 ), mInputRight( 0 ), mOutputLeft( 0 ), mOutputRight( 0 ), mOutputFloat( 0 ), mOutput32s( 0 ) {
    allocate( samples );
}

Pipeline::~Pipeline() {
    deallocate();
}

void 
Pipeline::allocate( uint32_t samples ) {
    mInputLeft = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * samples );
    mInputRight = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * samples );
    mOutputLeft = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * samples );
    mOutputRight = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * samples );
    mOutputFloat = (float *)aligned_alloc( sizeof( float ), sizeof( float ) * 2 * samples );
    mOutput32s = (int32_t *)aligned_alloc( sizeof( int32_t ), sizeof( int32_t ) * 2 * samples );
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

float * 
Pipeline::process( float *data ) {
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
    }

    for( Biquads::iterator i = mBiquads[ 1 ].begin(); i != mBiquads[ 1 ].end(); i++ ) {
        dsps_biquad_f32_aes3( mInputRight, mOutputRight, mSamples, (*i)->getCoefficients(), (*i)->getDelayLine() );
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
