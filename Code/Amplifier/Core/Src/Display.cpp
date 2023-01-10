/*
 * Display.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: duane
 */

#include "Display.h"
#include "main.h"
#include "cmsis_os.h"

Display::Display() : mShouldUpdate( true ) {
	// TODO Auto-generated constructor stub

}

Display::~Display() {
	// TODO Auto-generated destructor stub
}

void
Display::update() {
	mShouldUpdate = true;
}

void
Display::run() {
	for(;;) {
		// Check to see if we need to update the display
		if ( mShouldUpdate ) {

			// Clear the update for the next time
			mShouldUpdate = false;
		}

		osDelay( 5 );
	}
}

