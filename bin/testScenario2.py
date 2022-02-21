#!/usr/bin/env python3

# --------------------------------------------
# Test Scenario 2 (see "ADAK Scenarios 1.pdf")
# --------------------------------------------
#
# Objective: There initially should be a lot of subblock sharing to fill in “holes”,
# but eventually block sharing should happen, reducing the volume of subblock sharing.
# Block sharing should eventually result in the higher-rate node having 7x the keyspace
# as the lower rate node. (these values were chosen so that the keyspace blocks will
# end up being divided up into eighths, with one node having 1/8 of the keyspace and
# the other having 7/8 of the keyspace)

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Scenario 2')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    parser.add_argument('--config', '-c', default="config/scenario2-config.json",
        help='Config file name and path')
    args = parser.parse_args()
    days = args.days
    configFileName = args.config

    iterations = simUnitsPerSecond * seconds * minutes * hours * days
    print("Testing Scenario 2 with %s iterations (%g days) ..." % (iterations, days))

    numKeyspaces = runOneSim(configFileName, "full", iterations,  2)
    assert numKeyspaces > 2, "Test Scenario 2 failed: numKeyspaces=%d" % numKeyspaces
    print("Test Scenario 2 passed")
