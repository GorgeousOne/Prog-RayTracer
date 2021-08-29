#include <numeric>
#include <iomanip>
#include "composite.hpp"

#define EPSILON 0.001f

Composite::Composite(std::string const &name, std::shared_ptr<Material> material) :
	Shape(name, material), bounds_{nullptr} {}

Composite::Composite(glm::vec3 const& min, glm::vec3 const& max, std::string const &name, std::shared_ptr<Material> material) :
	Shape(name, material), bounds_{std::make_shared<Box>(min, max)} {}

float Composite::area() const {
	float area_sum = 0;
	for (auto const& child : children_) {
		area_sum += child.second->area();
	}
	return area_sum;
}

float Composite::volume() const {
	float volume_sum = 0;

	for (auto const& child : children_) {
		volume_sum += child.second->volume();
	}
	return volume_sum;
}

glm::vec3 Composite::min(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 min{};
	bool is_first = true;

	for (auto const& child : children_) {
		glm::vec3 child_min = child.second->min(transformation);

		if (is_first) {
			min = child_min;
			is_first = false;
		}else {
			min = glm::min(min, child_min);
		}
	}
	return min;
}

glm::vec3 Composite::max(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 max{};
	bool is_first = true;

	for (auto const& child : children_) {
		glm::vec3 child_max = child.second->max(transformation);

		if (is_first) {
			max = child_max;
			is_first = false;
		}else {
			max = glm::max(max, child_max);
		}
	}
	return max;
}

std::ostream &Composite::print(std::ostream &os) const {
	Shape::print(os);

	for (auto const& child : children_) {
		os << child.second;
	}
	return os;
}

void Composite::scale(float sx, float sy, float sz) {
	Shape::scale(sx, sy, sz);
	build_octree();
}

void Composite::rotate(float yaw, float pitch, float roll) {
	Shape::rotate(yaw, pitch, roll);
	build_octree();
}

void Composite::translate(float x, float y, float z) {
	Shape::translate(x, y, z);
	build_octree();
}

HitPoint Composite::intersect(Ray const& ray) const {
	assert(nullptr != bounds_);
	float t;
	bool bounds_hit = bounds_->intersect(ray, t);

//	test if bounding box is being intersected
	if (!bounds_hit) {
		return {};
	}
	HitPoint min_hit {};
	Ray ray_inv = transform_ray(world_transformation_inv_, ray);

	//find closest child that the ray_inv intersects with
	for (auto const& child : children_) {
		HitPoint hit = child.second->intersect(ray_inv);

		if (hit.does_intersect && (!min_hit.does_intersect || hit.distance < min_hit.distance)) {
			min_hit = hit;
		}
	}
	if (min_hit.does_intersect) {
		min_hit.position = transform_vec3(world_transformation_, min_hit.position, true);
		min_hit.surface_normal = glm::normalize(transform_vec3(world_transformation_, min_hit.surface_normal));
		min_hit.ray_direction = ray.direction;
	}
	return min_hit;
}

void Composite::add_child(std::shared_ptr<Shape> shape) {
	children_.emplace(shape->get_name(), shape);
}

void Composite::remove_child(std::string const& name) {
	children_.erase(name);
}

std::shared_ptr<Shape> Composite::find_child(std::string const& name) {
	auto it = children_.find(name);
	return children_.end() == it ? nullptr : it->second;
}

unsigned Composite::child_count() {
	return children_.size();
}

void Composite::build_octree() {
	bounds_ = std::make_shared<Box>(min(glm::mat4(1)), max(glm::mat4(1)));
//	bounds_ = std::make_shared<Box>(min(glm::mat4(1)), max(glm::mat4(1)), "bounds", std::make_shared<Material>());

	if (children_.size() <= 64) {
		return;
	}
	glm::vec3 oct_size = (max(glm::mat4(1)) - min(glm::mat4(1))) * 0.5f;
	std::vector<std::shared_ptr<Composite>> subdivisions;

	//create 8 smaller boxes
	for (int x = 0; x < 2; ++x) {
		for (int y = 0; y < 2; ++y) {
			for (int z = 0; z < 2; ++z) {
				glm::vec3 oct_min = min(glm::mat4(1)) + glm::vec3 {x * oct_size.x, y * oct_size.y, z * oct_size.z};
				glm::vec3 oct_max = min(glm::mat4(1)) + glm::vec3 {(x + 1) * oct_size.x, (y + 1) * oct_size.y, (z + 1) * oct_size.z};
				subdivisions.push_back(std::make_shared<Composite>(Composite{oct_min, oct_max, std::to_string(x + 2*y + 4*z)}));
			}
		}
	}
	for (auto const& oct : subdivisions) {
		for (auto const&  child : children_) {
			if (oct->bounds_->intersects_bounds(child.second)) {
				oct->add_child(child.second);
			}
		}
	}
	//stop subdividing if subdivisions have same amount of children
	for (auto const&  oct : subdivisions) {
		if (oct->children_.size() == children_.size()) {
			return;
		}
	}
	children_.clear();

	for (auto const& oct : subdivisions) {
		if (oct->child_count() > 0) {
			oct->build_octree();
			children_.emplace(oct->get_name(), oct);
		}
	}
}