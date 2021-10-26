#!/usr/bin/env python3

# Assert the absence of oscillation, i.e., number of keyspaces
# does not grow with number of iterations.

import statistics
from runOneSim import runOneSim

if __name__ == '__main__':

    cv = lambda x: statistics.stdev(x) / statistics.mean(x)

    configFileName = "config/oscillation-config.json"
    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",     500,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000,  2))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "single",    50, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",   500, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",  5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "single", 50000, 10))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",     500, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000, 10))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)
