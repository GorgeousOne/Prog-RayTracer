// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#include <chrono>
#include <glm/gtx/string_cast.hpp>
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
/*
void Renderer::render(Scene const& scene, Camera const& cam) {
	float fov_radians = cam.fov_x / 180 * PI;
	float img_plane_dist = (width_ / 2.0f) / tan(fov_radians / 2);

	glm::vec3 pixel_width = glm::cross(cam.direction, cam.up);
	glm::vec3 pixel_height {cam.up};
	assert(1.0f == 	glm::length(pixel_width));

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
	auto start = std::chrono::steady_clock::now();
	ppm_.save(filename_);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "save " << filename_ << "\n";
	std::cout << elapsed_seconds.count() << "s save time\n";
}*/

void Renderer::render(Scene const& scene, Camera const& cam) {
	float fov_radians = cam.fov_x / 180 * PI;
	float img_plane_dist = (width_ / 2.0f) / tan(fov_radians / 2);

	glm::vec3 u = glm::cross(cam.direction, cam.up);
	glm::vec3 v = glm::cross(u, cam.direction);
	glm::mat4 trans_mat{
			glm::vec4{u, 0},
			glm::vec4{v, 0},
			glm::vec4{-cam.direction, 0},
			glm::vec4{cam.position, 1}
	};

	std::cout << glm::to_string(trans_mat) << std::endl;

	// corner of img_plane relative to camera
	glm::vec3 min_corner{ -(width_/2.0f), -(height_/2.0f), -img_plane_dist };

	for (unsigned x = 0; x < width_; ++x) {
		for (unsigned y = 0; y < height_; ++y) {

			// vector for 3D position of 2D pixel relative to camera
			glm::vec3 pixel_pos = min_corner + glm::vec3{x, y, 0};

			glm::vec4 trans_ray_dir = trans_mat * glm::vec4{ glm::normalize(pixel_pos), 0 };
			Ray ray{ cam.position, glm::vec3{trans_ray_dir} };
			Pixel pixel{ x, y };
			pixel.color = get_intersection_color(ray, scene);
			write(pixel);
		}
	}
	auto start = std::chrono::steady_clock::now();
	ppm_.save(filename_);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "save " << filename_ << "\n";
	std::cout << elapsed_seconds.count() << "s save time\n";
}

void Renderer::write(Pixel const& p) {
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
		HitPoint hit = it.second->intersect(ray);

		if (!hit.does_intersect) {
			continue;
		}
		if (!closest_hit.does_intersect || hit.distance < closest_hit.distance) {
			closest_hit = hit;
		}
	}
	return closest_hit;
}

HitPoint Renderer::find_light_block(Ray const& light_ray, float range, Scene const& scene) {
	for (auto const& it : scene.shapes) {
		HitPoint hit = it.second->intersect(light_ray);

		if (hit.does_intersect && hit.distance <= range) {
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
	shaded_color += ambient_color(hitPoint, scene.ambient);
	shaded_color += diffuse_color(hitPoint, scene);
	return tone_map_color(shaded_color);
}

Color Renderer::ambient_color(HitPoint const& intersection, Light const& ambient) {
	return ambient.color * ambient.brightness * intersection.hit_material->ka;
}

Color Renderer::diffuse_color(HitPoint const& hitPoint, Scene const& scene) {
	Color diffuse_color {};

	for (PointLight const& light : scene.lights)  {
		glm::vec3 light_dir = light.position - hitPoint.position;
		float distance = glm::length(light_dir);

		Ray light_ray {hitPoint.position, glm::normalize(light_dir)};
		HitPoint light_block = find_light_block(light_ray, distance, scene);

		if (light_block.does_intersect) {
			continue;
		}
		float cos_incidence_angle = glm::dot(hitPoint.surface_normal, glm::normalize(light_ray.direction));

		if (cos_incidence_angle < 0) {
			continue;
		}
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

Color& Renderer::tone_map_color(Color& color) const {
	color.r /= color.r + 1;
	color.g /= color.g + 1;
	color.b /= color.b + 1;
	return color;
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



