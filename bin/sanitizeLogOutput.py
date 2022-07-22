#!/usr/bin/env python3

# Help: sanitizeLogOutput.py -h

import argparse
import datetime
import os
import sys

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

    fileSize = os.path.getsize(args.logOutput)
    currentSize = 0
    prevPercentComplete = -1

    file = open(args.logOutput, 'r')
    
    uuids = []
    foundChannels = False
    foundToFrom = False
    for line in file:
        percentComplete = int(currentSize*100.0/fileSize)
        if percentComplete > prevPercentComplete:
            sys.stderr.write("%d%%\r" % percentComplete)
            prevPercentComplete = percentComplete
        currentSize += len(line)

        lineParts = line.rstrip().split(splitOn)
        log = lineParts[0]

        # Find UUIDs
        if "Root UUID:" in log:
            uuidParts = log.split(": ")
            uuid = uuidParts[1]
            if uuid not in uuids:
                uuids.append(uuid)
        if "UUID root " in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[2]
            if uuid not in uuids:
                uuids.append(uuid)
        if " sent to " in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[0]
            if uuid not in uuids:
                uuids.append(uuid)
            uuid = uuidParts[3]
            if uuid not in uuids:
                uuids.append(uuid)
        if "calcing long" in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[0]
            if uuid not in uuids:
                uuids.append(uuid)
        if "objectConsumptionRatePerSecond" in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[0]
            if uuid not in uuids:
                uuids.append(uuid)
        if "+-peer: " in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[2]
            if uuid not in uuids:
                uuids.append(uuid)
        if "changed consumption rate" in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[0]
            if uuid not in uuids:
                uuids.append(uuid)
        if "(*d3)(*gen)" in log:
            uuidParts = log.split(" ")
            uuid = uuidParts[0]
            if uuid not in uuids:
                uuids.append(uuid)
        foundChannels = foundChannels or "CHANNELS" in log
        foundToFrom = foundToFrom or "TO,FROM,ID" in log
        if foundChannels and foundToFrom and "TO,FROM,ID" not in log:
            uuidParts = log.split(",")
            if len(uuidParts) == 3 and uuidParts[2].isnumeric():
                for uuid in uuidParts[0:1]:
                    if uuid not in uuids:
                        uuids.append(uuid)
            else:
                foundChannels = False
                foundToFrom = False
        
        # replace uuids with index into set array
        for index, value in enumerate(uuids):
            log = log.replace(value, "uuid" + str(index))
        print(log)
    
    # Closing files
    file.close()
    sys.stderr.write("100%\n")
