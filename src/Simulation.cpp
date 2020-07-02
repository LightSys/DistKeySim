
#include "Simulation.h"
#include "ControlStrategy.h"

using json = nlohmann::json;

// Number of rounds to complete to allow the simulation to settle
static const int NUM_ROUNDS = 50;

Simulation::Simulation(const struct Config &config)
    : numNodes(config.numNodes), network(Network(config.connectionMode, config.visiblePeers, config.lambda1, 
			    config.lambda2, config.lambda3, config.networkScale, config.latency, 
			    config.customLambda1, config.customLambda2, config.customLambda3)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    //needed numerouse items not in the Simulation class. Could move all into it to avoid acessing the file twice. 
    Config config(ifstream("config.json"));
    
    Node::setUnitsPerDay(config.unitsPerDay);
     
    ControlStrategy::setAccuracy(config.longTermPrecision);
     
    string message = "Started Simulation";
    Logger::log(message);

    // Create root node that will have the max keyspace 0/0
    network.addRootNode();

    cout << "Root UUID: " << network.getNodes().begin()->first << endl;

    // Create new nodes and add them to the map
    for (int i = 1; i < numNodes; i++) {
        UUID newNodeID = network.addEmptyNode();
        if(i % config.heartbeatFrequency == 0){  network.doAllHeartbeat(config.chunkiness);}
       	network.checkAndSendAllNodesLatency(config.latency);
	network.doAllTicks();
    	//update all of the logger stuff
       	Logger::getTimeslot(true);//increment timeslot
        Logger::getShared(true,0);//reset shared
        Logger::getConsumption(true,0);//reset consumption
     }


    //double lambda1 = 2.0, lambda2 = 3.0;//time till offline and time till online
    //now VVV uses the config's lambda
    EventGen* eventGen = new GeometricDisconnect(SIMULATION, config.lambda1, config.lambda2);

    network.printChannels();

    cout << "Ticking network a bunch" << endl;
    for(int i=0; i<config.simLength; i++){
        cout << "***********************************************Tick" << endl;
	cout << "Time Step:" << numNodes + 1 + i <<  " ... " <<  endl; 
        //adding stuff to make the tics have event
//All of these left-most staments are for debugging runtimes. Can help show the slowest sections of the code. 
//auto start = std::chrono::high_resolution_clock::now();
	network.checkAndSendAllNodesLatency(config.latency);
//auto end = std::chrono::high_resolution_clock::now();
//cout << "check and send took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//  << " ns" << endl;	
       //making the aggregation creation rate into a heartbeat ratio
       //so we when its changed in the gui this changes it... 
//start = std::chrono::high_resolution_clock::now();

      if(i % config.heartbeatFrequency == 0){
          network.doAllHeartbeat(config.chunkiness); 
       }
//end = std::chrono::high_resolution_clock::now();
//cout << "do all heartbeats took " <<std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//  << " ns" << endl;
 
//start = std::chrono::high_resolution_clock::now();

        if(config.runEvents){
           eventGen->eventTick(&network);
	}
        
	//tell nodes to CONSUMEEEEE (nom nom nom nom nom)
        network.tellAllNodesToConsumeObjects();

//end = std::chrono::high_resolution_clock::now();
//cout << "event tick took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
// << " ns" << endl;
	network.printChannels();
	network.doAllTicks();

	//update all of the logger stuff
       	Logger::getTimeslot(true);//increment timeslot
        Logger::getShared(true,0);//reset shared
        Logger::getConsumption(true,0);//reset consumpt
       
    }

    //here? 

    delete eventGen;
}
