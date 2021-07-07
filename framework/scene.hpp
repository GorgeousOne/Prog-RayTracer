#ifndef RAYTRACER_SCENE_HPP
#define RAYTRACER_SCENE_HPP

#include "shape.hpp"
#include "camera.hpp"
#include "pointLight.hpp"
#include "ambientLight.hpp"
#include <vector>
#include <map>


class Scene {
public:
	Scene();

private:
	std::map<std::string, std::shared_ptr<Shape>> shapes;
	std::vector<PointLight> lights;
	AmbientLight ambient;
	Camera camera;
	//std::vector<Pixel> pixel_array;
};

//std::vector<Pixel> create_image(int res_x, int res_y);
void add_shape(Shape const& shape);
void remove_shape(Shape const& shape);

#endif