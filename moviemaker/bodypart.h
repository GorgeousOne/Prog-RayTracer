#ifndef RAYTRACER_BODYPART_H
#define RAYTRACER_BODYPART_H

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <iostream>
#include <map>

struct BodyPart {
	std::string name = "undefined";
	glm::vec3 offset{};
	glm::vec3 rotation{};
	std::vector<std::shared_ptr<BodyPart>> children{};
};

struct Pose {
	std::map<std::string, glm::vec3> rotations;
};

class Body {
public:
	Body();
	void apply_pose(Pose const& pose);
	std::shared_ptr<BodyPart> find_part(std::string const& name) const;
	std::ostream& print(std::ostream& os) const;

private:
	glm::vec3 offset_;
	std::map<std::string, std::shared_ptr<BodyPart>> parts_;
};

std::ostream& operator<<(std::ostream& os, BodyPart const& part);
std::ostream& operator<<(std::ostream& os, Body const& body);

#endif //RAYTRACER_BODYPART_H
