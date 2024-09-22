#pragma once

#include <iostream>

#include "Logger.h"

class ACE {

public:

    static string toString(bool b) {
        return b ? "true" : "false";
    }

    // Inspired by Python PEP 0485
    // https://www.python.org/dev/peps/pep-0485/#proposed-implementation
    static bool areCloseEnough(long double left, long double right, long double relativeTolerance=1e-6,
                                                            long double absoluteTolerance=0.0) {
        bool simple = fabs(left-right) < relativeTolerance;
        bool complex = fabs(left-right) <= max(
            relativeTolerance * max(fabs(left), fabs(right)), absoluteTolerance);
        // if (simple != complex) {
        //     Logger::log(Formatter() << "areCloseEnough(" << left << "," << right << ")");
        //     Logger::log(Formatter() << "+-simple=" << toString(fabs(left-right) < relativeTolerance)
        //         << "  complex=" << toString(fabs(left-right) <= max(relativeTolerance * max(fabs(left), fabs(right)), absoluteTolerance)));
        // }

        return simple;
    }

    // Use areCloseEnough to define isLessThan
    static bool isLessThan(long double left, long double right) {
        if (areCloseEnough(left, right)) {
            return false;
        } else {
            return left < right;
        }
    }

    // Use areCloseEnough to define isGreaterThan
    static bool isGreaterThan(long double left, long double right) {
        if (areCloseEnough(left, right)) {
            return false;
        } else {
            return left > right;
        }
    }
};