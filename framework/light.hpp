#ifndef RAYTRACER_AMBIENTLIGHT_H
#define RAYTRACER_AMBIENTLIGHT_H

#include <glm/glm.hpp>
#include "color.hpp"

struct Light {
	std::string name = "default";
	Color color {255, 255, 255};
	float brightness = 1;
};
#endif
