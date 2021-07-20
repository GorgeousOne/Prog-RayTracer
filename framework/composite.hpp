#ifndef RAYTRACER_COMPOSITE_H
#define RAYTRACER_COMPOSITE_H

#include <vector>
#include "shape.hpp"
#include "box.hpp"

class Composite : public Shape {
public:
	Composite(std::string const& name = "composite", std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;
	glm::vec3 min() const override;
	glm::vec3 max() const override;

	std::ostream& print(std::ostream &os) const override;
	HitPoint intersect(Ray const& ray) const override;

	void add_child(std::shared_ptr<Shape> shape);

private:
	glm::vec3 min_;
	glm::vec3 max_;
	std::vector<std::shared_ptr<Shape>> children_;
};


#endif //RAYTRACER_COMPOSITE_H
