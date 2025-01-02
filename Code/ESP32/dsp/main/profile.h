#ifndef __PROFILE_H__
#define __PROFILE_H__

#include "config.h"
#include <string>
#include <map>

struct ProfileInfo {
    ProfileInfo() : mStartCycles( 0 ), mStopCycles( 0 ), mTotalCycles( 0 ) {}

    uint64_t mStartCycles;
    uint64_t mStopCycles;
    uint64_t mTotalCycles;
};

typedef std::map<std::string, ProfileInfo> ProfileInfoMap;

class Profile {
    public:
        Profile();

        void tick();
        void reset();

        void startProfiling( const char *profileName );
        void stopProfiling( const char *profileName );

        uint64_t getTotalCyles();

        void dump();
    protected:
        uint64_t mStartTime;
        ProfileInfoMap mProfiles;
};

#endif