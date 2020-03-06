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

    // Getters
    /**
     * Retrieves channel ID
     * @return Channel ID
     */
    unsigned long long getChannelId() const;
    
    /**
     * @return UUID of node channel is connecting to
     */
    UUID getToNode() const;
    
    /**
     * @return UUID of node channel is connecting from
     */
    UUID getFromNode() const;
};
#endif //ADAK_KEYING_CHANNELS_H