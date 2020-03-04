#include "catch.hpp"
#include "Node.h"
#include "Simulation.h"

TEST_CASE("Testing getActive", "[classic]") {
    Node a;
    REQUIRE(a.getActive());
}

TEST_CASE("Test foo", "[classic]") {
    Simulation s;
}