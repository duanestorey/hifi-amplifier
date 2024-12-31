#include "dsp.h"
#include "debug.h"


DSP::DSP() {

}

DSP::~DSP() {

}

void 
startAudioThread( void *param ) {
    // Proxy thread start to main amplifier thread
    ((DSP *)param)->handleAudioThread();
}

void
startGeneralThread( void *param ) {
    // Proxy thread start to main radio thread
    ((DSP *)param)->handleGeneralThread();
}

void
DSP::start() {
    xTaskCreate(
        startAudioThread,
        "Audio Thread",
        10000,
        (void *)this,
        2,
        NULL
    );   

    xTaskCreate(
        startGeneralThread,
        "General Thread",
        10000,
        (void *)this,
        1,
        NULL
    );

}

void 
DSP::handleAudioThread() {
    AMP_DEBUG_I( "Starting audio thread" );
    Message msg;
    while ( mAudioQueue.waitForMessage( msg, 5 ) ) {
    }
}


void
DSP::handleGeneralThread() {
    AMP_DEBUG_I( "Starting general thread" );
    Message msg;
    while ( mGeneralQueue.waitForMessage( msg, 5 ) ) {
    }
}