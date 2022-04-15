#!/usr/bin/env python3

# -----------------------------------------------------------------
# Test Simple Strategy with Scenario 2 (see "ADAK Scenarios 1.pdf")
# -----------------------------------------------------------------
#
# Objective: 

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Simple Strategy')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    parser.add_argument('--config', '-c', default="config/SimpleStrategy-config.json", help='Config file name and path')
    args = parser.parse_args()
    days = args.days
    configFileName = args.config

    iterations = simUnitsPerSecond * seconds * minutes * hours * days
    print("Testing Simple Strategy with %s iterations (%g days) ..." % (iterations, days))

    numKeyspaces = runOneSim(configFileName, "full", iterations,  2)
    assert numKeyspaces > 2, "Test Simple Strategy failed: numKeyspaces=%d" % numKeyspaces
    print("Test Simple Strategy passed")
