#ifndef RAYTRACER_COMPOSITE_H
#define RAYTRACER_COMPOSITE_H

#include <vector>
#include <map>
#include "shape.hpp"
#include "box.hpp"

class Composite : public Shape {
public:
	Composite(std::string const& name = "composite", std::shared_ptr<Material> material = {});
	Composite(glm::vec3 const& min, glm::vec3 const& max, std::string const& name = "composite", std::shared_ptr<Material> material = {});

	float area() const override;
	float volume() const override;
	glm::vec3 min(glm::mat4 transformation) const override;
	glm::vec3 max(glm::mat4 transformation) const override;

	virtual void scale(float sx, float sy, float sz) override;
	virtual void rotate(float yaw, float pitch, float roll) override;
	virtual void translate(float x, float y, float z) override;

	std::ostream& print(std::ostream &os) const override;
	HitPoint intersect(Ray const& ray_inv) const override;

	void add_child(std::shared_ptr<Shape> shape);
	void build_octree();
	unsigned int child_count();

private:
	std::shared_ptr<Box> bounds_;
	std::map<std::string, std::shared_ptr<Shape>> children_;
};

#endif //RAYTRACER_COMPOSITE_H
