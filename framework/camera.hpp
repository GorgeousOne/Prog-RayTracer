#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <glm/glm.hpp>
#include <string>
#include "color.hpp"
#define PI 3.14159265f

struct Camera {
	std::string name = "default";
	float fov_x = PI / 3;
	glm::vec3 position {};
	glm::vec3 direction {0, 0, -1};
	glm::vec3 up {0, 1, 0};
};
#endif
