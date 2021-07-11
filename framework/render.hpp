#ifndef RAYTRACER_RENDER_HPP
#define RAYTRACER_RENDER_HPP

#include <string>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "camera.hpp"
#include "scene.hpp"

struct Render {
	unsigned int res_x = 320;
	unsigned int res_y = 240;
	Camera camera{};
	std::string file_path = "./img.ppm";

	void render(Scene const& scene) const;
};

#endif //RAYTRACER_RENDER_HPP
