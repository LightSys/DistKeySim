#!/usr/bin/env python3

# Help: showVis -h

import argparse
import DataVis as datavis
import DataVis2 as datavis2

def parseCommandLineArgs():
    parser = argparse.ArgumentParser(
        description="Show Distributed Key Simulation visualization in browser")
    
    parser.add_argument("visNum", help="Visualization (1|2)", type=int)
    parser.add_argument("graphIsLog", help="Graph axis is logarithmic (True|False)")
    parser.add_argument("statsLog", help="statslog.csv file path")
    
    return parser.parse_args()

if __name__ == "__main__":

    args = parseCommandLineArgs()
    print(args.visNum, args.graphIsLog, args.statsLog)

    if args.graphIsLog.upper().startswith('T'):
        if args.visNum == 1:
            print("ERROR: Visualization 1 does not support logarithmic")
        elif args.visNum == 2:
            datavis2.runDataAnalyticsLog(args.statsLog)
    else:
        if args.visNum == 1:
            datavis.runDataAnalytics(args.statsLog)
        elif args.visNum == 2:
            datavis2.runDataAnalytics(args.statsLog)
