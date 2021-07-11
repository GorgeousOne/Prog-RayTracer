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
	virtual HitPoint intersect(Ray const& ray, float &t) const = 0;

	// Aufgabe 5.4
	virtual std::ostream& print (std::ostream& os) const;

protected:
	std::string name_;
	std::shared_ptr<Material> material_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

#endif
