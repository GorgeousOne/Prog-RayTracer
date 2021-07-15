
#ifndef RAYTRACER_MATERIAL_HPP
#define RAYTRACER_MATERIAL_HPP
#include <string>
#include <glm/glm.hpp>
#include "printVec3.hpp"

//Aufgabe 6.4
struct Material {
	std::string name = "default";
	//ambient coefficient
	glm::vec3 ka = {};
	//diffuse_color reflection coefficient
	glm::vec3 kd = {};
	//specular reflection coefficient
	glm::vec3 ks = {};
	//specular reflection exponent
	float m = 0;
};

std::ostream& operator<<(std::ostream& os, Material const& mat);

#endif
