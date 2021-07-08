
#ifndef RAYTRACER_HITPOINT_HPP
#define RAYTRACER_HITPOINT_HPP
#include <glm/vec3.hpp>
#include "color.hpp"
#include "material.hpp"

struct HitPoint {
	bool does_intersect = false;
	float intersection_distance = 0.0f;
	std::string hit_object = "null";
	std::shared_ptr<Material> hit_object_color = nullptr;
	glm::vec3 intersection_point = glm::vec3{};
	glm::vec3 ray_direction = glm::vec3{};
};
#endif
