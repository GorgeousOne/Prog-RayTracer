#include <numeric>
#include "composite.hpp"

Composite::Composite(std::string const &name, std::shared_ptr<Material> material) : Shape(name, material) {}

float Composite::area() const {
	auto lambda = [] (std::shared_ptr<Shape> const& child) -> float {return child->area();};
	return std::accumulate(children_.begin(), children_.end(), 0.0, lambda);
}

float Composite::volume() const {
	auto lambda = [] (std::shared_ptr<Shape> const& child) -> float {return child->volume();};
	return std::accumulate(children_.begin(), children_.end(), 0.0, lambda);
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
