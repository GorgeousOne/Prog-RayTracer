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

#define PI 3.14159265f

void Renderer::render(Scene const& scene, Camera const& cam) {
	float fov_radians = cam.fov_x / 180 * PI;
	float img_plane_dist = (width_ / 2.0f) / tan(fov_radians / 2);

	glm::vec3 pixel_width = glm::cross(cam.direction, cam.up);
	glm::vec3 pixel_height {cam.up};
	assert(1.0f == 	glm::length(pixelWidth));

	// corner of img_plane relative to camera
	glm::vec3 min_corner =
			img_plane_dist * cam.direction
			- (width_ / 2.0f) * pixel_width
			- (height_ / 2.0f) * pixel_height;

	for (unsigned x = 0; x < width_; ++x) {
		for (unsigned y = 0; y < height_; ++y) {

			// vector for 3D position of 2D pixel relative to camera
			glm::vec3 pixel_pos {
				min_corner
				+ static_cast<float>(x) * pixel_width
				+ static_cast<float>(y) * pixel_height};
			
			glm::vec3 ray_dir{ glm::normalize(pixel_pos) };
			Ray ray {cam.position, ray_dir};
			Pixel pixel {x, y};
			pixel.color = get_intersection_color(ray, scene);
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

HitPoint Renderer::get_closest_hit(Ray const& ray, Scene const& scene) {
	HitPoint closest_hit{};

	for (auto const& it : scene.shapes) {
		float t;
		HitPoint hit = it.second->intersect(ray, t);

		if (!hit.does_intersect) {
			continue;
		}
		if (!closest_hit.does_intersect || hit.intersection_distance < closest_hit.intersection_distance) {
			closest_hit = hit;
		}
	}
	return closest_hit;
}

HitPoint Renderer::find_light_block(Ray const& light_ray, Scene const& scene) {

	for (auto const &it : scene.shapes) {
		float t;
		HitPoint hit = it.second->intersect(light_ray, t);

		if (hit.does_intersect && t <= 1) {
			return hit;
		}
	}
	return HitPoint {};
}

Color Renderer::get_intersection_color(Ray const& ray, Scene const& scene) {
	HitPoint closest_hit = get_closest_hit(ray, scene);
	return closest_hit.does_intersect ? shade(closest_hit, scene) : Color {};
}

Color Renderer::shade(HitPoint const& hitPoint, Scene const& scene) {
	Color shaded_color {0, 0, 0};
//	shaded_color.r = hitPoint.hit_material->kd.x;
//	shaded_color.g = hitPoint.hit_material->kd.y;
//	shaded_color.b = hitPoint.hit_material->kd.z;

//	shaded_color += normal_color(hitPoint);
	shaded_color += diffuse_color(hitPoint, scene);
	return shaded_color;
}

Color Renderer::diffuse_color(HitPoint const& hitPoint, Scene const& scene) {
	Color diffuse_color {};

	for (PointLight const& light : scene.lights)  {
		glm::vec3 light_dir = light.position - hitPoint.position;
		Ray light_ray {hitPoint.position, light_dir};
		HitPoint light_block = find_light_block(light_ray, scene);

		if (light_block.does_intersect) {
			continue;
		}
		float cos_incidence_angle = glm::dot(hitPoint.surface_normal, glm::normalize(light_ray.direction));
		Color light_intensity = light.color * light.brightness;
		diffuse_color += light_intensity * hitPoint.hit_material->kd * cos_incidence_angle;
	}
	return diffuse_color;
}

Color Renderer::normal_color(HitPoint const& hitPoint) {
	return Color {
			(hitPoint.surface_normal.x + 1) / 2,
			(hitPoint.surface_normal.y + 1) / 2,
			(hitPoint.surface_normal.z + 1) / 2
	};
}

float *Renderer::pixel_buffer() const {
	auto* pixel_data = new float[width_ * height_ * 3];

	for (int i = 0; i < color_buffer_.size(); ++i) {
		Color color = color_buffer_[i];
		pixel_data[i * 3] = color.r;
		pixel_data[i * 3 + 1] = color.g;
		pixel_data[i * 3 + 2] = color.b;
	}

	return pixel_data;
}



