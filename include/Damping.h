#pragma once

#include <queue>

class Damping {
public:
    void setDampingFactor(int f) {
        factor = f;
    }

    void addValue(double value) {
        currentValue = value;
    }

    double getValue() const { return currentValue; }

private:
    int factor;
    std::queue<double> values;
    double currentValue;
};