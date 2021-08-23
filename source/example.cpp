
#include <GL/glew.h>
#include <GLFW//glfw3.h>
#include <iostream>
#include <chrono>
#include <window.hpp>

#include "scene.hpp"
#include "renderer.hpp"

int main(int argc, const char** argv) {
	unsigned img_width = 800;
	unsigned img_height = 800;

	Scene scene = load_scene("../../../sdf/example.sdf");
	Renderer renderer{img_width, img_height, "../../../sdf/img.ppm", 2};
	std::cout << "shapes " << scene.shapes.size() << "\n";
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
