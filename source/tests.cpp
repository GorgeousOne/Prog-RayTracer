#define CATCH_CONFIG_RUNNER

#include <catch.hpp>
#include <glm/glm.hpp>
#include "Sphere.h"

#define PI 3.14159265f

TEST_CASE("create a sphere", "[geometry") {
	Sphere s0{1};
	Sphere s1{0};
	Sphere s2{-3};

	REQUIRE(4.0f / 3.0f * PI == Approx(s0.volume()));
	REQUIRE(0 == Approx(s1.volume()));
	REQUIRE(4.0f / 3.0f * PI * pow(-3, 3) == Approx(s2.volume()));

}

int main(int argc, char *argv[]) {
	return Catch::Session().run(argc, argv);
}
