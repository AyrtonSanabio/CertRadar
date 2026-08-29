#include "certradar/product_info.hpp"

#include <doctest/doctest.h>

#include <string>

TEST_CASE("product_name returns the public product identity") {
    REQUIRE(certradar::product_name() != nullptr);
    CHECK(std::string(certradar::product_name()) == "CertRadar");
}
