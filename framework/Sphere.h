#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Shape.h"
#include "Ray.h"
#include "HitPoint.h"
#include <glm/vec3.hpp>

class Sphere : public Shape {

public:

	Sphere(float radius = 1.0f, glm::vec3 const& center = glm::vec3(0.0), std::string const& name = "sphere", Color const& color = {});

	float area() const override;
	float volume() const override;
	std::ostream& print(std::ostream &os) const override;

	HitPoint intersect(Ray const& ray);

private:
	float radius_;
	glm::vec3 center_;
};

#endif
