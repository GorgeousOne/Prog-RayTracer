#define CATCH_CONFIG_RUNNER

#include <catch.hpp>
#include <glm/vec3.hpp>
#include "Sphere.h"
#include "Box.h"

#define PI 3.14159265f

//Aufgabe 5.2
TEST_CASE("create a sphere", "[geometry]") {
	Sphere s0{1};
	REQUIRE(4 * PI == Approx(s0.area()));
	REQUIRE(4.0f / 3.0f * PI == Approx(s0.volume()));

	Sphere s1{0};
	REQUIRE(0 == Approx(s1.area()));
	REQUIRE(0 == Approx(s1.volume()));

	Sphere s2{-3};
	REQUIRE(36 * PI == Approx(s2.area()));
	REQUIRE(36 * PI == Approx(s2.volume()));
}

TEST_CASE("create cuboid", "[geometry]") {
	Box b0{{0, 0, 0}, {1, 2, 3}};
	REQUIRE(22 == b0.area());
	REQUIRE(6 == b0.volume());

	Box b1{{-1, -1, -1}, {1, 1, 1}};
	REQUIRE(24 == b1.area());
	REQUIRE(8 == b1.volume());
}

int main(int argc, char *argv[]) {
	return Catch::Session().run(argc, argv);
}
