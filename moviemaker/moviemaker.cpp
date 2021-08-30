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
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <scene.hpp>
#include "bodypart.h"
#include "animation.h"

void write_to_sdf(
		std::map<std::string, Interval> const& object_map,
		std::vector<Animation> const& animations,
		std::vector<CamAnimation> const& cam_animations,
		std::vector<BodyAnimation> const& body_animations,
		unsigned fps,
		float movie_duration,
		std::string const& out_directory,
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
		std::ofstream sdf_file(out_directory + "/" + file_name.str() + ".sdf");

		//sdf_file << "#body pose" << std::endl << *body;

		for (auto const& animation : body_animations) {
			if (animation.time.is_active(current_time)) {
				apply_curren_pose(body, animation, current_time);
				sdf_file << body;
				break;
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

void generate_movie(
		std::string const& res_dir,
		std::string const& out_dir) {
	float movie_duration = 10.0f;
	unsigned fps = 20;

	std::map<std::string, Interval> object_map;
	std::vector<Animation> animations;
	std::vector<CamAnimation> cam_animations;
	std::vector<BodyAnimation> body_animations;

	float walk_speed = 0.3f;
	float velocity =70;
	int pose_count = ceilf(movie_duration / walk_speed);

	//walking
	std::vector<Pose> walk;
	//load 8 poses
	for (int i = 0; i < 8; ++i) {
		walk.emplace_back(read_pose(res_dir + "/poses/pose0" + std::to_string(i+1) + ".txt"));
	}
	//cycle poses throughout whole animation
	for (unsigned i = 0; i < pose_count; ++i) {
		body_animations.emplace_back(BodyAnimation{
			walk[i % walk.size()],
			walk[(i + 1) % walk.size()],
			{i * walk_speed, walk_speed}});
	}
	animations.emplace_back(Animation{"body", "translate", {0, movie_duration}, 0, -100, 0, velocity * movie_duration, -100, 0});
	animations.emplace_back(Animation{"body", "rotate", {0, movie_duration}, 0, 90, 0, 0, 90, 0});

	// lights
	object_map.insert({"define ambient amb 1 1 1 1", {0, movie_duration}});
	object_map.insert({"define light bulb 50 100 500 .2 .2 .2 32", {0, movie_duration}});

	// materials: name, ka, kd, ks, m, glossiness, opacity, ior
	object_map.insert({"define material obsidian 0 0 0 .01 .01 .01 1 1 1 50 .1 1 1", {0, movie_duration}});
	object_map.insert({"define material blue_glass .8 .8 1 .6 .6 1 .6 .6 1 500 0.028 0.1 1.4", {0, movie_duration}});
	object_map.insert({"define material green_mirror .8 1 .8 .6 1 .6 .6 1 .6 500 1 1 1", {0, movie_duration}});
	object_map.insert({"define material white 1 1 1 1 1 1 0 0 0 1 0 1 1", {0, movie_duration}});
	object_map.insert({"define material yellow 1 1 0 1 1 0 1 1 0 50 1 1 1", {0, movie_duration}});

	// shapes
	object_map.insert({"define shape sphere s1 0 0 0 75 blue_glass", {0, movie_duration}});
	object_map.insert({"define shape box b1 -30 -30 -30 30 30 30 green_mirror", {0, movie_duration}});
//	object_map.insert({ "define shape obj superhot", {0, movie_duration}});

	// camera
	object_map.insert({"define camera eye 60 0 0 600 0 0 -1 0 1 0", {0, 0.1}});
	cam_animations.emplace_back(CamAnimation{ { 0.1, 5 }, { 0, 0, 600 }, { 0, 0, -1 }, { 0, 1, 0 }, { 5*velocity, 0, 600 }, { 0, 0, -1 }, { 0, 1, 0 } });
	cam_animations.emplace_back(CamAnimation{ { 5, 8 }, { 5*velocity, 0, 600 }, { 0, 0, -1 }, { 0, 1, 0 }, { 5*velocity, 0, 0 }, { 1, 0, 0 }});

	// translations
//	animations.emplace_back(Animation{ "superhot", "translate", {0, 8}, 0, -75, 0, 960, -75, 0 });
	animations.emplace_back(Animation{"b1", "translate", {0, 5}, 0, 0, -200, 0, 0, -200});
	animations.emplace_back(Animation{"s1", "translate", {0, 5}, 400, 0, 100, 400, 0, 100});

	// rotations
//	animations.emplace_back(Animation{ "superhot", "rotate", {0, 8}, 0, 90, 0, 0, 90, 0 });

	//scalings
	animations.emplace_back(Animation{"b1", "scale", {0, 5}, 3, 5, 1, 3, 5, 1});

	write_to_sdf(
			object_map,
			animations,
			cam_animations,
			body_animations,
			fps,
			movie_duration,
			out_dir,
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
	generate_movie("./movie/obj", "./movie/files");

	std::cout << "render sdfs";
	render_movie(0, 200, "./movie/files", "./movie/obj", "./movie/images");
	return 0;
}