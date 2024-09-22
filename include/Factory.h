#pragma once

class AbstractStrategy;

class Factory {

public:

    static AbstractStrategy* makeAdakStrategy(const struct Config& config);

};
