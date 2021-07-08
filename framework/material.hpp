
#ifndef RAYTRACER_MATERIAL_HPP
#define RAYTRACER_MATERIAL_HPP
#include <string>
#include "color.hpp"

struct Material {
	std::string name = "default";
	//ambient coefficient
	Color ka = {};
	//diffuse reflection coefficient
	Color kd = {};
	//specular reflection coefficient
	Color ks = {};
	//specular reflection exponent
	float m = 0;
};

std::ostream& operator<<(std::ostream& os, Material const& mat);

#endif
