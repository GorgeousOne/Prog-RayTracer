
#ifndef RAYTRACER_RAY_HPP
#define RAYTRACER_RAY_HPP

#include <glm/glm.hpp>

struct Ray {
	glm::vec3 origin = {0.0f, 0.0f, 0.0f};
	glm::vec3 direction = {0.0f, 0.0f, -1.0f};

	[[nodiscard]] glm::vec3 point(float distance) const {
		return origin + direction * distance;
	}
};

Ray transformRay(glm::mat4 const& mat, Ray const& ray);

#endif
