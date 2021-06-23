#include "Box.h"

Box::Box(glm::vec3 const& min, glm::vec3 const& max, std::string const& name, Color const& color) :
		Shape(name, color),
		min_(min),
		max_(max) {
	assert(min.x <= max.x);
	assert(min.y <= max.y);
	assert(min.z <= max.z);
}

float Box::size_x() const {
	return max_.x - min_.x;
}

float Box::size_y() const {
	return max_.y - min_.y;
}

float Box::size_z() const {
	return max_.z - min_.z;
}

float Box::area() const {
	return 2 * size_x() * size_y() +
	       2 * size_y() * size_z() +
	       2 * size_z() * size_x();
}

float Box::volume() const {
	return size_x() * size_y() * size_z();
}

std::ostream& Box::print(std::ostream &os) const {
	return Shape::print(os);
}
