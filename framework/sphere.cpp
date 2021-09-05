#include "sphere.hpp"
#include <cmath>   //pow

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#define PI 3.14159265f
#define EPSILON 0.1f

Sphere::Sphere(float radius, glm::vec3 const& center, std::string const& name, std::shared_ptr<Material> material) :
		Shape(name, material),
		radius_(std::abs(radius)),
		center_(center) {}

float Sphere::area() const {
	return 4.0f * PI * pow(radius_, 2);
}

float Sphere::volume() const {
	return 4.0f / 3.0f * PI * std::abs(pow(radius_, 3));
}

glm::vec3 Sphere::min(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 radius_trans = transform_vec3(transformation, {radius_, radius_, radius_});
	return center_ + glm::vec3{transformation[3]} - radius_trans;
}

glm::vec3 Sphere::max(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 radius_trans = transform_vec3(transformation, {radius_, radius_, radius_});
	return center_ + glm::vec3{transformation[3]} + radius_trans;
}

//Aufgabe 5.5
std::ostream& Sphere::print(std::ostream &os) const {
	Shape::print(os);
	return os << "\nradius: " << radius_ << "\ncenter: " << center_ << std::endl;
}

//Aufgabe 5.6
HitPoint Sphere::intersect(Ray const& ray) const {
	float t;
	Ray ray_inv = transform_ray(world_transformation_inv_, ray);

	bool does_intersect = glm::intersectRaySphere(
			ray_inv.origin, glm::normalize(ray_inv.direction),
			center_,
			radius_ * radius_,
			t);

	if (does_intersect) {
		t /= glm::length(ray_inv.direction);
		t -= EPSILON;
		glm::vec3 surface_normal_inv = get_surface_normal(ray_inv.point(t));
		glm::vec3 surface_normal = transform_vec3(world_transformation_, surface_normal_inv);
		return HitPoint{ true, t, name_, material_, ray.point(t), ray.direction, glm::normalize(surface_normal)};
	}else {
		return {};
	}
}

glm::vec3 Sphere::get_surface_normal(glm::vec3 const& intersection_inv) const {
	return glm::normalize(intersection_inv - center_);
}
