#include <numeric>
#include <iomanip>
#include "composite.hpp"

#define EPSILON 0.001f

Composite::Composite(std::string const &name, std::shared_ptr<Material> material) :
	Shape(name, material), bounds_{nullptr} {}

Composite::Composite(std::shared_ptr<Box> bounds, std::string const &name, std::shared_ptr<Material> material) :
	Shape(name, material), bounds_{bounds} {}

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
	if (nullptr != bounds_) {
		return bounds_->min();
	}
	glm::vec3 min{};

	for (auto child : children_) {
		glm::vec3 child_min = child->min();
		min.x = std::min(min.x, child_min.x);
		min.y = std::min(min.y, child_min.y);
		min.z = std::min(min.z, child_min.z);
	}
	return min;
}

glm::vec3 Composite::max() const {
	if (nullptr != bounds_) {
		return bounds_->max();
	}
	glm::vec3 max{};

	for (auto child : children_) {
		glm::vec3 child_max = child->max();
		max.x = std::max(max.x, child_max.x);
		max.y = std::max(max.y, child_max.y);
		max.z = std::max(max.z, child_max.z);
	}
	return max;
}

std::ostream &Composite::print(std::ostream &os) const {
	Shape::print(os);

	for (auto const& child : children_) {
		os << child;
	}
	return os;
}

HitPoint Composite::intersect(Ray const &ray) const {
	float t;
	bool bounds_hit = bounds_->intersect(ray, t);

	//test if bounding box is being intersected
	if (!bounds_hit) {
		return {};
	}
	float min_t = -1;
	HitPoint min_hit {};

	//find closest child that the ray intersects with
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
	children_.push_back(shape);
}

unsigned Composite::child_count() {
	return children_.size();
}

void Composite::build_octree() {
	if (nullptr == bounds_) {
		bounds_ = std::make_shared<Box>(min(), max());
	}
	if (children_.size() <= 64) {
		return;
	}
	glm::vec3 oct_size = (max() - min()) * 0.5f;
	std::vector<std::shared_ptr<Composite>> subdivisions;

	//create 8 smaller boxes
	for (int x = 0; x < 2; ++x) {
		for (int y = 0; y < 2; ++y) {
			for (int z = 0; z < 2; ++z) {
				glm::vec3 oct_min = min() + glm::vec3 {x * oct_size.x, y * oct_size.y, z * oct_size.z};
				glm::vec3 oct_max = min() + glm::vec3 {(x + 1) * oct_size.x, (y + 1) * oct_size.y, (z + 1) * oct_size.z};
				subdivisions.push_back(std::make_shared<Composite>(Composite{std::make_shared<Box>(Box{oct_min, oct_max})}));
			}
		}
	}
	for (auto oct : subdivisions) {
		for (auto child : children_) {
			if (oct->bounds_->intersects_bounds(child)) {
				oct->add_child(child);
			}
		}
	}
	//stop subdividing if subdivisions have same amount of children
	for (auto oct : subdivisions) {
		if (oct->children_.size() == children_.size()) {
			return;
		}
	}
	children_.clear();

	for (auto oct : subdivisions) {
		if (oct->children_.size() != 0) {
			oct->build_octree();
			children_.push_back(oct);
		}
	}
}
