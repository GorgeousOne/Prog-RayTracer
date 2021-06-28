#include "Sphere.h"
#include <cmath>   //pow

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#define PI 3.14159265f

Sphere::Sphere(float radius, glm::vec3 const& center, std::string const& name, Color const& color) :
		Shape(name, color),
		radius_(std::abs(radius)),
		center_(center) {
	std::cout << "created " << *this << std::endl << std::endl;
}

Sphere::~Sphere() {
	std::cout << "destroyed " << *this << std::endl << std::endl;
}

float Sphere::area() const {
	return 4.0f * PI * pow(radius_, 2);
}

float Sphere::volume() const {
	return 4.0f / 3.0f * PI * std::abs(pow(radius_, 3));
}

//Aufgabe 5.5
std::ostream& Sphere::print(std::ostream &os) const {
	Shape::print(os);
	return os << "radius: " << radius_ << "\ncenter: " << center_ << std::endl;
}

//Aufgabe 5.6
HitPoint Sphere::intersect(Ray const &ray) {
	float distance = 0.0f;

	bool result = glm::intersectRaySphere(
			ray.origin, glm::normalize(ray.direction),
			center_,
			radius_ * radius_,
			distance);

	return HitPoint{result, distance, name_, color_, ray.point(distance), ray.direction};
}
