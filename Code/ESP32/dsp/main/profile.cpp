#include "profile.h"
#include "esp_timer.h"
#include "esp_app_desc.h"
#include "esp_system.h"
#include "esp_cpu.h"
#include "debug.h"

Profile::Profile() : mStartTime( 0 ) {

}

void 
Profile::tick() {

}

void
Profile::reset() {
 //   esp_cpu_set_cycle_count( 0 );
    mStartTime = esp_timer_get_time();
    mProfiles.clear();
}

uint64_t 
Profile::getTotalCyles() {
    uint32_t cycles = 0;

    for ( ProfileInfoMap::iterator i = mProfiles.begin(); i != mProfiles.end(); i++ ) {
        cycles = cycles + (*i).second.mTotalCycles;
    }  

    return cycles;
}

void 
Profile::startProfiling( const char *profileName ) {
    ProfileInfoMap::iterator i = mProfiles.find( profileName );
    if ( i != mProfiles.end() ) {
        i->second.mStartCycles = esp_timer_get_time();
    } else {
        ProfileInfo info;
        mProfiles[ profileName ] = info;

        ProfileInfoMap::iterator i = mProfiles.find( profileName );
        i->second.mStartCycles = esp_timer_get_time();
    }
}

void 
Profile::stopProfiling( const char *profileName ) {
    uint32_t stopTime = esp_timer_get_time();
    ProfileInfoMap::iterator i = mProfiles.find( profileName );
    if ( i != mProfiles.end() ) {
        (*i).second.mStopCycles = stopTime;
        (*i).second.mTotalCycles = (*i).second.mTotalCycles + ( (*i).second.mStopCycles - (*i).second.mStartCycles );
    }
}

void
Profile::dump() {
    for ( ProfileInfoMap::iterator i = mProfiles.begin(); i != mProfiles.end(); i++ ) {
        AMP_DEBUG_I( "Profile info for [%s] used [%lu] microseconds", i->first.c_str(), (uint32_t)i->second.mTotalCycles );
    }  
}