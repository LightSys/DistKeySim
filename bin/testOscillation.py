#!/usr/bin/env python3

# Assert the absence of oscillation, i.e., number of keyspaces
# does not grow with number of iterations.

import csv
import json
import os
import subprocess

import numpy as np

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

if __name__ == '__main__':

    cv = lambda x: np.std(x, ddof=1) / np.mean(x)

    configFileName = "config/oscillation-config.json"
    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",     500,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000,  2))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000,  2))
    print(numKeyspaces, "mean=", np.mean(numKeyspaces), "std=", np.std(numKeyspaces, ddof=1), "cv=", cv(numKeyspaces))
    assert(cv(numKeyspaces) < 1)

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "single",    50, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",   500, 10))
    numKeyspaces.append(runOneSim(configFileName, "single",  5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "single", 50000, 10))
    print(numKeyspaces, "mean=", np.mean(numKeyspaces), "std=", np.std(numKeyspaces, ddof=1), "cv=", cv(numKeyspaces))
    assert(cv(numKeyspaces) < 1)

    numKeyspaces = []
    numKeyspaces.append(runOneSim(configFileName, "full",      50, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",     500, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",    5000, 10))
    numKeyspaces.append(runOneSim(configFileName, "full",   50000, 10))
    print(numKeyspaces, "mean=", np.mean(numKeyspaces), "std=", np.std(numKeyspaces, ddof=1), "cv=", cv(numKeyspaces))
    assert(cv(numKeyspaces) < 1)
