#include <ctime>
#include "Random.h"

using namespace std;

Random::Random() : EventGen() {
    // Seed Random
    cout << "Random Class Created... Seeding random number." << endl;
    srand(time(NULL));
}

void Random::doSomethingRandom(Simulation* simulation) {
    int action = rand() % 3 + 1; // random number between 1-3
    switch(action) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            cout << "Something wrong with random!" << endl;
            break;
    }
}