#ifndef RAYTRACER_ANIMATION_H
#define RAYTRACER_ANIMATION_H

#include <sstream>
#include <fstream>
#include <glm/glm.hpp>
#include "bodypart.h"

struct Interval {
	float start_time = 0.0f;
	float duration = 1.0f;

	bool is_active(float current_time) const {
		return current_time >= start_time - .001f && current_time < start_time + duration + .001f;
	}
};

struct Animation {
	std::string name = "undefined";
	std::string type = "translate";
	Interval time{};
	float start1 = 0;
	float start2 = 0;
	float start3 = 0;
	float end1 = start1;
	float end2 = start2;
	float end3 = start3;
};

struct CamAnimation {
	Interval time{};
	glm::vec3 pos_start{};
	glm::vec3 dir_start{};
	glm::vec3 up_start{};
	glm::vec3 pos_end = pos_start;
	glm::vec3 dir_end = dir_start;
	glm::vec3 up_end = up_start;
	float fov = 60.0f;
};

struct BodyAnimation {
	Pose pose0;
	Pose pose1;
	Interval time{};
};

std::string get_current_transform(Animation const& animation, float current_time);

std::string get_current_cam(CamAnimation const& cam, float current_time);

Pose read_pose(std::string const& file_path);


void apply_curren_pose(Body& body, BodyAnimation const& animation, float current_time);

#endif //RAYTRACER_ANIMATION_H
