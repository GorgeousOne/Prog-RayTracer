#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <glm/glm.hpp>
#include <string>
#include "color.hpp"

struct Camera {
	std::string name = "default";
	float fov_x = 60.0f;
	glm::vec3 position {};
	glm::vec3 direction {0, 0, -1};
	glm::vec3 up {0, 1, 0};
};
#endif
