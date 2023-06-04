#include "Damping.h"
#include "Logger.h"

void Damping::setDampingFactor(int f) {
    if (verbose) Logger::log(Formatter() << "Damping is setting Damping Factor to " << f);
    factor = f;
    assert(factor == f);
}

void Damping::addValue(double value) {
    if (verbose) Logger::log(Formatter() << "Damping adds the value=" << value << " to existing list=" << (values.size() == 0 ? "empty" : toString()));
    values.push_back(value);
    assert(values.back() == value);
    while (values.size() > factor) {
        if (verbose) Logger::log(Formatter() << "+-Damping is deleting value=" << values.front());
        values.pop_front();
    }
    if (verbose) Logger::log(Formatter() << "+-New Damping list is size=" << values.size() << " list=" << toString());
}

void Damping::setValue(double value) {
    if (verbose) Logger::log(Formatter() << "Damping sets all values to " << value);
    for (int i = 0; i < values.size(); i++) {
        values[i] = value;
    }
    if (verbose) Logger::log(Formatter() << "+-New Damping list is size=" << values.size() << " list=" << toString());
}

double Damping::getValue() const {
    if (verbose) Logger::log(Formatter() << "Damping is getting a value from the list " << toString());
    double sum = 0;
    for (int i = 0; i < values.size(); i++) {
        sum = values[i];
    }
    auto value = sum / values.size();
    if (verbose) Logger::log(Formatter() << "+-Damping computes next value:" << value << " = " << sum << " / " << values.size());
    if (verbose) Logger::log(Formatter() << "+-Damping returns value=" << value << " of list size=" << values.size() << " values=" << toString());
    return value;
}

std::string Damping::toString() const {
    ostringstream stream;
    for (int i = 0; i < values.size(); i++) {
        stream << values[i];
        if (i < values.size()-1) {
            stream << ", ";
        }
    }
    return stream.str();
}
