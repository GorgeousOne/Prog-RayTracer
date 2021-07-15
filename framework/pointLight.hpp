#ifndef RAYTRACER_POINTLIGHT_H
#define RAYTRACER_POINTLIGHT_H

#include "light.hpp"

struct PointLight : Light {
	glm::vec3 position = {};
};
#endif
