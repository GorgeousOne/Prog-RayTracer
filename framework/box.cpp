#include "box.hpp"

Box::Box(glm::vec3 const& min, glm::vec3 const& max, std::string const& name, std::shared_ptr<Material> material) :
		Shape(name, material),
		min_(min),
		max_(max) {

	if (min.x > max_.x || min.y > max_.y || min.z > max_.z) {
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

std::ostream &Box::print(std::ostream &os) const {
	Shape::print(os);
	return os << "min:" << min_ << "\nmax: " << max_ << std::endl;
}

HitPoint Box::intersect(Ray const& ray, float &t) const {
	float t_min = -std::numeric_limits<float>::infinity();
	float t_max = std::numeric_limits<float>::infinity();

	if (0 != ray.direction.x) {
		float ray_dir_inv_x = 1 / ray.direction.x;
		float tx1 = (min_.x - ray.origin.x) * ray_dir_inv_x;
		float tx2 = (max_.x - ray.origin.x) * ray_dir_inv_x;
		t_min = std::max(t_min, std::min(tx1, tx2));
		t_max = std::min(t_max, std::max(tx1, tx2));
	}
	if (0 != ray.direction.y) {
		float ray_dir_inv_y = 1 / ray.direction.y;
		float ty1 = (min_.y - ray.origin.y) * ray_dir_inv_y;
		float ty2 = (max_.y - ray.origin.y) * ray_dir_inv_y;
		t_min = std::max(t_min, std::min(ty1, ty2));
		t_max = std::min(t_max, std::max(ty1, ty2));
	}
	if (0 != ray.direction.z) {
		float ray_dir_inv_z = 1 / ray.direction.z;
		float tz1 = (min_.z - ray.origin.z) * ray_dir_inv_z;
		float tz2 = (max_.z - ray.origin.z) * ray_dir_inv_z;
		t_min = std::max(t_min, std::min(tz1, tz2));
		t_max = std::min(t_max, std::max(tz1, tz2));
	}
	if (t_max < t_min) {
		return HitPoint{};
	}
	//ensures that intersections in negative ray direction do not get selected
	if (t_min > 0) {
		t = t_min;
	} else if (t_max > 0) {
		t = t_max;
	} else {
		return HitPoint{};
	}
	return HitPoint{true, t, name_, material_, ray.point(t), ray.direction};
}