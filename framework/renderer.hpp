// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#ifndef BUW_RENDERER_HPP
#define BUW_RENDERER_HPP

#include <string>
#include <glm/glm.hpp>
#include "color.hpp"
#include "pixel.hpp"
#include "ppmwriter.hpp"
#include "scene.hpp"

class Renderer {
public:
	Renderer(unsigned w, unsigned h, std::string const &file);

	void render();
	void render(Scene const& scene, Camera const& cam);
	void write(Pixel const& p);

	inline std::vector<Color> const& color_buffer() const {
		return color_buffer_;
	}
	[[nodiscard]] float* pixel_buffer() const;

private:
	unsigned width_;
	unsigned height_;
	std::vector<Color> color_buffer_;
	std::string filename_;
	PpmWriter ppm_;

	Color get_intersection_color(Ray const& ray, Scene const& scene);
	Color shade(HitPoint const& hitPoint, Scene const& scene);

	Color ambient_color(HitPoint const &, Light const &ambient);
	Color diffuse_color(HitPoint const& hitPoint, Scene const& scene);
	Color normal_color(HitPoint const &hitPoint);

	HitPoint get_closest_hit(Ray const &ray, Scene const &scene);
	HitPoint find_light_block(Ray const &light_ray, Scene const &scene);
};

#endif // #ifndef BUW_RENDERER_HPP
