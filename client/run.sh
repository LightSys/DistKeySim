#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Illegal number of parameters; include simulation number, ip, username, and password"
    exit 2
fi

server_address=$2
server_username=$3
server_password=$4

echo "using server at: \"$server_address\"" 

# SCP the config file to the server .. 
echo "sending config file to server..."

sshpass -p "$server_password" scp ./config.json $server_username@$server_address:~/devel-josh/DistKeySim/ 


#ssh to server 
echo "sshing to server..."

sshpass -p "$server_password" ssh -tt $server_username@$server_address << EOF
    @echo OFF
    cd $server_username-josh/DistKeySim/scripts

    echo "run simulation! ..."

    rm -rf ~/temp_simulation_out

    mkdir ~/temp_simulation_out

    touch ~/temp_simulation_out/build_output.txt

    
    echo "building..."
    ./build.sh > ~/temp_simulation_out/build_output.txt
    echo "done building!"
    
    
    cd ../build/bin/

    touch ~/temp_simulation_out/output.txt 
    
    echo "running ADAK..."
    ./adak > ~/temp_simulation_out/output.txt 
    echo "adak finished running! "



    exit

EOF


#copy files from server 
echo "copying files from server... "

rm -rf "simulation_$1"
mkdir "simulation_$1" 
sshpass -p "$server_password" scp $server_username@$server_address:~/temp_simulation_out/* ./simulation_$1/ 


echo "finished!"
