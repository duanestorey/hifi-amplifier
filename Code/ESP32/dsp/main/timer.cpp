#include "timer.h"
#include "debug.h"

#include <list>

Timer::Timer() : mCurrentEventID( 1 ) {
}

uint32_t
Timer::setTimer( uint32_t timeout, Queue &queue, bool recurring ) {
    TimerInfo timerInfo;
    uint32_t futureTime = getCurrentTimeInMS() + timeout;
    
    ScopedLock lock( mMutex );

    timerInfo.mEventID = mCurrentEventID;
    timerInfo.mQueue = &queue;
    timerInfo.mEventTime = futureTime;
    timerInfo.mTimeout = timeout;
    timerInfo.mRecurring = recurring;
  
    mTimers[ timerInfo.mEventID ] = timerInfo;

    mCurrentEventID++;

    AMP_DEBUG_I( "Setting new time with timeout %lu and ID %lu, futureTime %lu, currentTime %lu", timeout, timerInfo.mEventID , futureTime, getCurrentTimeInMS() );

    return timerInfo.mEventID;
}

 void 
 Timer::cancelTimer( uint32_t timerID ) {
    AMP_DEBUG_I( "Timer Cancelled" );
    ScopedLock lock( mMutex );

    TimerMap::iterator i = mTimers.find( timerID );
    if ( i != mTimers.end() ) {
        mTimers.erase( i );
    }
 }

uint32_t 
Timer::getCurrentTimeInMS() const {
    return ((uint32_t)( xTaskGetTickCount() * portTICK_PERIOD_MS ));
}

void
Timer::processTick() {  
    // Hold the lock while we assemble a list of all the dispatches
    {   
        ScopedLock lock( mMutex );

      //  AMP_DEBUG_SI( "Processing timer tick " << mTimers.size() << " " << toDispatch.size() );

        for ( TimerMap::iterator i = mTimers.begin(); i != mTimers.end(); ) {
            TimerInfo &timerInfo = i->second;

            uint32_t currentTime = getCurrentTimeInMS();

            if ( currentTime >= timerInfo.mEventTime ) {
                mDispatchList.push_back( timerInfo );

                if ( !timerInfo.mRecurring ) {
                    TimerMap::iterator toErase = i;
                    i++;
                    mTimers.erase( toErase );
                } else {
                    timerInfo.mEventTime += timerInfo.mTimeout;
                    ++i;
                }
            } else {
                ++i;
            }
        }
    }

    // Lock is released, so can call setTimer from other task
    for ( TimerList::iterator i = mDispatchList.begin(); i != mDispatchList.end(); i++ ) {
        TimerInfo &timerInfo = *i;

        timerInfo.mQueue->add( Message::MSG_TIMER, timerInfo.mEventID );
    }

    mDispatchList.clear();
}