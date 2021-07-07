#define CATCH_CONFIG_RUNNER

#include <catch.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#include "sphere.hpp"
#include "box.hpp"
#include "scene.hpp"

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

	Box b1{{-1, -1, -1}, {1,  1,  1}};
	REQUIRE(24 == b1.area());
	REQUIRE(8 == b1.volume());

	Box b2{{}, {}};

	REQUIRE_THROWS(Box{{0, 0, 0}, {-1, -1, -1}});
}

//Aufgabe 5.5
TEST_CASE("print shapes", "[print]") {
	Box b{{0, 0, 0}, {1, 2, 3}};
	b.print(std::cout);

	Sphere s{1};
	s.print(std::cout);
}

//Aufgabe 5.6
TEST_CASE("intersect_ray_sphere", "[intersect]") {
	// Ray
	glm::vec3 ray_origin{0.0f, 0.0f, 0.0f};
	glm::vec3 ray_direction{0.0f, 0.0f, 1.0f};
	// Sphere
	glm::vec3 sphere_center{0.0f, 0.0f, 5.0f};
	float sphere_radius{1.0f};
	float distance = 0.0f;

	auto result = glm::intersectRaySphere(
			ray_origin, ray_direction,
			sphere_center,
			sphere_radius * sphere_radius,
			distance);

	REQUIRE(true == result);
	REQUIRE(distance == Approx(4.0f));
}

TEST_CASE("intersect_ray_sphere2", "[intersect]") {
	float t;
	Ray ray{{0.0f, 0.0f, 0.0f},
	        {0.0f, 0.0f, 1.0f}};

	Sphere sphere0{2.0f, {0.0f, 0.0f, 10.0f}};
	HitPoint hit0 = sphere0.intersect(ray, t);
	REQUIRE(true == hit0.does_intersect);
	REQUIRE(8.0f == hit0.intersection_distance);
	REQUIRE(glm::vec3{0.0f, 0.0f, 8.0f} == hit0.intersection_point);

	Sphere sphere1{2.0f, {4.0f, 0.0f, 10.0f}};
	HitPoint hit1 = sphere1.intersect(ray, t);
	REQUIRE(false == hit1.does_intersect);

	Sphere sphere2{2.0f, {2.0f, 0.0f, 10.0f}};
	HitPoint hit2 = sphere2.intersect(ray, t);

	REQUIRE(true == hit2.does_intersect);
	REQUIRE(10.0f == hit2.intersection_distance);
	REQUIRE(glm::vec3{0.0f, 0.0f, 10.0f} == hit2.intersection_point);
}

//Aufgabe 5.8
TEST_CASE("virtual_destructor", "[destruct]") {
	glm::vec3 position{0.0f, 0.0f, 0.0f};

	auto *s1 = new Sphere{1.2f, position, "sphere0"};
	Shape *s2 = new Sphere{1.2f, position, "sphere1"};

	s1->print(std::cout) << std::endl << std::endl;
	s2->print(std::cout) << std::endl << std::endl;

	delete s1;
	delete s2;
}

int main(int argc, char *argv[]) {

	Scene scene1{};
	std::istringstream words_stream("material red 1 2 3 4 5 6 7 8 9 10");

	add_to_scene(words_stream, scene1);

	return Catch::Session().run(argc, argv);
}
