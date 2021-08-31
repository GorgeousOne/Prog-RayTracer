#ifndef RAYTRACER_ANIMATION_H
#define RAYTRACER_ANIMATION_H

#include <sstream>
#include <fstream>
#include <glm/glm.hpp>
#include <functional>
#include "bodypart.h"
#define PI 3.14159265f

float ease_linear(float percent, float exp);

float ease_cos_in(float percent, float exp);

float ease_cos_out(float percent, float exp);

struct Interval {
	float start_time = 0.0f;
	float duration = 1.0f;

	bool is_active(float current_time) const {
		return current_time >= start_time - .01f && current_time < start_time + duration + .01f;
	}
};

struct Animation {
	std::string name = "undefined";
	std::string type = "translate";
	Interval time{};
	glm::vec3 start{};
	glm::vec3 end = start;
};

struct CamAnimation {
	Interval time{};
	glm::vec3 pos_start{};
	glm::vec3 dir_start{};
	glm::vec3 pos_end = pos_start;
	glm::vec3 dir_end = dir_start;
	std::function<float(float, float)> ease = ease_linear;
	float ease_exp = 1;
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
