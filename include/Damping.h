#pragma once

#include <deque>

class Damping {
public:
     explicit Damping() { }
    ~Damping() = default;

    void setDampingFactor(int f);
    void addValue(double value);
    void setValue(double value);
    double getValue() const;
    std::string toString() const;

private:
    int factor = 1;
    std::deque<double> values;
    bool verbose = true;
};