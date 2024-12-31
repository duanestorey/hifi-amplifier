#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class Mutex {
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
protected:
    SemaphoreHandle_t mMutex;
};

class ScopedLock {
public:
    ScopedLock( Mutex &mutex ) : mMutex( mutex ) { mMutex.lock(); }
    ~ScopedLock() { mMutex.unlock(); }
protected:
    Mutex &mMutex;
};

#endif