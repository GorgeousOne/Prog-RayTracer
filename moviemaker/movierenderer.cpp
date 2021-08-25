
#include <string>
#include <filesystem>
#include <iostream>
#include "scene.hpp"

namespace fs = std::filesystem;

void render_movie(
		std::string const& src_dir,
		std::string const& res_dir,
		std::string const& out_dir) {

	std::cout << src_dir << "\n";
	for (auto const& entry : fs::directory_iterator(src_dir)) {
		std::string name = entry.path().filename().string();
		std::cout << name << " ";

		//checks that file has sdf extension
		if(name.substr(name.find_last_of(".") + 1) != "sdf") {
			continue;
		}
		std::cout << "is sdf" << "\n";
		//loads and renders scene
		load_scene(entry.path().string(), res_dir, out_dir);
	}
}

int main(int argc, char** argv) {
	render_movie("./movie/files", "./movie/obj", "./movie/images");
	return 0;
}
