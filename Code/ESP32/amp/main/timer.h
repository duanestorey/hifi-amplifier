#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "queue.h"
#include "mutex.h"

#include <stack>
#include <map>
#include <list>

class TimerObserver;

struct TimerInfo {
    uint32_t mEventID;
    uint32_t mEventTime;
    uint32_t mUser;
    uint32_t mTimeout;

    bool mRecurring;
    Queue *mQueue;
};

typedef std::map< uint32_t, TimerInfo > TimerMap;
typedef std::list<TimerInfo> TimerList;

class Timer {
public:
    Timer();

    uint32_t setTimer( uint32_t timeout, Queue &queue, bool recurring = false );
    void cancelTimer( uint32_t timerID );

    void processTick();
protected:
    uint32_t mCurrentEventID;
    TimerMap mTimers;
    TimerList mDispatchList;
    Mutex mMutex;
private:
    uint32_t getCurrentTimeInMS() const;
};

class TimerObserver {
public:
    virtual void onTimer( uint32_t eventID ) = 0;
};

#endif