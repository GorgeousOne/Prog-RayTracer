#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Shape.h"
#include <glm/vec3.hpp>

class Sphere : public Shape {

public:

	Sphere(float radius = 1.0f, glm::vec3 const& center = glm::vec3(0.0), std::string const& name = "sphere", Color const& color = {}) :
			Shape(name, color),
			radius_(std::abs(radius)),
			center_(center){
	}

	float area() const override;
	float volume() const override;
	std::ostream& print (std::ostream &os) const override;

private:
	float radius_;
	glm::vec3 center_;
};

#endif
