import csv
import json
import os
import subprocess
import sys

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
    subprocess.run(["./adak"], stderr=sys.stderr, stdout=sys.stdout)

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

