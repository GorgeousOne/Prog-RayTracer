/* What do we need:
* Datenstruktur, die Strings mit Zeitintervallen speichern kann
* Funktion sdf-writer mithilfe der Datenstruktur
* Funktion, die in einem Zeitintervall die Transformationen f�r Objekte berechnet und z.B. in einer map speichert
* Klasse zur Darstellung von Superhot
* Funktion dieser Klasse zur Bewegung von Superhot (Muss string mit Positionen zur�ckgeben)
* Ostream Funktion für die Datenstrukturen zum SDF schreiben
* Kamerabewegung und Position benütigt evtl auch eigene Datenstruktur
*/

#include <iostream>
#include <iomanip>
#include <fstream> 
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <scene.hpp>
#include "bodypart.h"

struct Interval {
	float start_time = 0.0f;
	float duration = 1.0f;

	bool is_active(float current_time) const {
		return current_time >= start_time && current_time < start_time + duration;
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

struct Cam_Animation {
	Interval time{};
	glm::vec3 pos_start{};
	glm::vec3 dir_start{};
	glm::vec3 up_start{};
	glm::vec3 pos_end = pos_start;
	glm::vec3 dir_end = pos_end;
	glm::vec3 up_end = pos_end;
	float fov = 60.0f;
};

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

std::string get_current_cam(Cam_Animation const& cam, float current_time) {
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

struct BodyAnimation {
	Pose pose0;
	Pose pose1;
	Interval time;
};

Pose read_pose(std::string const& file_path) {
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

void write_to_sdf(
		std::map<std::string, Interval> const& object_map,
		std::vector<Animation> const& animations,
		std::vector<Cam_Animation> const& cam_animations,
		std::vector<BodyAnimation> const& body_animations,
		unsigned fps,
		float movie_duration,
		std::string const& directory,
		unsigned res_x,
		unsigned res_y,
		unsigned aa_steps,
		unsigned ray_bounces) {

	float frame_duration = 1.0f / fps;
	unsigned total_frame_count = movie_duration * fps;
						Body body{};

	for (int frame = 0; frame < total_frame_count; ++frame) {
		float current_time = frame * frame_duration;

		std::stringstream file_name;
		file_name << "frame" << std::setfill('0') << std::setw(4) << frame+1;
		std::ofstream sdf_file(directory + "/" + file_name.str() + ".sdf");

		for (auto const& animation : body_animations) {
			if (animation.time.is_active(current_time)) {
				apply_curren_pose(body, animation, current_time);
				sdf_file << body;
			}
		}
		sdf_file << std::endl;

		for (auto const& object : object_map) {
			if (object.second.is_active(current_time)) {
				sdf_file << object.first << std::endl;
			}
		}
		sdf_file << std::endl;

		for (auto const& animation : animations) {
			if (animation.time.is_active(current_time)) {
				sdf_file << get_current_transform(animation, current_time) << std::endl;
			}
		}
		sdf_file << std::endl;

		for (auto const& animation : cam_animations) {
			if (animation.time.is_active(current_time)) {
				sdf_file << get_current_cam(animation, current_time) << std::endl;
			}
		}
		sdf_file << "render " << file_name.str() << ".ppm " << res_x << " " << res_y  << " " << aa_steps  << " " << ray_bounces << std::endl;
		sdf_file.close();
	};
}


void generate_movie(std::string const& res_dir) {
	float movie_duration = 2.0f;
	unsigned fps = 24;

	std::map<std::string, Interval> object_map;
	std::vector<Animation> animations;
	std::vector<Cam_Animation> cam_animations;
	std::vector<BodyAnimation> body_animations;

	// lights
	object_map.insert({ "define ambient amb 1 1 1 1", {0, movie_duration}});
	object_map.insert({ "define light bulb 0 9 0 .2 .2 .2 32", {0, movie_duration}});

	// materials
	object_map.insert({"define material white 1 1 1 1 1 1 0 0 0 1 0 1 1", {0, movie_duration}});
	object_map.insert({"define material yellow 1 1 0 1 1 0 1 1 0 50 1 1 1", {0, movie_duration}});
	object_map.insert({"define material obsidian 0 0 0 .01 .01 .01 1 1 1 50 .1 1 1", {0, movie_duration}});

	// shapes
	object_map.insert({"define shape sphere s1 5 0 0 1 white", {0, 2}});
	object_map.insert({"define shape box b1 -1 -1 -1 1 1 1 white", {0, 2}});

	// camera
	object_map.insert({"define camera eye 60 0 0 20 0 0 -1 0 1 0", {0, 0.1}});
	cam_animations.emplace_back(Cam_Animation{ { 0.1, 2 }, { 0, 0, 20 }, { 0, 0, -1 }, { 0, 1, 0 }, { 5, 0, 20 }, { 0, 0, -1 }, { 0, 1, 0 } });

	// translations
	//animations.emplace_back(Animation{"b1", "translate", {0, 2}, 2, -5, 0, 2, 5, 0});
	//animations.emplace_back(Animation{"s1", "translate", {0, 2}, -2, 5, 0, -2, -5, 0});

	// rotations

	//scalings
	animations.emplace_back(Animation{"b1", "scale", {0, 1}, 1, 1, 1, 2, 3, 1});

	//walk animation
	animations.emplace_back(Animation{"chest", "rotate", {0, 5}, 0, 90, 0, 0, 90, 0});
	std::vector<Pose> walk;

	for (int i = 0; i < 8; ++i) {
		walk.emplace_back(read_pose(res_dir + "/poses/pose0" + std::to_string(i+1) + ".txt"));
	}
	for (unsigned i = 0; i < pose_count; ++i) {
		body_animations.emplace_back(BodyAnimation{walk[i % walk.size()], walk[(i + 1) % walk.size()], {i * walk_speed, walk_speed}});
	}

	write_to_sdf(
			object_map,
			animations,
			cam_animations,
			body_animations,
			fps,
			movie_duration,
			directory,
			720, 480,
			2,
			5);
}

void render_movie(
		unsigned start_frame,
		unsigned end_frame,
		std::string const& src_dir,
		std::string const& res_dir,
		std::string const& out_dir) {

	for (int i = start_frame; i < end_frame; ++i) {
		std::stringstream file_name;
		file_name << "frame" << std::setfill('0') << std::setw(4) << i+1 << ".sdf";
		load_scene(src_dir + "/" + file_name.str(), res_dir, out_dir);
		std::cout << file_name.str() << "\n";
	}
}

int main(int argc, char** argv) {
	std::cout << "generate sdfs\n";
	generate_movie("./movie/obj");
	std::cout << "render sdfs";
	render_movie(0, 120, "./movie/files", "./movie/obj", "./movie/images");
	return 0;
}