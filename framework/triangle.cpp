#include "triangle.hpp"
#include <glm/gtx/intersect.hpp>

#define EPSILON 0.0001f

Triangle::Triangle(
		glm::vec3 const& v0,
		glm::vec3 const& v1,
		glm::vec3 const& v2,
		std::string const& name,
		std::shared_ptr<Material> material) :
		Triangle(v0, v1, v2, glm::normalize(glm::cross(v1 - v0, v2 - v0)), name, material) {}

Triangle::Triangle(
		glm::vec3 const& v0,
		glm::vec3 const& v1,
		glm::vec3 const& v2,
		glm::vec3 const& n,
		std::string const& name,
		std::shared_ptr<Material> material) :
		Shape(name, material),
		v0_{v0},
		v1_{v1},
		v2_{v2},
		n_ {n} {}

float Triangle::area() const {
	glm::vec3 v0v1 = v1_ - v0_;
	glm::vec3 v0v2 = v2_ - v0_;
	return glm::length(glm::cross(v0v1, v0v2)) / 2;
}

float Triangle::volume() const {
	return 0;
}

std::ostream &Triangle::print(std::ostream &os) const {
	Shape::print(os);
	return os << "\nv0:" << v0_ << "\nv1:" << v1_ << "\nv2:" << v2_ << "\nn:" << n_ << std::endl;
}

//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
HitPoint Triangle::intersect(Ray const& ray) const {
	glm::vec3 v0v1 = v1_ - v0_;
	glm::vec3 v0v2 = v2_ - v0_;
	glm::vec3 p_vec = glm::cross(ray.direction, (v0v2));
	float det =glm::dot(v0v1, p_vec);

	//returns if the triangle is facing the triangle backwards / is parallel to it
	//remove "det > -EPSILON &&" for large objs maybe
    if (det > -EPSILON && det < EPSILON) {
    	return {};
    }
	float inv_det = 1 / det;
	glm::vec3 t_vec = ray.origin - v0_;
	float u = glm::dot(t_vec, p_vec) * inv_det;

	if (u < 0 || u > 1) {
		return {};
	}
	glm::vec3 q_vec = glm::cross(t_vec, v0v1);
	float v = glm::dot(ray.direction, q_vec) * inv_det;

	if (v < 0 || u + v > 1) {
		return {};
	}
	t = glm::dot(v0v2, q_vec) * inv_det;

	if (t < EPSILON) {
		return {};
	}else {
		t -= EPSILON;
		return {true, t, name_, material_, ray.point(t), ray.direction, n_};
	}
}
