#include "Sphere.h"
#include <cmath>   //pow

#define PI 3.14159265f

float Sphere::area() const {
	return 4.0f * PI * pow(radius_, 2);
}

float Sphere::volume() const {
	return 4.0f / 3.0f * PI * pow(radius_, 3);
}
