#include "printVec3.hpp"

std::ostream& operator<<(std::ostream &os, glm::vec3 const& v) {
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}