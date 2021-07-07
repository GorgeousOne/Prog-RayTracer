#ifndef RAYTRACER_POINTLIGHT_H
#define RAYTRACER_POINTLIGHT_H

#include "ambientLight.hpp"

struct PointLight : AmbientLight {
	glm::vec3 position = {};
};
#endif
