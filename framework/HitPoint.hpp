
#ifndef RAYTRACER_HITPOINT_H
#define RAYTRACER_HITPOINT_H
#include <glm/vec3.hpp>
#include "color.hpp"

struct HitPoint {
	bool does_intersect = false;
	float intersection_distance = 0.0f;
	std::string hit_object = "null";
	Color hit_object_color = Color{};
	glm::vec3 intersection_point = glm::vec3{};
	glm::vec3 ray_direction = glm::vec3{};
};
#endif
