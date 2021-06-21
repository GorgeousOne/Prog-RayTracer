#ifndef RAYTRACER_CUBE_H
#define RAYTRACER_CUBE_H

#include "Shape.h"
#include <glm/vec3.hpp>

class Box : public Shape {

public:
	Box(glm::vec3 const& min = glm::vec3(0.0), glm::vec3 const& max = glm::vec3(0.0), std::string const& name = "box", Color const& color = {}) :
			Shape(name, color),
			min_(min),
			max_(max) {
		assert(min.x <= max.x);
		assert(min.y <= max.y);
		assert(min.z <= max.z);
	}

	float area() const override;
	float volume() const override;

	float size_x() const;
	float size_y() const;
	float size_z() const;
	std::ostream& print (std::ostream &os) const override;

private:
	glm::vec3 min_;
	glm::vec3 max_;
};
#endif
