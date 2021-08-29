#include "animation.h"

std::string get_current_transform(Animation const& animation, float current_time) {
	float progress = (current_time - animation.time.start_time) / animation.time.duration;

	float change_1 = (animation.end1 - animation.start1) * progress;
	float change_2 = (animation.end2 - animation.start2) * progress;
	float change_3 = (animation.end3 - animation.start3) * progress;

	float new_v1 = animation.start1 + change_1;
	float new_v2 = animation.start2 + change_2;
	float new_v3 = animation.start3 + change_3;

	std::stringstream current_transform;
	current_transform << "transform " << animation.name << " " << animation.type << " ";
	current_transform << new_v1 << " " << new_v2 << " " << new_v3;
	return current_transform.str();
}

std::string get_current_cam(CamAnimation const& cam, float current_time) {
	float progress = (current_time - cam.time.start_time) / cam.time.duration;

	glm::vec3 position = (cam.pos_end - cam.pos_start) * progress;
	glm::vec3 direction = (cam.dir_end - cam.dir_start) * progress;
	glm::vec3 up = (cam.up_end - cam.up_start) * progress;

	glm::vec3 new_pos = cam.pos_start + position;
	glm::vec3 new_dir = cam.dir_start + direction;
	glm::vec3 new_up = cam.up_start + up;

	std::stringstream current_transform;
	current_transform << "define camera eye " << cam.fov << " ";
	current_transform << new_pos.x << " " << new_pos.y << " " << new_pos.z << " ";
	current_transform << new_dir.x << " " << new_dir.y << " " << new_dir.z << " ";
	current_transform << new_up.x << " " << new_up.y << " " << new_up.z;
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
