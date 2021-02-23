#!/usr/bin/env python3

#
# Usage: runVisOnly -g graphIsLog (True|False) -s statslog.csv
#
import argparse
import DataVis2 as datavis2

def parseCommandLineArgs():
    parser = argparse.ArgumentParser(
        description="Show Distributed Key Simulation visualization in browser")
    
    parser.add_argument("graphIsLog", help="Graph axis is logrithmic")
    
    parser.add_argument("statsLog", default="statslog.csv",
                        help="statslog.csv file path")
    
    return parser.parse_args()

if __name__ == "__main__":

    args = parseCommandLineArgs()

    if(args.graphIsLog):
        datavis2.runDataAnalyticsLog(args.statsLog)
    else:
        datavis2.runDataAnalytics(args.statsLog)
