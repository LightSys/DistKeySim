#!/usr/bin/env python3

# Help: showVis -h

import argparse
import DataVis as datavis
import DataVis2 as datavis2
import os
import sys

def parseCommandLineArgs():
    parser = argparse.ArgumentParser(
        description="Show Distributed Key Simulation visualization in browser")
    
    parser.add_argument("-v", "--visNum", help="Visualization (1|2)", type=int, default=1)
    parser.add_argument("-l", "--graphIsLog", help="Graph axis is logarithmic (True|False)", default="False")
    parser.add_argument("-f", "--imageFormat", help="Image format (html|plotly write image format)", type=str, default="html")
    parser.add_argument("statsLog", help="statslog.csv file path")
    
    return parser.parse_args()

if __name__ == "__main__":

    args = parseCommandLineArgs()

    if not os.path.exists(args.statsLog):
            print("ERROR: File not found: %s" % (args.statsLog))
            sys.exit()

    if args.graphIsLog.upper().startswith('T'):
        if args.visNum == 1:
            print("ERROR: Visualization 1 does not support logarithmic")
        elif args.visNum == 2:
            datavis2.runDataAnalyticsLog(args.statsLog, args.imageFormat)
        else:
            print("ERROR: visNum must have only values 1 or 2")
    else:
        if args.visNum == 1:
            datavis.runDataAnalytics(args.statsLog, args.imageFormat)
        elif args.visNum == 2:
            datavis2.runDataAnalytics(args.statsLog, args.imageFormat)
        else:
            print("ERROR: visNum must have only values 1 or 2")
