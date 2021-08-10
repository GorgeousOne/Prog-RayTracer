#include "ray.hpp"

Ray transformRay(glm::mat4 const& mat, Ray const& ray) {
	glm::vec4 new_origin = mat * glm::vec4{ ray.origin, 1 };
	glm::vec4 new_direction = mat * glm::vec4{ ray.direction, 0 };
	return { glm::vec3{new_origin}, glm::vec3{new_direction} };
}