
#include <GL/glew.h>
#include <GLFW//glfw3.h>
#include <iostream>
#include <chrono>

#include "scene.hpp"
#include "renderer.hpp"

void display_image(unsigned width, unsigned height, float* data) {
	GLFWwindow* window;

	if (!glfwInit()) {
		throw "Could not init glfw";
	}
	window = glfwCreateWindow(width, height, "hi", nullptr, nullptr);

	if (!window) {
		throw "Could not open window";
	}
	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawPixels(width, height, GL_RGB, GL_FLOAT, data);
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}
}

int main(int argc, const char** argv) {
	unsigned img_width = 800;
	unsigned img_height = 800;

	Scene scene = load_scene("../../../sdf/example.sdf");
	Renderer renderer{img_width, img_height, "../../../sdf/img.ppm"};

	std::cout << "shapes " << scene.shapes.size() << "\n";
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
	display_image(img_width, img_height, renderer.pixel_buffer());
	return 0;
}
