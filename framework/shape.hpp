#ifndef RAYTRACER_SHAPE_HPP
#define RAYTRACER_SHAPE_HPP

#include <string>
#include "color.hpp"
#include <glm/glm.hpp>
#include <memory>
#include "hitPoint.hpp"
#include "ray.hpp"
#include "material.hpp"
#include "printVec3.hpp"

class Shape {

public:
	Shape(std::string const& name, std::shared_ptr<Material> material);

	virtual std::string get_name() const;

	virtual float area() const = 0;
	virtual float volume() const = 0;
	virtual glm::vec3 min() const = 0;
	virtual glm::vec3 max() const = 0;
	virtual HitPoint intersect(Ray const& ray) const = 0;

	virtual void translate(float d_x, float d_y, float d_z);
	virtual void rotate(float roll, float pitch, float yaw);
	virtual void scale(float scale_x, float scale_y, float scale_z);

	virtual std::ostream& print (std::ostream& os) const;

protected:
	std::string name_;
	std::shared_ptr<Material> material_;
	glm::mat4 world_transformation_;
	glm::mat4 world_transformation_inv_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

#endif
