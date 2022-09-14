
#include <GL/glew.h>
#include <GLFW//glfw3.h>
#include <iostream>
#include <chrono>
#include <window.hpp>
#include <fstream>

#include "scene.hpp"
#include "renderer.hpp"

int main(int argc, const char** argv) {
	unsigned img_width = 1202;
	unsigned img_height = 676;
//	unsigned img_width = 100;
//	unsigned img_height = 100;
//	unsigned img_width = 400;
//	unsigned img_height = 400;

	std::string sdf_directory_path = argv[1];
	std::cout << "resource directory: " << sdf_directory_path << std::endl;

	Scene scene = load_scene(sdf_directory_path + "/cornell.sdf", sdf_directory_path, sdf_directory_path);
	Renderer renderer{img_width, img_height, sdf_directory_path + "/img.ppm", 4, 7, 250};
	std::cout << "shapes " << scene.root->child_count() << "\n";
	std::cout << "lights " << scene.lights.size() << "\n";

	try {
		auto start = std::chrono::steady_clock::now();
		renderer.render(scene, scene.camera);
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		std::cout << elapsed_seconds.count() << "s\n";
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
