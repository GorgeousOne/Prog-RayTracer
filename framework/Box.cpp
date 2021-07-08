#include "box.hpp"

Box::Box(glm::vec3 const& min, glm::vec3 const& max, std::string const& name, std::shared_ptr<Material> material) :
		Shape(name, material),
		min_(min),
		max_(max) {

	if (min.x > max.x || min.y > max.y || min.z > max.z) {
		throw "Box minimum cannot be greater than maximum";
	}
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
	Shape::print(os);
	return os << "min:" << min_ << "\nmax: " << max_ << std::endl;
}

HitPoint Box::intersect(Ray const &ray, float &t) const {
	return HitPoint();
}
