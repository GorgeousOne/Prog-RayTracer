#include "triangle.hpp"
#include <glm/gtx/intersect.hpp>

#define EPSILON 0.001f

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

glm::vec3 Triangle::min(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 v0_trans = transform_vec3(transformation, v0_, true);
	glm::vec3 v1_trans = transform_vec3(transformation, v1_, true);
	glm::vec3 v2_trans = transform_vec3(transformation, v2_, true);
	return glm::min(glm::min(v0_trans, v1_trans), v2_trans);
}

glm::vec3 Triangle::max(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 v0_trans = transform_vec3(transformation, v0_, true);
	glm::vec3 v1_trans = transform_vec3(transformation, v1_, true);
	glm::vec3 v2_trans = transform_vec3(transformation, v2_, true);
	return glm::max(glm::max(v0_trans, v1_trans), v2_trans);
}

std::ostream &Triangle::print(std::ostream &os) const {
	Shape::print(os);
	return os << "\nv0:" << v0_ << "\nv1:" << v1_ << "\nv2:" << v2_ << "\nn:" << n_ << std::endl;
}

//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
HitPoint Triangle::intersect(Ray const& ray) const {
	Ray ray_inv = transform_ray(world_transformation_inv_, ray);

	glm::vec3 v0v1 = v1_ - v0_;
	glm::vec3 v0v2 = v2_ - v0_;
	glm::vec3 p_vec = glm::cross(ray_inv.direction, (v0v2));
	float det = glm::dot(v0v1, p_vec);

	//returns if the ray is parallel to the triangle
    if (det < EPSILON && det > -EPSILON) {
    	return {};
    }
	float inv_det = 1 / det;
	glm::vec3 t_vec = ray_inv.origin - v0_;
	float u = glm::dot(t_vec, p_vec) * inv_det;

	if (u < 0 || u > 1) {
		return {};
	}
	glm::vec3 q_vec = glm::cross(t_vec, v0v1);
	float v = glm::dot(ray_inv.direction, q_vec) * inv_det;

	if (v < 0 || u + v > 1) {
		return {};
	}
	float t = glm::dot(v0v2, q_vec) * inv_det;

	if (t < EPSILON) {
		return {};
	}else {
		t -= EPSILON;

		glm::vec3 surface_normal = transform_vec3(world_transformation_, n_);
		if (glm::dot(surface_normal, ray_inv.direction) > 0) {
			surface_normal *= -1;
		}
		return {true, t, name_, material_, ray.point(t), ray.direction, glm::normalize(surface_normal)};
	}
}
