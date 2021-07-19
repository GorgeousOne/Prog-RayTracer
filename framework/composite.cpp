#include <numeric>
#include "composite.hpp"

Composite::Composite(std::string const &name, std::shared_ptr<Material> material) : Shape(name, material) {}

float Composite::area() const {
	float area_sum = 0;
	for (auto child : children_) {
		area_sum += child->area();
	}
	return area_sum;
}

float Composite::volume() const {
	float volume_sum = 0;
	for (auto child : children_) {
		volume_sum += child->volume();
	}
	return volume_sum;
}

std::ostream &Composite::print(std::ostream &os) const {
	Shape::print(os);

	for (auto const& child : children_) {
		os << child;
	}
	return os;
}

HitPoint Composite::intersect(Ray const &ray) const {
	float min_t = -1;
	HitPoint min_hit {};

	for (auto const& child : children_) {
		HitPoint hit = child->intersect(ray);

		if (!hit.does_intersect) {
			continue;
		}
		if (!min_hit.does_intersect || hit.distance < min_t) {
			min_t = hit.distance;
			min_hit = hit;
		}
	}
	return min_hit;
}

Composite& Composite::add_child(std::shared_ptr<Shape> shape) {
	children_.push_back(shape);
	return *this;
}
