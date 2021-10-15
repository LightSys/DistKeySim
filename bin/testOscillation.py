#!/usr/bin/env python3

# Assert the absence of oscillation, i.e., number of keyspaces
# does not grow with number of iterations.

import csv
import json
import numpy as np
import os
import statistics
import subprocess

def runOneSim(configFileName, connectionMode, simLength, numNodes):
 
    # Read and edit config file
    with open(configFileName, 'r') as configFile:
       config = json.load(configFile)
    config["connectionMode"] = connectionMode
    config["simLength"] = simLength
    config["numNodes"] = numNodes

    # Write config file as JSON
    editedConfigFileName = "build/src/config.json"
    json_object = json.dumps(config, indent = 4)
    with open(editedConfigFileName, "w") as outfile:
        outfile.write(json_object)

    # Run one simulation
    cwd = os.getcwd()
    os.chdir("build/src")
    subprocess.run(["./adak"], capture_output=True)

    # Compute total number of keyspaces
    uuids = {}
    open('statslog.csv')
    with open('statslog.csv') as statsFile:
        reader = csv.DictReader(statsFile)
        for row in reader:
            uuid = row['UUID']
            numKeyspaces = row['numKeyspaces']
            uuids[uuid] = numKeyspaces

    totalNumKeyspaces = 0
    for uuid in uuids:
        if uuid in uuids and uuids[uuid] is not None:
            totalNumKeyspaces += int(uuids[uuid])

    os.chdir(cwd)

    return totalNumKeyspaces

def hasOutliers(x):
    # Parameters
    k = 2                       # window width above and below, i.e. the window size is 2*k + 1
    outlierStandardDev = 2      # Number of standard deviations away at which we call a point an outlier

    # Calculate medians within a sliding window
    L = len(x)
    result = []
    for j in range(L):
        windowBottom = j-k
        windowTop = j+k
        if windowBottom < 0:
            windowBottom = 0
        if windowTop > L:
            windowTop = L
        window = x[windowBottom:windowTop+1]
        windowMedian = np.median(window)
        windowDeviation = abs(window - windowMedian) / 1.4826
        print(window)
        print(windowDeviation)
        # 1.4826 is approx (sqrt(2) erfi(1/2))^-1, scales from median deviation to gaussian standard deviation
        for jj in range(len(windowDeviation)):
            if windowDeviation[jj] > outlierStandardDev:
                result.append(windowBottom + jj + 1)

    # Remove duplicates
    outlierIndices = []
    [outlierIndices.append(r) for r in result if r not in outlierIndices]

    print('\nOutlier index/value pairs:\n')
    for i in outlierIndices:
        print('(' + str(i) + ', ' + str(x[i]) + ')')
    print('\n')
    
    return len(outlierIndices) > 0

if __name__ == '__main__':

    cv = lambda x: statistics.stdev(x) / statistics.mean(x)

    configFileName = "config/oscillation-config.json"
    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",     500,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000,  2))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)
    # Doesn't work:
    # assert(not hasOutliers(numKeyspaces))

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "single",    50, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",   500, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",  5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "single", 50000, 10))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)
    # Doesn't work:
    # assert(not hasOutliers(numKeyspaces))

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",     500, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000, 10))
    print(f"pop={numKeyspaces}, mean={statistics.mean(numKeyspaces)}, stdev={statistics.stdev(numKeyspaces)}, cv={cv(numKeyspaces)}, cv<1={cv(numKeyspaces) < 1}")
    assert(cv(numKeyspaces) < 1)
    # Doesn't work:
    # assert(not hasOutliers(numKeyspaces))
