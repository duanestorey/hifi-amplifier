#include "queue.h"

Queue::Queue(  uint8_t queueSize ) {
    mQueueHandle = xQueueCreate( queueSize, sizeof( Message ) );
}

Queue::~Queue() {
    vQueueDelete( mQueueHandle );
}

void 
Queue::add( Message msg ) {
    xQueueSendToBack( mQueueHandle, (void *)&msg, 10 );
}

void 
Queue::addFromISR( Message msg ) {
    BaseType_t taskWoken;
    xQueueSendToBackFromISR( mQueueHandle, (void *)&msg, &taskWoken );
}

bool 
Queue::hasMessage() {
    return ( uxQueueMessagesWaiting( mQueueHandle ) > 0 );
}

bool 
Queue::waitForMessage( Message &msg, uint32_t maxTimeInMS ) {
    return ( xQueueReceive( mQueueHandle, (void *)&msg, maxTimeInMS / portTICK_PERIOD_MS ) == pdTRUE );
}

Message 
Queue::getMessage() {
    Message msg;

    xQueueReceive( mQueueHandle, (void *)&msg, 10 );

    return msg;
}