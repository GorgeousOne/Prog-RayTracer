#ifndef RAYTRACER_AMBIENTLIGHT_H
#define RAYTRACER_AMBIENTLIGHT_H

#include <glm/vec3.hpp>

struct AmbientLight {
	std::string name = "default";
	glm::vec3 color = {};
	float brightness = 0;
};
#endif
