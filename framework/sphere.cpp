#include "sphere.hpp"
#include <cmath>   //pow

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#define PI 3.14159265f
#define EPSILON 0.0001f

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

//Aufgabe 5.5
std::ostream& Sphere::print(std::ostream &os) const {
	Shape::print(os);
	return os << "\nradius: " << radius_ << "\ncenter: " << center_ << std::endl;
}

//Aufgabe 5.6
HitPoint Sphere::intersect(Ray const& ray, float &t) const {
	float distance = 0.0f;

	bool result = glm::intersectRaySphere(
			ray.origin, glm::normalize(ray.direction),
			center_,
			radius_ * radius_,
			distance);

	glm::vec3 intersection = ray.point(distance - EPSILON);
	return HitPoint{result, distance, name_, material_, intersection, ray.direction, get_surface_normal(intersection)};
}

glm::vec3 Sphere::get_surface_normal(glm::vec3 const& intersection) const {
	return glm::normalize(intersection - center_);
}
