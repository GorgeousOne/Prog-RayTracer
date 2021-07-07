#ifndef RAYTRACER_SCENE_HPP
#define RAYTRACER_SCENE_HPP

#include "Shape.hpp"
#include "camera.hpp"
#include "pointLight.hpp"
#include "ambientLight.hpp"
#include <vector>
#include <map>

struct Scene {
	std::map<std::string, std::shared_ptr<Shape>> shapes{};
	std::vector<PointLight> lights{};
	AmbientLight ambient{};
	Camera camera{};
};
#endif