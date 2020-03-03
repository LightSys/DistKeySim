#include "catch.hpp"
#include "Node.h"

TEST_CASE("Testing getActive", "[classic]") {
    Node a;
    REQUIRE(a.getActive());
}