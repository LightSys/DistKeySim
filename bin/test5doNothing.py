#!/usr/bin/env python3

# --------------------------------------------
# Objective: Test Do Nothing ADAK Strategy
# --------------------------------------------

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Do Nothing ADAK Strategy')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    parser.add_argument('--config', '-c', default="config/doNothing-config.json", help='Config file name and path')
    args = parser.parse_args()
    days = args.days
    configFileName = args.config
    print(configFileName)

    iterations = simUnitsPerSecond * seconds * minutes * hours * days
    print("Testing Scenario 1 with %s iterations (%g days) ..." % (iterations, days))

    numKeyspaces = runOneSim(configFileName, "full", iterations,  2)
    assert numKeyspaces == 1, "Test Do Nothing ADAK Strategy failed: numKeyspaces=%d" % numKeyspaces
    print("Test Do Nothing ADAK Strategy passed")
