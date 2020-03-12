#!/bin/bash

# author: Colin Burdine

#NOTE: this script may not work on all
#    machines. It repeats the process
#	 in the ReadMe file. This script
#	 is tailored for working on the
#	 server set up for development. 

PROJ_NAME=adak
ADAK_CONFIG_FILE_PATH=./config.json
cd ..

if [[ -d ./build ]]; then
	echo 'clobbering build directory...'
	rm -rf ./build
fi

mkdir build
cd build
cmake .. -DBUILD_TESTING=0

make

mkdir bin
mv "./src/$PROJ_NAME" ./bin/

cd ..
cp $ADAK_CONFIG_FILE_PATH ./build/bin/


# return to scripts directory:
cd scripts
