#!/usr/bin/env python3

# Help: sanitizeStatsLog.py -h

import argparse
import datetime

def parseCommandLineArgs():
    parser = argparse.ArgumentParser(
        description="Sanitize stats log files by replacing UUIDs")
    
    parser.add_argument("statsLog", help="statsLog.csv file path")
    
    return parser.parse_args()

if __name__ == "__main__":

    args = parseCommandLineArgs()

    file = open(args.statsLog, 'r')
    
    uuids = []
    for line in file:
        line = line.strip()
        lineParts = line.split(",")
        uuid = lineParts[0]
        if uuid != "UUID" and uuid not in uuids:
            uuids.append(uuid)
        # replace uuids with index into set array
        for index, value in enumerate(uuids):
            line = line.replace(value, str(index))
        print(line)
    
    # Closing files
    file.close()
