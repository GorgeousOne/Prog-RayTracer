#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H

#include <string>
#include "color.hpp"
#include <glm/vec3.hpp>

class Shape {

public:

	Shape(std::string const& name, Color const &color);
	virtual ~Shape();

	virtual float area() const = 0;
	virtual float volume() const = 0;

	// Aufgabe 5.4
	virtual std::ostream& print (std::ostream& os) const;

protected:
	std::string name_;
	Color color_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

std::ostream& operator<<(std::ostream &os, glm::vec3 const& v);
#endif
