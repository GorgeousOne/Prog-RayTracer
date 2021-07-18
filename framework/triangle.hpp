#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H
#include "shape.hpp"

class Triangle : public Shape {
public:
	Triangle(
			glm::vec3 const& v0,
			glm::vec3 const& v1,
			glm::vec3 const& v2,
			std::string const& name = "triangle",
			std::shared_ptr<Material> material = {});

	Triangle(
			glm::vec3 const& v0,
			glm::vec3 const& v1,
			glm::vec3 const& v2,
			glm::vec3 const& n,
			std::string const& name = "triangle",
			std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;

	std::ostream& print (std::ostream &os) const override;
	HitPoint intersect(Ray const& ray, float &t) const override;

private:
	glm::vec3 v0_;
	glm::vec3 v1_;
	glm::vec3 v2_;
	glm::vec3 n_;
};


#endif //RAYTRACER_TRIANGLE_H
