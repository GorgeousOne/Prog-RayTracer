// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#include "renderer.hpp"

Renderer::Renderer(unsigned w, unsigned h, std::string const& file)
		: width_(w), height_(h), color_buffer_(w * h, Color{0.0, 0.0, 0.0}), filename_(file), ppm_(width_, height_) {}

void Renderer::render() {
	std::size_t const checker_pattern_size = 20;

	for (unsigned y = 0; y < height_; ++y) {
		for (unsigned x = 0; x < width_; ++x) {
			Pixel p(x, y);
			if (((x / checker_pattern_size) % 2) != ((y / checker_pattern_size) % 2)) {
				p.color = Color{0.0f, 1.0f, float(x) / height_};
			} else {
				p.color = Color{1.0f, 0.0f, float(y) / width_};
			}

			write(p);
		}
	}
	ppm_.save(filename_);
}

glm::vec3 Renderer::hit_color(Ray const& ray, Scene const& scene) {
	HitPoint min_hit{};

	for (auto const& it : scene.shapes) {
		float t;
		HitPoint hit = it.second->intersect(ray, t);
		if (!hit.does_intersect) {
			continue;
		}
		if (!min_hit.does_intersect || hit.intersection_distance < min_hit.intersection_distance) {
			min_hit = hit;
		}
	}
	return min_hit.does_intersect ? min_hit.hit_object_color->ka : glm::vec3{};
}

#define PI 3.14159265f

void Renderer::render(Scene const& scene, Camera const& cam) {
	float fov_radians = cam.fov_x / 180 * PI;
	float img_plane_dist = (width_ / 2.0f) / tan(fov_radians / 2);

	glm::vec3 pixel_width = glm::cross(cam.direction, cam.up);
	glm::vec3 pixel_height {cam.up};
	assert(1.0f == 	glm::length(pixelWidth));
	glm::vec3 min_corner =
			cam.position
			+ img_plane_dist * cam.direction
			- (width_ / 2.0f) * pixel_width
			- (height_ / 2.0f) * pixel_height;

	for (unsigned x = 0; x < width_; ++x) {
		for (unsigned y = 0; y < height_; ++y) {
			glm::vec3 ray_dir {
				min_corner
				+ static_cast<float>(x) * pixel_width
				+ static_cast<float>(y) * pixel_height};

			Ray ray {cam.position, glm::normalize(ray_dir)};
			Pixel pixel {x, y};
			auto col = hit_color(ray, scene);
			pixel.color = Color{col.x, col.y, col.z};
			write(pixel);
		}
	}
	ppm_.save(filename_);
}

void Renderer::write(Pixel const &p) {
	// flip pixels, because of opengl glDrawPixels
	size_t buf_pos = (width_ * p.y + p.x);

	if (buf_pos >= color_buffer_.size() || (int) buf_pos < 0) {
		std::cerr << "Fatal Error Renderer::write(Pixel p) : "
		          << "pixel out of ppm_ : "
		          << (int) p.x << "," << (int) p.y
		          << std::endl;
	} else {
		color_buffer_[buf_pos] = p.color;
	}
	ppm_.write(p);
}
