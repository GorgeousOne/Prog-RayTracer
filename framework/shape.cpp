#include "shape.hpp"

Shape::Shape(std::string const& name, std::shared_ptr<Material> material) :
		name_{name},
		material_{material} {}

std::string Shape::get_name() const {
	return name_;
}

//Aufgabe 5.4
std::ostream& Shape::print(std::ostream &os) const {
	return os << "=== " << name_ << " ===" << "\ncolor:"<< material_;
}

std::ostream& operator<<(std::ostream &os, Shape const& s) {
	return s.print(os);
}