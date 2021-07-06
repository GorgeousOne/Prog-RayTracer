#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Shape.hpp"
#include "Ray.hpp"
#include "HitPoint.hpp"
#include <glm/vec3.hpp>

class Sphere : public Shape {

public:

	Sphere(float radius = 1.0f, glm::vec3 const& center = glm::vec3(0.0), std::string const& name = "sphere", Color const& color = {});
	~Sphere() override;

	float area() const override;
	float volume() const override;
	std::ostream& print(std::ostream &os) const override;

	HitPoint intersect(Ray const& ray, float &t) const override;

private:
	float radius_;
	glm::vec3 center_;
};

#endif
