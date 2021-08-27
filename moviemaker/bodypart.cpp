#include "bodypart.h"

std::ostream &operator<<(std::ostream &os, BodyPart const& part) {
	for (auto const& child : part.children) {
		os << *child;
	}
	os << "define shape obj " << part.name << std::endl;

	if (!part.children.empty()) {
		os << "add " << part.name;

		for (auto const &child: part.children) {
			os << " " << child->name;
		}
		os << std::endl;
	}
	os << "transform " << part.name << " translate " << part.offset.x << " " << part.offset.y << " " << part.offset.z << std::endl;
	os << "transform " << part.name << " rotate " << part.rotation.x << " " << part.rotation.y << " " << part.rotation.z << std::endl;
	os << std::endl;
	return os;
}
