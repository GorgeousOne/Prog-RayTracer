
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

Ray transform_ray(glm::mat4 const& mat, Ray const& ray);
glm::vec3 transform_vec3(glm::mat4 const& mat, glm::vec3 const& v, bool is_origin = false);

#endif
