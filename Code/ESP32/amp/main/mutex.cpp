#include "mutex.h"

Mutex::Mutex() {
    mMutex = xSemaphoreCreateMutex();
}

Mutex::~Mutex() {
    vSemaphoreDelete( mMutex );
}

void 
Mutex::lock() {
    xSemaphoreTake( mMutex, portMAX_DELAY );
}
    
void 
Mutex::unlock() {
    xSemaphoreGive( mMutex );
}