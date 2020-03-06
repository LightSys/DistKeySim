#ifndef ADAK_KEYING_CHANNELS_H
#define ADAK_KEYING_CHANNELS_H

#include "UUID.h"

class Channel {

private:
    /**
     * Each channel has a channelId that is auto incremented each creation of a new channel
     */
    unsigned long long channelId;

    /**
     * The UUIDs of the connected Nodes
     */
    UUID toNode;
    UUID fromNode;

public:
    Channel(UUID to, UUID from);

    //Getters
    unsigned long long getChannelId() const;
    UUID getToNode() const;
    UUID getFromNode() const;
};
#endif //ADAK_KEYING_CHANNELS_H