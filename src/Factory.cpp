#include <iostream>

#include "AbstractStrategy.h"
#include "config.hpp"
#include "ControlStrategy.h"
#include "DoNothingStrategy.h"
#include "Factory.h"

AbstractStrategy* Factory::makeAdakStrategy(const struct Config& config) {
    switch (config.adakStrategy) {
        case Config::ADAKStrategy::Control:
            return new ControlStrategy();
        case Config::ADAKStrategy::DoNothing:
            return new DoNothingStrategy();
    }
}
