#include "animation.h"
#define PI 3.14159265f

float ease_linear(float percent, float exp) {
	return percent;
};

float ease_cos_in(float percent, float exp) {
	return powf(1.0f - cosf(percent * PI * 0.5f), exp);
};

float ease_sin_in(float percent, float exp) {
	return powf(sinf(percent * PI * 0.5f), exp);
};

std::string get_current_transform(Animation const& animation, float current_time) {
	float progress = (current_time - animation.time.start_time) / animation.time.duration;
	progress = animation.ease(progress, animation.ease_exp);

	glm::vec3 delta = (animation.end - animation.start) * progress;
	glm::vec3 new_state = animation.start + delta;

	std::stringstream current_transform;
	current_transform << "transform " << animation.name << " " << animation.type << " ";
	current_transform << new_state.x << " " << new_state.y << " " << new_state.z;
	return current_transform.str();
}

std::string get_current_cam(CamAnimation const& animation, float current_time) {
	float progress = (current_time - animation.time.start_time) / animation.time.duration;
	progress = animation.ease(progress, animation.ease_exp);

	glm::vec3 position = (animation.pos_end - animation.pos_start) * progress;
	glm::vec3 direction = (animation.dir_end - animation.dir_start) * progress;

	glm::vec3 new_pos = animation.pos_start + position;
	glm::vec3 new_dir = animation.dir_start + direction;

	std::stringstream current_transform;
	current_transform << "define camera eye " << animation.fov << " ";
	current_transform << new_pos.x << " " << new_pos.y << " " << new_pos.z << " ";
	current_transform << new_dir.x << " " << new_dir.y << " " << new_dir.z << " ";
	current_transform << "0 1 0";
	return current_transform.str();
}

Pose read_pose(std::string const& file_path) {
//	std::cout << "reading " << file_path << "\n";
	Pose pose{};
	std::ifstream pose_file_stream(file_path);
	std::string line_buffer;

	while (std::getline(pose_file_stream, line_buffer)) {
		std::string name;
		float pitch;
		float yaw;
		float roll;

		std::istringstream arg_stream(line_buffer);
		arg_stream >> name;
		arg_stream >> pitch >> yaw >> roll;
//		std::cout << pitch << " " << yaw << " " << roll << "\n";
		pose.rotations.insert({name, glm::vec3{pitch, yaw, roll}});
	}
	return pose;
}

void apply_curren_pose(Body& body, BodyAnimation const& animation, float current_time) {
	float progress = (current_time - animation.time.start_time) / animation.time.duration;
	Pose current_pose{};

	for (auto const& entry : animation.pose0.rotations) {
		glm::vec3 rot0 = entry.second;
		glm::vec3 rot1 = animation.pose1.rotations.find(entry.first)->second;
		glm::vec3 current_rotation = (1 - progress) * rot0 + progress * rot1;
		current_pose.rotations.insert({entry.first, current_rotation});
	}
	body.apply_pose(current_pose);
}
