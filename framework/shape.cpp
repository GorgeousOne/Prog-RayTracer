#include "shape.hpp"
#include "glm/gtx/transform.hpp"
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
	translation[3][0] += d_x;
	translation[3][1] += d_y;
	translation[3][2] += d_z;
//	std::cout << glm::to_string(translation) << std::endl;
//	world_transformation_ = world_transformation_ * translation;
	world_transformation_ = glm::translate(world_transformation_, { d_x, d_y, d_z });
	world_transformation_inv_ = glm::inverse(world_transformation_);

	std::cout << glm::to_string(world_transformation_) << std::endl;
//	std::cout << glm::to_string() << std::endl;
}

void Shape::rotate(float roll, float pitch, float yaw) {

}

void Shape::scale(float scale_x, float scale_y, float scale_z) {

}

//Aufgabe 5.4
std::ostream& Shape::print(std::ostream &os) const {
	return os << "=== " << name_ << " ===" << "\ncolor:"<< material_;
}

std::ostream& operator<<(std::ostream &os, Shape const& s) {
	return s.print(os);
}