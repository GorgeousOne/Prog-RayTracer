
#ifndef RAYTRACER_MATERIAL_HPP
#define RAYTRACER_MATERIAL_HPP
#include <string>
#include <glm/glm.hpp>
#include "printVec3.hpp"

//Aufgabe 6.4
struct Material {
	std::string name = "default";
	//ambient color reflection coefficient
	glm::vec3 ka = {0.2, 0.2, 0.2};
	//diffuse color reflection coefficient
	glm::vec3 kd = {0.5, 0.5, 0.5};
	//specular reflection coefficient
	glm::vec3 ks = {};
	//specular reflection exponent
	float m = 0;
	//glossiness coefficient
	float glossy = 0.01;

	float opacity = 1;
	//index of refraction
	float ior = 1;
};

std::ostream& operator<<(std::ostream& os, Material const& mat);

#endif
