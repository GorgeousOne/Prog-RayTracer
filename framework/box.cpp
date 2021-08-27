#include "box.hpp"

#define EPSILON 0.001f

Box::Box(
		glm::vec3 const& min,
		glm::vec3 const& max,
		std::string const& name,
		std::shared_ptr<Material> material) :
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

glm::vec3 Box::min(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 min_trans = transform_vec3(transformation, min_, true);
	min_trans = glm::min(min_trans, transform_vec3(transformation, {min_.x, min_.y, max_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, {max_.x, min_.y, max_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, {min_.x, max_.y, max_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, {max_.x, max_.y, min_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, {max_.x, min_.y, min_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, {min_.x, max_.y, min_.z}, true));
	min_trans = glm::min(min_trans, transform_vec3(transformation, max_, true));
	return min_trans;
}

glm::vec3 Box::max(glm::mat4 transformation) const {
	transformation *= world_transformation_;
	glm::vec3 max_trans = transform_vec3(transformation, min_, true);
	max_trans = glm::max(max_trans, transform_vec3(transformation, {min_.x, min_.y, max_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, {max_.x, min_.y, max_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, {min_.x, max_.y, max_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, {max_.x, max_.y, min_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, {max_.x, min_.y, min_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, {min_.x, max_.y, min_.z}, true));
	max_trans = glm::max(max_trans, transform_vec3(transformation, max_, true));
	return max_trans;
}

std::ostream &Box::print(std::ostream &os) const {
	Shape::print(os);
	return os << "\nmin:" << min_ << "\nmax: " << max_ << std::endl;
}

bool Box::intersects_bounds(std::shared_ptr<Shape> const& shape) const {
	glm::vec3 min = shape->min(glm::mat4(1));
	glm::vec3 max = shape->max(glm::mat4(1));
	return
		(min.x >= min_.x && min.x <= max_.x || max.x >= min_.x && max.x <= max_.x || min.x < min_.x && max.x > max_.x) &&
		(min.y >= min_.y && min.y <= max_.y || max.y >= min_.y && max.y <= max_.y || min.y < min_.y && max.y > max_.y) &&
		(min.z >= min_.z && min.z <= max_.z || max.z >= min_.z && max.z <= max_.z || min.z < min_.z && max.z > max_.z);
}

bool Box::contains(glm::vec3 const& v) const{
	return
		v.x >= min_.x && v.x <= max_.x &&
		v.y >= min_.y && v.y <= max_.y &&
		v.z >= min_.z && v.z <= max_.z;
}

HitPoint Box::intersect(Ray const& ray) const {
	float t;
	Ray ray_inv = transform_ray(world_transformation_inv_, ray);
	bool does_intersect = intersect(ray_inv, t);

	if (!does_intersect) {
		return HitPoint{};
	}else {
		//calculate the intersection point with found t
		glm::vec3 surface_normal_inv = get_surface_normal(ray_inv.point(t));
		glm::vec3 surface_normal = transform_vec3(world_transformation_, surface_normal_inv);
		return HitPoint{true, t, name_, material_, ray.point(t), ray.direction, glm::normalize(surface_normal)};
	}
}

//https://tavianator.com/2011/ray_box.html
bool Box::intersect(Ray const& ray_inv, float &t) const {
	//furthest entering position with a box plane
	float t_min = -std::numeric_limits<float>::infinity();
	//closest exiting position with a box plane
	float t_max = std::numeric_limits<float>::infinity();

	if (0 == ray_inv.direction.x) {
		//returns no position if the ray runs parallel on yz-plane to the box
		if (ray_inv.origin.x < min_.x || ray_inv.origin.x > max_.x) {
			return false;
		}
	//checks if the intersections of the ray with the min & max yz-planes of the box
	// are closer to the surface of the box than any previous position with a min / max plane
	} else {
		float ray_dir_inv_x = 1 / ray_inv.direction.x;
		float tx1 = (min_.x - ray_inv.origin.x) * ray_dir_inv_x;
		float tx2 = (max_.x - ray_inv.origin.x) * ray_dir_inv_x;
		//updates the entering position, if the new position is further away from ray origin (aka closer to box)
		t_min = std::max(t_min, std::min(tx1, tx2));
		//updates the exiting position, if the new position is closer to ray origin (aka also closer to box)
		t_max = std::min(t_max, std::max(tx1, tx2));
	}
	if (0 == ray_inv.direction.y) {
		if (ray_inv.origin.y < min_.y || ray_inv.origin.y > max_.y) {
			return false;
		}
	} else {
		float ray_dir_inv_y = 1 / ray_inv.direction.y;
		float ty1 = (min_.y - ray_inv.origin.y) * ray_dir_inv_y;
		float ty2 = (max_.y - ray_inv.origin.y) * ray_dir_inv_y;
		t_min = std::max(t_min, std::min(ty1, ty2));
		t_max = std::min(t_max, std::max(ty1, ty2));
	}
	if (0 == ray_inv.direction.z) {
		if (ray_inv.origin.z < min_.z || ray_inv.origin.z > max_.z) {
			return false;
		}
	} else {
		float ray_dir_inv_z = 1 / ray_inv.direction.z;
		float tz1 = (min_.z - ray_inv.origin.z) * ray_dir_inv_z;
		float tz2 = (max_.z - ray_inv.origin.z) * ray_dir_inv_z;
		t_min = std::max(t_min, std::min(tz1, tz2));
		t_max = std::min(t_max, std::max(tz1, tz2));
	}
	//returns no position, if the ray misses the box
	if (t_max < t_min) {
		return false;
	}
	t;
	//ensures that no position in negative ray direction get returned
	if (t_min > 0) {
		t = t_min;
	//returns max t if ray is inside box
	} else if (t_max > 0) {
		t = t_max;
	} else {
		return false;
	}
	t -= EPSILON;
	return true;
}

glm::vec3 Box::get_surface_normal(glm::vec3 const& intersection_inv) const {
	if (intersection_inv.x <= min_.x) {
		return glm::vec3{-1, 0, 0};
	} else if (intersection_inv.y <= min_.y) {
		return glm::vec3{0, -1, 0};
	} else if (intersection_inv.z <= min_.z) {
		return glm::vec3{0, 0, -1};
	} else if (intersection_inv.x >= max_.x) {
		return glm::vec3{1, 0, 0};
	} else if (intersection_inv.y >= max_.y) {
		return glm::vec3{0, 1, 0};
	} else {
		return glm::vec3{0, 0, 1};
	}
}

