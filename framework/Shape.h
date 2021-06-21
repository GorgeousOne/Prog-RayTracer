#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H

#include <string>
#include "color.hpp"

class Shape {

public:

	Shape(std::string const& name, Color const &color);
	virtual float area() const = 0;
	virtual float volume() const = 0;

	// Aufgabe 5.4
	virtual std::ostream& print (std::ostream& os) const;

private:
	std::string name_;
	Color color_;
};

std::ostream& operator<<(std::ostream& os, Shape const& s);

#endif
