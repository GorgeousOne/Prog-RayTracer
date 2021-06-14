#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Shape.h"
#include <glm/glm.hpp>

class Sphere : public Shape {

public:

	Sphere(float radius = 1.0f, glm::vec3 const& center = glm::vec3(0.0)) :
			radius_(radius),
			center_(center){}

	float area() const override;

	float volume() const override;

private:
	float radius_;
	glm::vec3 center_;
};


#endif
