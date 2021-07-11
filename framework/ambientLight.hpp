#ifndef RAYTRACER_AMBIENTLIGHT_H
#define RAYTRACER_AMBIENTLIGHT_H

#include <glm/glm.hpp>
#include "color.hpp"

struct AmbientLight {
	std::string name = "default";
	Color color = {};
	float brightness = 0;
};
#endif
