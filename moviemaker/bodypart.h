#ifndef RAYTRACER_BODYPART_H
#define RAYTRACER_BODYPART_H

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <iostream>

struct BodyPart {
	std::string name = "undefined";
	glm::vec3 offset{};
	glm::vec3 rotation{};
	std::vector<std::shared_ptr<BodyPart>> children{};
};

std::ostream& operator<<(std::ostream& os, BodyPart const& part);


#endif //RAYTRACER_BODYPART_H
