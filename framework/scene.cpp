#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "scene.hpp"

std::shared_ptr<Material> Scene::find_mat(std::string const& name) const {
	return materials.find(name)->second;
}

std::shared_ptr<Material> load_mat(std::istringstream& arg_stream) {
	std::string name;
	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float brightness;

	arg_stream >> name;
	arg_stream >> ka.x >> ka.y >> ka.z;
	arg_stream >> kd.x >> kd.y >> kd.z;
	arg_stream >> ks.x >> ks.y >> ks.z;
	arg_stream >> brightness;
	return std::make_shared<Material>(Material{name, ka, kd, ks, brightness});
}

void add_to_scene(std::istringstream& words_stream, Scene& new_scene) {
	std::string token_str;
	words_stream >> token_str;
	int arg_count =  std::distance(std::istream_iterator<std::string>(words_stream), std::istream_iterator<std::string>());

	if ("material" == token_str) {
		if (arg_count < 11) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 11 required to create Material");
		}
		auto new_mat = load_mat(words_stream);
		new_scene.materials.emplace(new_mat->name, new_mat);
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
