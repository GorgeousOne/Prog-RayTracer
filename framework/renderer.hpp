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
#include <glm/glm.hpp>
#include "color.hpp"
#include "pixel.hpp"
#include "ppmwriter.hpp"
#include "scene.hpp"

class Renderer {
public:
	Renderer(unsigned w, unsigned h, std::string const& file_name, unsigned AA_steps, unsigned max_ray_bounces);

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
	unsigned AA_steps_;
	unsigned max_ray_bounces_;

	std::atomic_uint pixel_index_;
	void thread_function(Scene const& scene, float img_plane_dist, glm::mat4 const& trans_mat);

	Color trace_color(Ray const& ray, Scene const& scene, unsigned ray_bounces) const;
	HitPoint get_closest_hit(Ray const& ray, Scene const& scene) const;
	bool light_is_blocked(glm::vec3 const& position, glm::vec3 light_dir, float range, Scene const& scene) const;

	Color shade(HitPoint const& hitPoint, Scene const& scene, unsigned ray_bounces) const;
	Color ambient_color(HitPoint const& , Light const& ambient) const;
	Color diffuse_color(HitPoint const& hitPoint, Scene const& scene) const;
	Color specular_color(HitPoint const& hitPoint, Scene const& scene) const;

	Color reflection_color(const HitPoint &hitPoint, const Scene &scene, unsigned int ray_bounces) const;
	Color refraction_color(HitPoint const& hit_point, const Scene &scene, unsigned int ray_bounces) const;
	float schlick_reflection_ratio(glm::vec3 const& ray_dir, glm::vec3 const& normal, float ior, float min_reflectance) const;

	Color normal_color(HitPoint const& hitPoint);
	Color tone_map_color(Color color) const;

};

#endif // #ifndef BUW_RENDERER_HPP
