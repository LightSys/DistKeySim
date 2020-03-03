//
// Created by tomlo on 3/3/2020.
//

#ifndef ADAK_KEYING_CHANNELS_H
#define ADAK_KEYING_CHANNELS_H

#include "UUID.h"

class Channel {

private:
    unsigned long long channelId;
    UUID toNode;
    UUID fromNode;

public:
    Channel(UUID to, UUID from);

    //Getters
    unsigned long long getChannelId();
    UUID getToNode();
    UUID getFromNode();


};


#endif //ADAK_KEYING_CHANNELS_H
