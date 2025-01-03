#include "pipeline.h"
#include <limits>
#include <memory.h>
#include <esp_dsp.h>
#include "debug.h"
#include "dsp-custom.h"

Pipeline::Pipeline( Profile *profile ) : mSamples( 0 ), mOutputFloat( 0 ), mOutput32s( 0 ), mOutput16s( 0 ), mProfile( profile ) {
    mChannels[ CHANNEL_LEFT ] = ChannelInfo( CHANNEL_LEFT );
    mChannels[ CHANNEL_RIGHT ] = ChannelInfo( CHANNEL_LEFT );
}

Pipeline::~Pipeline() {
    destroy();
}

void 
Pipeline::resetAll() {
    for ( uint8_t channel = CHANNEL_LEFT; channel <= CHANNEL_RIGHT; channel++ ) {
        for( Biquads::iterator i = mChannels[ channel ].mBiquads.begin(); i != mChannels[ channel ].mBiquads.end(); i++ ) {
            Biquad *biquad = (*i);
            delete biquad;
        }   
    }

    mChannels[ CHANNEL_LEFT ] = ChannelInfo( CHANNEL_LEFT );
    mChannels[ CHANNEL_RIGHT ] = ChannelInfo( CHANNEL_LEFT );

    deallocate();
}

void 
Pipeline::setAttenuation( uint8_t which, float level ) {
    if ( which <= CHANNEL_RIGHT ) {
        mChannels[ which ].mAttenuation = level;  
        mChannels[ which ].mGainFactor = 1.0f/pow( 10, level/20.0f );
        mChannels[ which ].mHasAttenuation = true;  
    }
}

void 
Pipeline::setAttenuationLeft( float level ) {
    setAttenuation( CHANNEL_LEFT, level );
}

void 
Pipeline::setAttenuationRight( float level ) {
    setAttenuation( CHANNEL_RIGHT, level );
}

void 
Pipeline::checkAllocate( uint32_t samples ) {
    if ( samples == mSamples ) {
        return;
    }

    if ( samples > mSamples ) {
        deallocate();

        mSamples = samples;
        AMP_DEBUG_I( "Allocating audio buffers" );
    }

    if ( !mOutputFloat ) {
        mOutputFloat = (float *)aligned_alloc( DSP_DATA_ALIGN, sizeof( float ) * 2 * mSamples );
    }

    if ( !mOutput32s ) {
        mOutput32s = (int32_t *)aligned_alloc( DSP_DATA_ALIGN, sizeof( int32_t ) * 2 * mSamples );
    }

    if ( !mOutput16s ) {
        mOutput16s = (int16_t *)aligned_alloc( DSP_DATA_ALIGN, sizeof( int16_t ) * 2 * mSamples );
    }
}

void 
Pipeline::setSource( uint8_t which, uint8_t source ) {
    if ( which <= CHANNEL_RIGHT ) { 
        mChannels[ which ].mSource = source;
    } 
}

void 
Pipeline::setLeftSource( uint8_t source ) { 
    setSource( CHANNEL_LEFT, source );
}

void 
Pipeline::setRightSource( uint8_t source ) { 
    setSource( CHANNEL_RIGHT, source );
}

float 
Pipeline::convertInt32ToFloat( int32_t i ) {
    float result;
	asm("FLOAT.S %0, %1, 0\t\n"
		: "=f" (result)
		: "a" (i));

    return result;
}

float 
Pipeline::convertInt32ToFloatHalf( int32_t i ) {
    float result;
	asm("FLOAT.S %0, %1, 1\t\n"
		: "=f" (result)
		: "a" (i));

    return result;
}

int32_t 
Pipeline::convertFloatToInt32( float f ) {
    int result;
    asm("ROUND.S %0, %1, 0\t\n"
        : "=a" (result)
        : "f" (f));

    return result;  
}

int16_t 
Pipeline::convertFloatToInt16( float f ) {
    int result;
    asm("ROUND.S %0, %1, 0\t\n"
        : "=a" (result)
        : "f" (f));

    return result;  
}

void 
Pipeline::buildChannelsInt16( int16_t *data, uint32_t samples ) {
    for ( uint8_t channel = CHANNEL_LEFT; channel <= CHANNEL_RIGHT; channel++ ) {
        if ( mChannels[ channel ].mSource == SOURCE_COMBINED ) { 
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = convertInt32ToFloat( data[ 2*i ] ) + convertInt32ToFloat( data[ 2*i + 1 ] );

                mOutputFloat[ 2*i + channel ] = combined;
            }      
        } else if ( mChannels[ channel ].mSource == SOURCE_COMBINED_6DB ) {
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = convertInt32ToFloat( data[ 2*i ] ) + convertInt32ToFloat( data[ 2*i + 1 ] );

                mOutputFloat[ 2*i + channel ] = combined; 
            }   

            dsps_mulc_f32_ae32( mOutputFloat + channel, mOutputFloat + channel, samples, 0.5, 2, 2 );    
        } else {
            for ( uint32_t i = 0; i < samples; i++ ) {
                mOutputFloat[ 2*i + channel ] = convertInt32ToFloat( data[ 2*i + mChannels[ channel ].mSource ] ); 
            }   
        }   
    }
}

void 
Pipeline::buildChannelsInt32( int32_t *data, uint32_t samples ) {
    for ( uint8_t channel = CHANNEL_LEFT; channel <= CHANNEL_RIGHT; channel++ ) {
        if ( mChannels[ channel ].mSource == SOURCE_COMBINED ) { 
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = convertInt32ToFloat( data[ 2*i ] ) + convertInt32ToFloat( data[ 2*i + 1 ] );

                mOutputFloat[ 2*i + channel ] = combined;
            }      
        } else if ( mChannels[ channel ].mSource == SOURCE_COMBINED_6DB ) {
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = convertInt32ToFloat( data[ 2*i ] ) + convertInt32ToFloat( data[ 2*i + 1 ] );

                mOutputFloat[ 2*i + channel ] = combined; 
            }   

            dsps_mulc_f32_ae32( mOutputFloat + channel, mOutputFloat + channel, samples, 0.5, 2, 2 );    
        } else {
            for ( uint32_t i = 0; i < samples; i++ ) {
                mOutputFloat[ 2*i + channel ] = convertInt32ToFloat( data[ 2*i + mChannels[ channel ].mSource ] ); 
            }   
        }   
    }
}

void 
Pipeline::buildChannelsFloat( float *data, uint32_t samples ) {
    for ( uint8_t channel = CHANNEL_LEFT; channel <= CHANNEL_RIGHT; channel++ ) {
        if ( mChannels[ channel ].mSource == SOURCE_COMBINED ) { 
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = data[ 2*i ] + data[ 2*i + 1 ];

                mOutputFloat[ 2*i + channel ] = combined;
            }      
        } else if ( mChannels[ channel ].mSource == SOURCE_COMBINED_6DB ) {
            for ( uint32_t i = 0; i < samples; i++ ) {
                float combined = data[ 2*i ] + data[ 2*i + 1 ];

                mOutputFloat[ 2*i + channel ] = combined; 
            }   

            dsps_mulc_f32_ae32( mOutputFloat + channel, mOutputFloat + channel, samples, 0.5, 2, 2 );    
        } else {
            for ( uint32_t i = 0; i < samples; i++ ) {
                mOutputFloat[ 2*i + channel ] = data[ 2*i + mChannels[ channel ].mSource ]; 
            }   
        }   
    }
}

int16_t *
Pipeline::process( int16_t *data, uint32_t samples ) {
    mProfile->startProfiling( "allocate" );

    checkAllocate( samples );

   // AMP_DEBUG_I( "Allocation done" );

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    buildChannelsInt16( data, samples );

    mProfile->stopProfiling( "deinterleave" );

    //AMP_DEBUG_I( "Processing" );

    processInternal( samples );

    mProfile->startProfiling( "output" );

    //AMP_DEBUG_I( "Converting back" );

    // convert two mono channels into one stereo channel for output
    for ( uint32_t i = 0; i < samples; i++ ) {
        mOutput16s[ 2*i ] = convertFloatToInt16( mOutputFloat[ 2*i ] );
        mOutput16s[ 2*i+1 ] = convertFloatToInt16( mOutputFloat[ 2*i+1 ] );
    }

    mProfile->stopProfiling( "output" );

    return mOutput16s;
}


int32_t * 
Pipeline::process( int32_t *data, uint32_t samples ) {
    mProfile->startProfiling( "allocate" );

    checkAllocate( samples );

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    buildChannelsInt32( data, samples );

    mProfile->stopProfiling( "deinterleave" );

    processInternal( samples );

    mProfile->startProfiling( "output" );

    for ( uint32_t i = 0; i < samples; i++ ) {
        mOutput32s[ 2*i ] = convertFloatToInt32( mOutputFloat[ 2*i ] );
        mOutput32s[ 2*i + 1 ] = convertFloatToInt32( mOutputFloat[ 2*i+1 ] );
    }

    mProfile->stopProfiling( "output" );

    return mOutput32s;
}

float * 
Pipeline::process( float *data, uint32_t samples ) {
    mProfile->startProfiling( "allocate" );

    checkAllocate( samples );

    mProfile->stopProfiling( "allocate" );

    mProfile->startProfiling( "deinterleave" );

    buildChannelsFloat( data, samples );

    mProfile->stopProfiling( "deinterleave" );

    processInternal( samples );

    return mOutputFloat;
}

void 
Pipeline::dsps_biquad_f32_skip_transposed2( const float *input, float *output, uint32_t samples, float *coeff, float *delay ) {
    for ( unsigned int i = 0; i < samples; i++ ) {
        float in = input[2*i];
      //  float out = output[2*i];
        float out = delay[0]+coeff[0]*in;
        delay[0] = delay[1]+coeff[1]*in-coeff[3]*out;
        delay[1] = coeff[2]*in-coeff[4]*out;
        output[2*i] = out;
    }
}
/*
// f0 - b0
// f1 - b1
// f2 - b2
// f3 - a1
// f4 - a2
*/

void 
Pipeline::processInternal( uint32_t samples ) {
    for( Biquads::iterator i = mChannels[ CHANNEL_LEFT ].mBiquads.begin(); i != mChannels[ CHANNEL_LEFT ].mBiquads.end(); i++ ) {
       // dsps_biquad_f32_aes3( mInputLeft, mOutputLeft, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        mProfile->startProfiling( "biquadleft" );

        dsps_biquad_f32_skip_dft2_aes3( mOutputFloat, mOutputFloat, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
       // dsps_biquad_f32_skip_aes3( mOutputFloat, mOutputFloat, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
      
        mProfile->stopProfiling( "biquadleft" );
    }

    for( Biquads::iterator i = mChannels[ CHANNEL_RIGHT ].mBiquads.begin(); i != mChannels[ CHANNEL_RIGHT ].mBiquads.end(); i++ ) {
        mProfile->startProfiling( "biquadright" );
       // dsps_biquad_f32_aes3( mInputRight, mOutputRight, samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        dsps_biquad_f32_skip_dft2_aes3( &mOutputFloat[1], &mOutputFloat[1], samples, (*i)->getCoefficients(), (*i)->getDelayLine() );
        //dsps_biquad_f32_skip_aes3( &mOutputFloat[1], &mOutputFloat[1], samples, (*i)->getCoefficients(), (*i)->getDelayLine() );

        mProfile->stopProfiling( "biquadright" );
    }

    mProfile->startProfiling( "attenuate" );

    if ( mChannels[ CHANNEL_LEFT ].mHasAttenuation ) {
        dsps_mulc_f32_ae32( mOutputFloat, mOutputFloat, samples, mChannels[ CHANNEL_LEFT ].mGainFactor, 2, 2 );
    }

    if ( mChannels[ CHANNEL_RIGHT ].mHasAttenuation ) {
        dsps_mulc_f32_ae32( &mOutputFloat[1], &mOutputFloat[1], samples, mChannels[ CHANNEL_RIGHT ].mGainFactor, 2, 2 );
    }

    mProfile->stopProfiling( "attenuate" );
}

void 
Pipeline::deallocate() {
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

    mSamples = 0;
}

void 
Pipeline::destroy() {
    deallocate();
}

void 
Pipeline::generate( uint32_t samplingRate ) {
    for ( uint8_t channels = CHANNEL_LEFT; channels <= CHANNEL_RIGHT; channels++ ) {
        for( Biquads::iterator i = mChannels[ channels ].mBiquads.begin(); i != mChannels[ channels ].mBiquads.end(); i++ ) {
            (*i)->generate( samplingRate );
        }
    }
}

void 
Pipeline::addBiquad( uint8_t which, Biquad *biquad ) {
    if ( which <= CHANNEL_RIGHT ) {
        mChannels[ which ].mBiquads.push_back( biquad );
    }
}

void 
Pipeline::addBiquadLeft( Biquad *biquad ) {
    addBiquad( CHANNEL_LEFT, biquad );
}

void 
Pipeline::addBiquadRight( Biquad *biquad ) {
    addBiquad( CHANNEL_RIGHT, biquad );
}

void 
Pipeline::resetDelayLines() {
    for ( uint8_t channels = CHANNEL_LEFT; channels <= CHANNEL_RIGHT; channels++ ) {
        for( Biquads::iterator i = mChannels[ channels ].mBiquads.begin(); i != mChannels[ channels ].mBiquads.end(); i++ ) {
            (*i)->resetDelayLine();
        }
    }
}
