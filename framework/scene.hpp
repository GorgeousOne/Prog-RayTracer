#ifndef RAYTRACER_SCENE_HPP
#define RAYTRACER_SCENE_HPP

#include "Shape.hpp"
#include "camera.hpp"
#include "pointLight.hpp"
#include "ambientLight.hpp"
#include <vector>
#include <map>

struct Scene {
	std::map<std::string, std::shared_ptr<Material>> materials{};
	std::map<std::string, std::shared_ptr<Shape>> shapes{};
	std::vector<PointLight> lights{};
	AmbientLight ambient{};
	Camera camera{};
};

void add_to_scene(std::istringstream& words_stream, Scene& new_scene);
Scene load_scene(std::string const& file_path);
#endif