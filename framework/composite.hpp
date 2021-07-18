#ifndef RAYTRACER_COMPOSITE_H
#define RAYTRACER_COMPOSITE_H

#include <vector>
#include "shape.hpp"

class Composite : public Shape {
public:
	Composite(std::string const& name = "sphere", std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;
	std::ostream& print(std::ostream &os) const override;
	HitPoint intersect(Ray const& ray) const override;

	Composite& add_child(std::shared_ptr<Shape> shape);

private:
	std::vector<std::shared_ptr<Shape>> children_;
};


#endif //RAYTRACER_COMPOSITE_H
