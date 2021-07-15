
#ifndef RAYTRACER_HITPOINT_HPP
#define RAYTRACER_HITPOINT_HPP
#include <glm/glm.hpp>
#include "color.hpp"
#include "material.hpp"

struct HitPoint {
	bool does_intersect = false;
	float intersection_distance = 0.0f;
	std::string hit_object = "null";
	std::shared_ptr<Material> hit_object_color = nullptr;
	glm::vec3 intersection {};
	glm::vec3 ray_direction {};
	glm::vec3 surface_normal {};
};
#endif
