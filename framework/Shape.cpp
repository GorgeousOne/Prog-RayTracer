#include "Shape.h"

Shape::Shape(std::string const& name, Color const &color) :
		name_{name},
		color_{color} {}

// Aufgabe 5.4
std::ostream& Shape::print(std::ostream &os) const {
	os << "(name:" << name_ << ",color:"<< color_ << ")" << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, Shape const& s) {
	return s.print(os);
}

