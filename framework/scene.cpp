#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "scene.hpp"

void add_mat_to_scene(std::istringstream& arg_stream, Scene& new_scene) {
	std::string name;
	Color ka;
	Color kd;
	Color ks;
	float brightness;

	arg_stream >> name;
	arg_stream >> ka.r >> ka.g >> ka.b;
	arg_stream >> kd.r >> kd.g >> kd.b;
	arg_stream >> ks.r >> ks.g >> ks.b;
	arg_stream >> brightness;
	new_scene.materials.emplace(name, std::make_shared<Material>(Material{name, ka, kd, ks, brightness}));
}

void add_to_scene(std::istringstream& words_stream, Scene& new_scene) {
	std::string token_str;
	words_stream >> token_str;
	int arg_count =  std::distance(std::istream_iterator<std::string>(words_stream), std::istream_iterator<std::string>());

	if ("material" == token_str) {
		if (arg_count < 11) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 11 required to create Material");
		}
		add_mat_to_scene(words_stream, new_scene);
	}
}

Scene load_scene(std::string const& file_path) {
	Scene new_scene{};

	std::ifstream input_sdf_file(file_path);
	std::string line_buffer;

	//writes sdf file line by line into line_buffer
	while (std::getline(input_sdf_file, line_buffer)) {
		std::cout << line_buffer << std::endl;
		std::istringstream words_stream(line_buffer);

		//streams words of each line
		std::string token_str;
		words_stream >> token_str;

		if ("#" == token_str) {
			continue;
		}
		if ("define" == token_str) {
			add_to_scene(words_stream, new_scene);
		} else if ("render") {

		}
	}
	return new_scene;
}