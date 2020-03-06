#include "Channel.h"

Channel::Channel(UUID to, UUID from) {
    static unsigned long long lastChannelId = 0;
    this->toNode = to;
    this->fromNode = from;
    this->channelId = lastChannelId;
    lastChannelId++;
}

unsigned long long Channel::getChannelId() const {
    return this->channelId;
}

UUID Channel::getToNode() const {
    return this->toNode;
}
UUID Channel::getFromNode() const {
    return this->fromNode;
}