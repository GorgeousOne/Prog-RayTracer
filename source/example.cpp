
#include <GL/glew.h>
#include <GLFW//glfw3.h>
#include <iostream>
#include <chrono>
#include <window.hpp>
#include <fstream>

#include "scene.hpp"
#include "renderer.hpp"

int main(int argc, const char** argv) {
	unsigned img_width = 800;
	unsigned img_height = 800;

	std::ifstream resource_paths_file("resource-paths.txt");
	std::string sdf_directory_path;
	std::getline(resource_paths_file, sdf_directory_path);
	std::cout << "resource directory: " << sdf_directory_path << std::endl;

	Scene scene = load_scene(sdf_directory_path + "/cornell.sdf", sdf_directory_path, sdf_directory_path);
	Renderer renderer{img_width, img_height, sdf_directory_path + "/img.ppm", 2, 5};
	std::cout << "shapes " << scene.root->child_count() << "\n";
	std::cout << "lights " << scene.lights.size() << "\n";

	try {
		renderer.render(scene, scene.camera);
	} catch (const char *str) {
		std::cout << "Exception: " << str << std::endl;
	}
	Window window{{img_width, img_height}};

	while (!window.should_close()) {
		if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			window.close();
		}
		window.show(renderer.color_buffer());
	}
	return 0;
}
