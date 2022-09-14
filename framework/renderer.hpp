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
#include <thread>
#include <atomic>
#include <random>
#include <glm/glm.hpp>
#include "color.hpp"
#include "pixel.hpp"
#include "ppmwriter.hpp"
#include "scene.hpp"

class Renderer {
public:
	Renderer(unsigned w, unsigned h, std::string const& file_name, unsigned aa_steps, unsigned max_ray_bounces, unsigned int samples);

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
	unsigned aa_steps_;
	unsigned max_ray_bounces_;
	unsigned samples_;

	std::atomic_uint pixel_index_;
	std::atomic<float> progress;
	std::minstd_rand gen_;
	std::uniform_real_distribution<float> dist_;
	void thread_function(Scene const& scene, float img_plane_dist, glm::mat4 const& trans_mat);

	Color primary_trace(Ray const& ray, Scene const& scene);
	Color trace(Ray const& ray, Scene const& scene, unsigned samples, unsigned ray_bounces);
	HitPoint get_closest_hit(Ray const& ray, Scene const& scene) const;
	Color shade(HitPoint const &hit_point, Scene const &scene, unsigned int samples, unsigned ray_bounces);

	Color tone_map_color(Color color) const;
	Color diffuse(HitPoint const &hit_point, Scene const &scene, unsigned int samples, unsigned int ray_bounces);
	Color reflection(HitPoint const &point, Scene const &scene, unsigned samples, unsigned bounces);
	Color refraction(HitPoint const &hit_point, Scene const &scene, unsigned samples, unsigned ray_bounces);
	float schlick_reflection_ratio(glm::vec3 const &ray_dir, glm::vec3 const &normal, float const &ior) const;

	Color primary_trace(Ray const &ray, Scene const &scene, unsigned int samples);
};

#endif // #ifndef BUW_RENDERER_HPP
