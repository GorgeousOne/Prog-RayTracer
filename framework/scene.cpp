#include <fstream>
#include <sstream>
#include "scene.hpp"

Scene loadScene(std::string const& file_path) {
	Scene new_scene{};

	std::ifstream input_sdf_file(file_path);
	std::string line_buffer = "";

	//writes sdf file line by line into line_buffer
	while (std::getline(input_sdf_file, line_buffer)) {
		std::cout << line_buffer << std::endl;
		std::istringstream words_stream(line_buffer);

		//streams words of each line
		std::string token_str = "";
		words_stream >> token_str;

		if ("#" == token_str) {
			continue;
		}
		if ("define" == token_str) {

		} else if ("render") {

		}
	}
	return new_scene;
}