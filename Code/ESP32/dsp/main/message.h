#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdio.h>
#include <stdlib.h>

class Message {
public:
    Message() : mMessageType( MSG_NONE ), mParam( 0 ) {}

    enum MessageType {
        MSG_NONE = 0,
        MSG_TIMER,
        MSG_I2C,
        MSG_I2S_RECV,
        MSG_AUDIO_START,
        MSG_AUDIO_STOP,
        MSG_AUDIO_ADD_FILTER_LEFT,
        MSG_AUDIO_ADD_FILTER_RIGHT,
        MSG_AUDIO_FULL_RESET
    };

    Message( MessageType t ) : mMessageType( t ), mParam( 0 ) {}
    Message( MessageType t, uint32_t param ) : mMessageType( t ), mParam( param ) {}

    MessageType mMessageType;
    uint32_t mParam;
};

#endif