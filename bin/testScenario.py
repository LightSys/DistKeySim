#!/usr/bin/env python3

import argparse
from runOneSim import runOneSim

if __name__ == '__main__':

    # 40 milliseconds is the length of a simulation "tick".
    # Therefore, there are 25 simulation ticks per second.

    simUnitsPerSecond = 25
    seconds           = 60
    minutes           = 60
    hours             = 24

    parser = argparse.ArgumentParser(description='Test Scenario')
    parser.add_argument('--scenarioNum', '-s', type=int, help='Scenario number')
    parser.add_argument('--days', '-d', type=float, default=0.1, help='Number of simulated days')
    parser.add_argument('--config', '-c', help='Config file name and path')
    parser.add_argument('--assertion', '-a', help='Python assertion code')
    args = parser.parse_args()

    iterations = simUnitsPerSecond * seconds * minutes * hours * args.days
    print("Testing Scenario %d with %s iterations (%g days) ..." % (args.scenarioNum, iterations, args.days))

    numKeyspaces = runOneSim(args.config, "full", iterations)
    exec(args.assertion)
    print("Test Scenario %d passed, numKeyspaces=%d" % (args.scenarioNum, numKeyspaces))
