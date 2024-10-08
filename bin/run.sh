#!/bin/bash

if [ "$#" -ne 6 ]; then
    echo "Illegal number of parameters; include simulation number, ip, username, password, DistKeySim folder location on the server (including the DistKeySim folder), and port number"
    exit 2
fi

server_address=$2
server_username=$3
server_password=$4

server_file_loc=$5

server_port=$6

echo "using server at: \"$server_address\"" 

# SCP the config file to the server .. 
echo "sending config file to server..."

#uses config in client folder 
sshpass -p "$server_password" scp -P $server_port ./config.json $server_username@$server_address:"$server_file_loc"


#ssh to server 
echo "sshing to server..."

sshpass -p "$server_password" ssh -p $server_port -tt $server_username@$server_address << EOF
    cd $server_file_loc
    cd scripts

    echo "run simulation! ..."

    rm -rf $server_file_loc/temp_simulation_out

    mkdir $server_file_loc/temp_simulation_out

    touch $server_file_loc/temp_simulation_out/build_output.txt

    
    echo "building..."
    ./build.sh > $server_file_loc/temp_simulation_out/build_output.txt
    echo "done building!"
    
    
    cd ../build/bin/

    touch $server_file_loc/temp_simulation_out/output.txt 
    
    echo "running ADAK..."
    ./adak > $server_file_loc/temp_simulation_out/output.txt 
    echo "adak finished running! "

    rm $server_file_loc/temp_simulation_out/logOutput.txt
    cp ./logOutput.txt $server_file_loc/temp_simulation_out/logOutput.txt
    rm ./logOutput.txt
    
    rm $server_file_loc/temp_simulation_out/statsLog.csv
    cp ./statsLog.csv $server_file_loc/temp_simulation_out/statsLog.csv
    rm ./statsLog.csv

    exit

EOF


#copy files from server 
echo "copying files from server... "

######################

mkdir "simulations"
rm -rf "simulations/simulation_$1"
mkdir "simulations/simulation_$1" 


sshpass -p "$server_password" scp -P $server_port -r $server_username@$server_address:$server_file_loc/temp_simulation_out/* ./simulations/simulation_$1/ 

############

echo "removing temp files from server..."

sshpass -p "$server_password" ssh -p $server_port -tt $server_username@$server_address << EOF
    rm $server_file_loc/temp_simulation_out/ -rf
    exit
EOF



echo "finished!"
