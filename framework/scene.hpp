#ifndef RAYTRACER_SCENE_HPP
#define RAYTRACER_SCENE_HPP

#include <vector>


struct Scene {
	std::vector<Shape> shapes;
	std::vector<PointLight> lights;
	AmbientLight ambient;
	Camera camera;
	std::vector<Pixel> pixel_array;
};


#endif