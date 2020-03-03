TEST_CASE("Testing getActive", "[classic]") {
    Node a = new Node();
    REQUIRE(a.getActive() == true);
}