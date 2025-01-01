#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdio.h>
#include <stdlib.h>

class Message {
public:
    Message() : mMessageType( MSG_NONE ), mParam( 0 ) {}

    enum MessageType {
        MSG_NONE = 0,
        MSG_TIMER = 1,
        MSG_I2C = 2,
        MSG_I2S_RECV = 3
    };

    Message( MessageType t ) : mMessageType( t ), mParam( 0 ) {}
    Message( MessageType t, uint32_t param ) : mMessageType( t ), mParam( param ) {}

    MessageType mMessageType;
    uint32_t mParam;
};

#endif