/*
 * Audio.cpp
 *
 *  Created on: Jan 11, 2023
 *      Author: duane
 */

#include "Audio.h"
#include "main.h"

Audio::Audio( Amplifier *amp ) : Runnable( amp ), mDecoder( 0 ), mDAC( 0 ), mHasBeenInitialized( false ), mTick( 0 ) {
	// TODO Auto-generated constructor stub

}

Audio::~Audio() {
	// TODO Auto-generated destructor stub
}

void
Audio::preTick() {
	if ( !mHasBeenInitialized ) {
		// Let's run the audio init.. we'll put this in the for loop in case for some reason we need to initialize again
		if ( mDAC && mDecoder ) {
			// We have a valid DAC and a valid decoder.. for now both are required

			// Initialize the Dolby Decoder
			mDecoder->initialize();

			if ( mDecoder->isInitialized() ) {
				// This means the startup of the decoder was successful

				// Let's mute the output on the decoder
				//mDecoder->mute( true );

				// Decoder should be sending a clock signal to the DAC chip, so it should be responsive
				// The datasheet says the DAC needs about 5ms to be responsive, so let's wait 10
				HAL_Delay( 10 );

				//mDAC->init();

				HAL_Delay( 10 );

				// Time to unleash the KRAKEN!  Let's start decoding...

				// Now let's pull the Decoder out of the IDLE state

				HAL_Delay( 50 );

				mDecoder->play();

				HAL_Delay( 50 );

				mDecoder->mute( false );

				mDecoder->run();

				mHasBeenInitialized = true;

				mDAC->enable( true );
			}
		}
	}
}

void
Audio::tick() {
	// We need to start setting up the audio interfaces
	// First we'll configure the Dolby Decoder
	for(;;) {
		if ( mDecoder && mHasBeenInitialized ) {
			mDecoder->checkForInterrupt();

			if ( mTick % 5000 == 0 ) {
				mDecoder->checkFormat();
			}
		}

		mTick = mTick + 1;

		HAL_Delay(1);
	}
}

void
Audio::start() {
//	mHasBeenInitialized = true;
}

void
Audio::checkFormat() {

}



