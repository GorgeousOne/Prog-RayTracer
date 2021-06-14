#ifndef RAYTRACER_SHAPE_H
#define RAYTRACER_SHAPE_H

class Shape {

public:
	virtual float area() const = 0;
	virtual float volume() const = 0;
};
#endif
