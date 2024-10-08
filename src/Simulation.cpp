
#include "ControlStrategy.h"
#include "DoNothingStrategy.h"
#include "EventGen.h"
#include "Factory.h"
#include "GeometricDisconnect.h"
#include "Logger.h"
#include "Simulation.h"
#include "UUID.h"

using json = nlohmann::json;

// Number of rounds to complete to allow the simulation to settle
static const int NUM_ROUNDS = 50;

Simulation::Simulation(const struct Config& config)
    : numNodes(config.numNodes),
      network(Network(config.connectionMode, config.visiblePeers, config.lambda1, config.lambda2,
                      config.lambda3, config.networkScale, config.latency, config.customLambda1,
                      config.customLambda2, config.customLambda3, config.enableSendMsgLog)) {
    // Seed random number
    srand(config.randomSeed);
}

void Simulation::run() {
    // needed numerouse items not in the Simulation class. Could move all into it to avoid acessing
    // the file twice.
    Config config(ifstream("config.json"));

    Logger::log(Formatter() << "Making " << config.adakStrategy << " strategy");
    adakStrategy = Factory::makeAdakStrategy(config);
    adakStrategy->setAccuracy(config.longTermPrecision);

    Logger::log("Started Simulation");

    // Create root node that will have the max keyspace 0/0
    network.addRootNode(config.randomSeed);

    Logger::log(Formatter() << "Root UUID: " << network.getNodes().begin()->first);

    // Create new nodes and add them to the map
    for (int i = 1; i < numNodes; i++) {
        UUID newNodeID = network.addEmptyNode(config.randomSeed);
        if (i % config.heartbeatFrequency == 0) {
            network.doAllHeartbeat(adakStrategy, config.chunkiness);
        }
        network.checkAndSendAllNodesLatency(config.latency);
        network.doAllTicks();
        // update all of the logger stuff
        Logger::getTimeslot(true);        // increment timeslot
        Logger::getShared(true, 0);       // reset shared
        Logger::getConsumption(true, 0);  // reset consumption
    }

    // double lambda1 = 2.0, lambda2 = 3.0;//time till offline and time till online
    // now VVV uses the config's lambda
    EventGen* eventGen = new GeometricDisconnect(SIMULATION, config.lambda1, config.lambda2);

    network.printChannels();
    network.printKeyspaces();

    Logger::log(Formatter() << "Ticking network " << config.simLength << " iterations");
    long prevPercentComplete = -1;
    for (long i = 0; i < config.simLength; i++) {
        Logger::log(Formatter() << "***********************************************Tick");
        int step = numNodes + 1 + i;
        Logger::log(Formatter() << "Time Step " << step
            << ", SimTime " << double(i)/double(config.timeStepUnitsPerSecond) << "s ... ");
        long percentComplete = long(i * 100) / long(config.simLength);
        Logger::log(Formatter()
            << "percentComplete ( " << percentComplete << ")"
            << " = i (" << i << ")"
            << " * 100 / config.simLength (" << config.simLength << ")");
        if (percentComplete != prevPercentComplete) {
            std::cout <<  percentComplete << "%\r" << std::flush;
            prevPercentComplete = percentComplete;
        }
        // adding stuff to make the tics have event
        // All of these left-most statements are for debugging runtimes. Can help show the slowest
        // sections of the code. auto start = std::chrono::high_resolution_clock::now();
        network.checkAndSendAllNodesLatency(config.latency);
        // auto end = std::chrono::high_resolution_clock::now();
        // Logger::log(Formatter() << "check and send took " <<
        // std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        //  << " ns");
        // making the aggregation creation rate into a heartbeat ratio
        // so we when its changed in the gui this changes it...
        // start = std::chrono::high_resolution_clock::now();

        if (i % config.heartbeatFrequency == 0) {
            network.doAllHeartbeat(adakStrategy, config.chunkiness);
        }
        // end = std::chrono::high_resolution_clock::now();
        // Logger::log(Formatter() << "do all heartbeats took "
        // <<std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        //  << " ns");

        // start = std::chrono::high_resolution_clock::now();

        if (config.runEvents) {
            eventGen->eventTick(&network);
        }

        // tell nodes to CONSUMEEEEE (nom nom nom nom nom)
        network.tellAllNodesToConsumeObjects();

        // end = std::chrono::high_resolution_clock::now();
        // Logger::log(Formatter() << "event tick took " <<
        // std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        // << " ns");
        network.printChannels();
        network.printKeyspaces();
        network.doAllTicks();

        // update all of the logger stuff
        Logger::getTimeslot(true);        // increment timeslot
        Logger::getShared(true, 0);       // reset shared
        Logger::getConsumption(true, 0);  // reset consumpt
    }

    // here?

    delete eventGen;
}
