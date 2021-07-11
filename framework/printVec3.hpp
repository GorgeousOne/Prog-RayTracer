#ifndef RAYTRACER_PRINTVEC3_HPP
#define RAYTRACER_PRINTVEC3_HPP

#include <iostream>
#include <string>
#include <glm/glm.hpp>

std::ostream& operator<<(std::ostream &os, glm::vec3 const& v);

#endif
