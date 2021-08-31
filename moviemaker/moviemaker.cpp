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
				break;
			}
		}
		sdf_file << "render " << file_name.str() << ".ppm " << res_x << " " << res_y  << " " << aa_steps  << " " << ray_bounces << std::endl;
		sdf_file.close();
	};
}

void generate_movie(
		std::string const& res_dir,
		std::string const& out_dir) {
	float movie_duration = 20;
	unsigned fps = 20;

	std::map<std::string, Interval> object_map;
	std::vector<Animation> animations;
	std::vector<CamAnimation> cam_animations;
	std::vector<BodyAnimation> body_animations;

	float velocity = 80;
	float walk_speed = 18 / velocity;
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
	animations.emplace_back(Animation{"body", "translate", {0, movie_duration}, {0, 95, 0}, {velocity * movie_duration, 95, 0}});
	animations.emplace_back(Animation{"body", "rotate", {0, movie_duration}, {0, 90, 0}});

	// lights
	object_map.insert({"define ambient amb 1 1 1 1", {0, movie_duration}});
	object_map.insert({"define light bulb -20000 100000 30000 1 1 1 6", {0, movie_duration}});

	// materials: name, ka, kd, ks, m, glossiness, opacity, ior
	object_map.insert({"define material white 1 1 1 1 1 1 0 0 0 1 0 1 1", {0, movie_duration}});
	object_map.insert({"define material white_glaze 1 1 1 1 1 1 1 1 1 200 .1 1 1", {0, movie_duration}});
	object_map.insert({"define material silver 1 1 .9 1 1 .9 1 1 .9 200 .5 1 1", {0, movie_duration}});
	object_map.insert({"define material gold 1 .76 0 1 .76 0 1 .76 0 200 .5 1 1", {0, movie_duration}});
	object_map.insert({"define material bronze .64 .28 .15 .64 .28 .15 .96 .14 .08 200 .5 1 1", {0, movie_duration}});
	object_map.insert({"define material lapis .05 0 1 .05 0 1 1 1 1 200 .05 1 1", {0, movie_duration}});

	Interval wb_time{1, 11};
	Interval ws_time{3, 18};
	Interval rb_time{6, 16};
	// shapes
	object_map.insert({"define shape box floor 0 0 0 10000 1 2000 white", {0, movie_duration}});

	object_map.insert({"define shape box wb1 -20 0 -20 20 40 20 white",wb_time});
	object_map.insert({"define shape box wb2 -30 -30 -30 30 100 30 white", wb_time});
	object_map.insert({"define shape box wb3 -15 -15 -15 15 70 15 white", wb_time});

	object_map.insert({"define shape sphere ws1 0 10 0 10 white_glaze", ws_time});
	object_map.insert({"define shape sphere ws2 0 35 0 35 white_glaze", ws_time});
	object_map.insert({"define shape sphere ws3 0 20 0 20 white_glaze", ws_time});

	object_map.insert({"define shape box rb1 -40 0 -40 40 160 40 silver", rb_time});
	object_map.insert({"define shape box rb2 -40 0 -40 40 160 40 gold", rb_time});
	object_map.insert({"define shape box rb3 -40 0 -40 40 160 40 bronze", rb_time});
	object_map.insert({"define shape box rb4 -40 0 -40 40 160 40 lapis", rb_time});

	float chest_height = 100;
	float cam_dist = 550;
	// camera
	cam_animations.emplace_back(CamAnimation{{0, 3}, {50, 20, 150}, {0, -.1, -1}, {3*velocity, chest_height, cam_dist}, {0, 0, -1}, ease_cos_in, 0.7});
	cam_animations.emplace_back(CamAnimation{{3, 17}, {3*velocity, chest_height, cam_dist}, {0, 0, -1}, {20*velocity, chest_height, cam_dist}});

	// translations
	animations.emplace_back(Animation{"floor", "translate", {0, movie_duration}, {-1000, -1, -800}});

	animations.emplace_back(Animation{"wb1", "translate", wb_time, {370, 0, -110}});
	animations.emplace_back(Animation{"wb2", "translate", wb_time, {450, 0, -100}});
	animations.emplace_back(Animation{"wb3", "translate", wb_time, {510, 0, -110}});

	animations.emplace_back(Animation{"ws1", "translate", ws_time, {600, 0, 60}});
	animations.emplace_back(Animation{"ws2", "translate", ws_time, {650, 0, 40}});
	animations.emplace_back(Animation{"ws3", "translate", ws_time, {700, 0, 70}});

	animations.emplace_back(Animation{"rb1", "translate", rb_time, {950, 0, -100}});
	animations.emplace_back(Animation{"rb2", "translate", rb_time, {1080, 0, -120}});
	animations.emplace_back(Animation{"rb3", "translate", rb_time, {1210, 0, -110}});
	animations.emplace_back(Animation{"rb4", "translate", rb_time, {1340, 0, -100}});

	// rotations
	animations.emplace_back(Animation{"wb1", "rotate", wb_time, {0, 35, 0}});
	animations.emplace_back(Animation{"wb2", "rotate", wb_time, {0, -15, 15}});
	animations.emplace_back(Animation{"wb3", "rotate", wb_time, {0, 10, -10}});

	animations.emplace_back(Animation{"rb1", "rotate", rb_time, {0, -40, 0}});
	animations.emplace_back(Animation{"rb2", "rotate", rb_time, {0, 20, 0}});
	animations.emplace_back(Animation{"rb3", "rotate", rb_time, {0, 35, 0}});
	animations.emplace_back(Animation{"rb4", "rotate", rb_time, {0, 25, 0}});

	//scalings

	write_to_sdf(
			object_map,
			animations,
			cam_animations,
			body_animations,
			fps,
			movie_duration,
			out_dir,
			854, 480, 1,
//			1280, 720, 2,
			3);
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

	std::cout << "render sdfs\n";
	render_movie(0, 200, "./movie/files", "./movie/obj", "./movie/images");
	return 0;
}