#ifndef RAYTRACER_COMPOSITE_H
#define RAYTRACER_COMPOSITE_H

#include <vector>
#include "shape.hpp"
#include "box.hpp"

class Composite : public Shape {
public:
	Composite(std::string const& name = "composite", std::shared_ptr<Material> material = {});
	Composite(std::shared_ptr<Box> bounds, std::string const& name = "composite", std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;
	glm::vec3 min(glm::mat4 transformation) const override;
	glm::vec3 max(glm::mat4 transformation) const override;

	std::ostream& print(std::ostream &os) const override;
	HitPoint intersect(Ray const& ray) const override;

	void add_child(std::shared_ptr<Shape> shape);
	void build_octree();
	unsigned int child_count();

private:
	std::shared_ptr<Box> bounds_;
	std::vector<std::shared_ptr<Shape>> children_;
};

#endif //RAYTRACER_COMPOSITE_H
