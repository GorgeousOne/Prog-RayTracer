#include "ray.hpp"

Ray transform_ray(glm::mat4 const& mat, Ray const& ray) {
	glm::vec4 new_origin = mat * glm::vec4{ ray.origin, 1 };
	glm::vec4 new_direction = mat * glm::vec4{ ray.direction, 0 };
	return { glm::vec3{new_origin}, glm::vec3{new_direction} };
}

glm::vec3 transform_vec3(glm::mat4 const& mat, glm::vec3 const& v, bool is_origin) {
	glm::vec4 new_v = mat * glm::vec4{v, is_origin};
	return glm::vec3{new_v};
}