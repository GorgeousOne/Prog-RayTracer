#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "scene.hpp"
#include "box.hpp"
#include "sphere.hpp"

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

std::shared_ptr<Box> load_box(std::istringstream& arg_stream, std::map<std::string, std::shared_ptr<Material>> const& materials) {
	std::string name;
	std::string mat_name;
	glm::vec3 min;
	glm::vec3 max;

	arg_stream >> name;
	arg_stream >> min.x >> min.y >> min.z;
	arg_stream >> max.x >> max.y >> max.z;
	arg_stream >> mat_name;

	auto it = materials.find(mat_name);
	return std::make_shared<Box>(min, max, name, it->second);
}

std::shared_ptr<Sphere> load_sphere(std::istringstream& arg_stream, std::map<std::string, std::shared_ptr<Material>> const& materials) {
	std::string name;
	std::string mat_name;
	float radius;
	glm::vec3 center;

	arg_stream >> name;
	arg_stream >> center.x >> center.y >> center.z;
	arg_stream >> radius;
	arg_stream >> mat_name;

	auto it = materials.find(mat_name);
	return std::make_shared<Sphere>(radius, center, name, it->second);
}

PointLight load_point_light(std::istringstream& arg_stream) {
	std::string name;
	glm::vec3 pos;
	Color color;
	float brightness;

	arg_stream >> name;
	arg_stream >> pos.x >> pos.y >> pos.z;
	arg_stream >> color.r >> color.g >> color.b;
	arg_stream >> brightness;

	return { name, color, brightness, pos };
}

AmbientLight load_ambient(std::istringstream& arg_stream) {
	std::string name;
	Color color;
	float brightness;

	arg_stream >> name;
	arg_stream >> color.r >> color.b >> color.g;
	arg_stream >> brightness;

	return { name, color, brightness };
}

Camera load_camera(std::istringstream& arg_stream) {
	std::string name;
	glm::vec3 pos{ 0 , 0 , 0 };
	glm::vec3 direction{ 0, 0, -1 };
	float fov_x;

	arg_stream >> name;
	arg_stream >> fov_x;

	return { name, pos, direction, fov_x };
}

void add_to_scene(std::istringstream& words_stream, Scene& new_scene) {
	std::string token_str;
	words_stream >> token_str;
	int arg_count =  std::distance(std::istream_iterator<std::string>(words_stream), std::istream_iterator<std::string>());

	if ("material" == token_str) {
		if (arg_count != 11) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 11 required to create Material");
		}
		auto new_mat = load_mat(words_stream);
		new_scene.materials.emplace(new_mat->name, new_mat);
	}
	if ("shape" == token_str) {
		words_stream >> token_str;
		if ("box" == token_str) {
			if (arg_count != 8) {
				throw std::runtime_error(std::to_string(arg_count) + " arguments given, 8 required to create Material");
			}
			auto new_box = load_box(words_stream, new_scene.materials);
			new_scene.shapes.emplace(new_box->get_name(), new_box);
		}
		if ("sphere" == token_str) {
			if (arg_count != 6) {
				throw std::runtime_error(std::to_string(arg_count) + " arguments given, 6 required to create Material");
			}
			auto new_sphere = load_sphere(words_stream, new_scene.materials);
			new_scene.shapes.emplace(new_sphere->get_name(), new_sphere);
		}
	}
	if ("light" == token_str) {
		if (arg_count != 8) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 8 required to create Material");
		}
		PointLight new_light{load_point_light(words_stream) };
		new_scene.lights.push_back(new_light);
	}
	if ("ambient" == token_str) {
		if (arg_count != 5) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 5 required to create Material");
		}
		new_scene.ambient = {load_ambient(words_stream) };
	}
	if ("camera" == token_str) {
		if (arg_count != 2) {
			throw std::runtime_error(std::to_string(arg_count) + " arguments given, 2 required to create Material");
		}
		new_scene.camera = {load_camera(words_stream) };
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