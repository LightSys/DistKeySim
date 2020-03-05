#include "Channel.h"

Channel::Channel(UUID to, UUID from) {
    static unsigned long long lastChannelId = 0;
    this->toNode = to;
    this->fromNode = from;
    this->channelId = lastChannelId;
    lastChannelId++;
}

unsigned long long Channel::getChannelId() {
    return this->channelId;
}

UUID Channel::getToNode() {
    return this->toNode;
}
UUID Channel::getFromNode() {
    return this->fromNode;
}