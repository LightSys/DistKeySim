#!/usr/bin/env python3

# --------------------------------------------
# Test Scenario 3 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: Ensure block sharing eventually stabilizes, and determine what resolution
# the keyspace is broken up into (eighths, 16ths, 32nds) by ADAK in an attempt to distribute
# the keyspace according to object creation rate.

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Scenario 3')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    parser.add_argument('--config', '-c', default="config/scenario3-config.json",
        help='Config file name and path')
    args = parser.parse_args()
    days = args.days
    configFileName = args.config

    iterations = simUnitsPerSecond * seconds * minutes * hours * days
    print("Testing Scenario 3 with %s iterations (%g days) ..." % (iterations, days))

    numKeyspaces = runOneSim(configFileName, "full", iterations,  3)
    assert numKeyspaces > 3, "Test Scenario 3 failed: numKeyspaces=%d" % numKeyspaces
    print("Test Scenario 3 passed, numKeyspaces=%d" % numKeyspaces)
