#include <iostream>

#include "AbstractStrategy.h"
#include "config.hpp"
#include "ControlStrategy.h"
#include "DoNothingStrategy.h"
#include "Factory.h"
#include "SimpleStrategy.h"

AbstractStrategy* Factory::makeAdakStrategy(const struct Config& config) {
    switch (config.adakStrategy) {
        case Config::ADAKStrategy::DoNothing:
            return new DoNothingStrategy();
        case Config::ADAKStrategy::Simple:
            return new SimpleStrategy();
        case Config::ADAKStrategy::Control:
            return new ControlStrategy();
    }
}
