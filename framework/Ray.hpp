
#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <glm/vec3.hpp>

struct Ray {
	glm::vec3 origin = {0.0f, 0.0f, 0.0f};
	glm::vec3 direction = {0.0f, 0.0f, -1.0f};

	[[nodiscard]] glm::vec3 point(float distance) const {
		return origin + direction*distance;
	}
};
#endif
