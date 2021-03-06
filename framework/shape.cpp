#define GLM_FORCE_RADIANS
#include "shape.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

Shape::Shape(std::string const& name, std::shared_ptr<Material> material) :
		name_{name},
		material_{material},
		world_transformation_{ glm::mat4() },
		world_transformation_inv_{ glm::inverse(world_transformation_) } {}

std::string Shape::get_name() const {
	return name_;
}

void Shape::translate(float d_x, float d_y, float d_z) {
	glm::mat4 translation(1);
	translation[3][0] = d_x;
	translation[3][1] = d_y;
	translation[3][2] = d_z;

	world_transformation_ = world_transformation_ * translation;
	world_transformation_inv_ = glm::inverse(world_transformation_);
}

void Shape::rotate(float roll, float pitch, float yaw) {
	glm::mat4 rotation_z(1);
	rotation_z[0][0] =  cosf(roll);
	rotation_z[0][1] =  sinf(roll);
	rotation_z[1][0] = -sinf(roll);
	rotation_z[1][1] =  cosf(roll);

	glm::mat4 rotation_x(1);
	rotation_x[1][1] =  cosf(pitch);
	rotation_x[1][2] =  sinf(pitch);
	rotation_x[2][1] = -sinf(pitch);
	rotation_x[2][2] =  cosf(pitch);

	glm::mat4 rotation_y(1);
	rotation_y[0][0] =  cosf(yaw);
	rotation_y[0][2] = -sinf(yaw);
	rotation_y[2][0] =  sinf(yaw);
	rotation_y[2][2] =  cosf(yaw);

	world_transformation_ = world_transformation_ * rotation_y * rotation_x * rotation_z;
	world_transformation_inv_ = glm::inverse(world_transformation_);
}

void Shape::scale(float scale_x, float scale_y, float scale_z) {
	glm::mat4 scaling(1);
	scaling[0][0] = scale_x;
	scaling[1][1] = scale_y;
	scaling[2][2] = scale_z;

	world_transformation_ = world_transformation_ * scaling;
	world_transformation_inv_ = glm::inverse(world_transformation_);
}

//Aufgabe 5.4
std::ostream& Shape::print(std::ostream &os) const {
	return os << "=== " << name_ << " ===" << "\ncolor:"<< material_;
}

std::ostream& operator<<(std::ostream &os, Shape const& s) {
	return s.print(os);
}