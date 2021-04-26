#!/usr/bin/env python3

# Help: sanitizeLogOutput.py -h

import argparse
import datetime

def parseCommandLineArgs():
    parser = argparse.ArgumentParser(
        description="Sanitize log output files by removing date and replacing UUIDs")
    
    today = datetime.datetime.today()
    defaultDayOfWeek = today.strftime("%a")
    parser.add_argument("-d", "--dayOfWeek", default=defaultDayOfWeek,
        help="Day of week used for removing the date on each line; default={}"
            .format(defaultDayOfWeek))
    parser.add_argument("logOutput", help="logOutput.txt file path")
    
    return parser.parse_args()

if __name__ == "__main__":

    args = parseCommandLineArgs()
    splitOn = " -- {} ".format(args.dayOfWeek)

    file = open(args.logOutput, 'r')
    
    uuids = []
    foundChannels = False
    foundToFrom = False
    for line in file:
        lineParts = line.strip().split(splitOn)
        log = lineParts[0]
        if "Root UUID:" in log:
            uuidParts = log.split(": ")
            uuid = uuidParts[1]
            if uuid not in uuids:
                uuids.append(uuid)
        foundChannels = foundChannels or "CHANNELS" in log
        foundToFrom = foundToFrom or "TO,FROM,ID" in log
        if foundChannels and foundToFrom and "TO,FROM,ID" not in log:
            uuidParts = log.split(",")
            for uuid in uuidParts[0:1]:
                if uuid not in uuids:
                    uuids.append(uuid)
            foundChannels = False
            foundToFrom = False
        # replace uuids with index into set array
        for index, value in enumerate(uuids):
            log = log.replace(value, str(index))
        print(log)
    
    # Closing files
    file.close()
