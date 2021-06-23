#include "Shape.h"

Shape::Shape(std::string const& name, Color const &color) :
		name_{name},
		color_{color} {
	std::cout << "created " << *this << std::endl << std::endl;
}

Shape::~Shape() {
	std::cout << "destroyed " << *this << std::endl << std::endl;
}

//Aufgabe 5.4
std::ostream& Shape::print(std::ostream &os) const {
	return os << "=== " << name_ << " ===" << "\ncolor:"<< color_;
}

std::ostream& operator<<(std::ostream &os, Shape const& s) {
	return s.print(os);
}

std::ostream& operator<<(std::ostream &os, glm::vec3 const& v) {
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

