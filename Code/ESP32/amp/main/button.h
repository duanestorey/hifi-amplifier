#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "queue.h"

#define BUTTON_DEBOUNCE_TIME    25

class Button {
public:
    Button( uint8_t gpio, Queue *queue, uint16_t debounceMs = BUTTON_DEBOUNCE_TIME );
    void handleInterrupt();
    void tick();
protected:
    uint8_t mGpio;
    uint16_t mDebounceTime;

    Queue *mQueue;

    bool mThinking;
    uint32_t mThinkingStartTime;
    bool mPressed;
private:
    void startThinking();
    void stopThinking();
    void pressed();
    void unpressed();
};

#endif