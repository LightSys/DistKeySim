#!/usr/bin/env python3

# --------------------------------------------
# Test Scenario 1 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure the functionality of sharing keyspace blocks and
# the stability of subblock sharing (there should be minimal,
# if any, subblock sharing in this scenario)

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Scenario 1')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    args = parser.parse_args()
    days = args.days

    iterations = simUnitsPerSecond * seconds * minutes * hours * days
    print("Testing Scenario 1 with %s iterations (%g days) ..." % (iterations, days))

    configFileName = "config/scenario1-config.json"
    numKeyspaces = runOneSim(configFileName, "full", iterations,  2)
    assert(numKeyspaces == 2)
    print("Test Scenario 1 passed")
