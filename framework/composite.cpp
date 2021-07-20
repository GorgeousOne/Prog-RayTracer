#include <numeric>
#include "composite.hpp"

Composite::Composite(std::string const &name, std::shared_ptr<Material> material) :
	Shape(name, material), min_{glm::vec3 {}}, max_{glm::vec3 {}} {}

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

glm::vec3 Composite::min() const {
	return min_;
}

glm::vec3 Composite::max() const {
	return max_;
}

std::ostream &Composite::print(std::ostream &os) const {
	Shape::print(os);

	for (auto const& child : children_) {
		os << child;
	}
	return os;
}

HitPoint Composite::intersect(Ray const &ray) const {
	Box bounds = Box {min(), max()};
	HitPoint bound_hit = bounds.intersect(ray);

	if (!bound_hit.does_intersect) {
		return bound_hit;
	}

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

void Composite::add_child(std::shared_ptr<Shape> shape) {

	if (children_.empty()) {
		min_ = shape->min();
		max_ = shape->max();
	}else {
		glm::vec3 child_min = shape->min();
		glm::vec3 child_max = shape->max();
		min_.x = std::min(min_.x, child_min.x);
		min_.y = std::min(min_.y, child_min.y);
		min_.z = std::min(min_.z, child_min.z);
		max_.x = std::max(max_.x, child_max.x);
		max_.y = std::max(max_.y, child_max.y);
		max_.z = std::max(max_.z, child_max.z);
	}
	children_.push_back(shape);
}