#ifndef RAYTRACER_CUBE_H
#define RAYTRACER_CUBE_H

#include "shape.hpp"

class Box : public Shape {

public:
	Box(glm::vec3 const& min = glm::vec3(0.0), glm::vec3 const& max = glm::vec3(0.0), std::string const& name = "box", std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;
	glm::vec3 min(glm::mat4 transformation) const override;
	glm::vec3 max(glm::mat4 transformation) const override;

	float size_x() const;
	float size_y() const;
	float size_z() const;

	std::ostream& print (std::ostream &os) const override;

	bool intersects_bounds(std::shared_ptr<Shape> const &shape) const;
	bool contains(glm::vec3 const &v) const;
	HitPoint intersect(Ray const& ray) const override;
	bool intersect(Ray const &ray_inv, float &t) const;

private:
	glm::vec3 min_;
	glm::vec3 max_;
	glm::vec3 get_surface_normal(glm::vec3 const& intersection_inv) const;
};
#endif
