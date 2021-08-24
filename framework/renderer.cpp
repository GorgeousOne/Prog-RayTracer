// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/gtx/string_cast.hpp>
#include "renderer.hpp"

#define EPSILON 0.001f

Renderer::Renderer(unsigned w, unsigned h, std::string const& file, unsigned AA_steps, unsigned max_ray_bounces):
		width_(w),
		height_(h),
		color_buffer_(w * h, Color{0.0, 0.0, 0.0}),
		filename_(file),
		ppm_(width_, height_),
		AA_steps_(AA_steps),
		max_ray_bounces_(max_ray_bounces) {}

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

	glm::vec3 u = glm::cross(cam.direction, cam.up);
	glm::vec3 v = glm::cross(u, cam.direction);
	glm::mat4 trans_mat{
			glm::vec4{u, 0},
			glm::vec4{v, 0},
			glm::vec4{-cam.direction, 0},
			glm::vec4{cam.position, 1}
	};
	//gets amount of parallel threads supported by hardware
	size_t core_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.resize(core_count);

	auto start = std::chrono::steady_clock::now();
	pixel_index_ = 0;

	//starts parallel threads all doing the same task
	for (std::thread& t : threads) {
		t = std::thread(&Renderer::thread_function, this, scene, img_plane_dist, trans_mat);
	}
	//lets main thread wait until all parallel threads finished
	for (std::thread& t : threads) {
		t.join();
	}
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << elapsed_seconds.count() << "s rendering\n";

	ppm_.save(filename_);
}

void Renderer::thread_function(Scene const& scene, float img_plane_dist, glm::mat4 const& trans_mat) {
	//continuously picks pixels to render
	float AA_unit = 1.0f / AA_steps_;
	while (true) {
		unsigned current_pixel = pixel_index_++;
		unsigned x = current_pixel % width_;
		unsigned y = current_pixel / width_;

		if (current_pixel >= width_ * height_) {
			return;
		}
		Pixel pixel{ x, y };
		for (int x_count = 0; x_count < AA_steps_; x_count++) {
			for (int y_count = 0; y_count < AA_steps_; y_count++) {
				glm::vec3 pixel_pos = glm::vec3{
					x + x_count * AA_unit - (width_ * 0.5f),
					y + y_count * AA_unit - (height_ * 0.5f),
					-img_plane_dist };

				glm::vec4 trans_ray_dir = trans_mat * glm::vec4{ glm::normalize(pixel_pos), 0 };
				Ray ray{ glm::vec3{trans_mat[3]}, glm::vec3{trans_ray_dir} };
				pixel.color += tone_map_color(trace_color(ray, scene, 0));
			}
		}
		pixel.color *= 1.0f / (AA_steps_ * AA_steps_);
		write(pixel);
	}
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

HitPoint Renderer::get_closest_hit(Ray const& ray, Scene const& scene) const {
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

HitPoint Renderer::find_light_block(Ray const& light_ray, float range, Scene const& scene) const {
	for (auto const& it : scene.shapes) {
		HitPoint hit = it.second->intersect(light_ray);

		if (hit.does_intersect && hit.distance <= range) {
			return hit;
		}
	}
	return HitPoint {};
}

Color Renderer::trace_color(Ray const& ray, Scene const& scene, unsigned ray_bounces) const {
	HitPoint closest_hit = get_closest_hit(ray, scene);
	return closest_hit.does_intersect ? shade(closest_hit, scene, ray_bounces) : Color {};
}

Color Renderer::shade(HitPoint const& hitPoint, Scene const& scene, unsigned ray_bounces) const {
	auto material = hitPoint.hit_material;

	Color shaded_color = ambient_color(hitPoint, scene.ambient);
	shaded_color += diffuse_color(hitPoint, scene);

	shaded_color *= 1 - material->glossy;
	shaded_color *= material->opacity;

//	if (material->glossy > 0 && material->opacity < 1) {

	if (material->glossy > 0) {
		shaded_color += reflection_color(hitPoint, scene, ray_bounces) * material->glossy;
	}

	if (material->opacity < 1) {
		shaded_color += refraction_color(hitPoint, scene, ray_bounces) * (1 - material->opacity);
	}

	if (material->m > 0) {
		shaded_color += specular_color(hitPoint, scene);
	}

	return shaded_color;
}

Color Renderer::ambient_color(HitPoint const& intersection, Light const& ambient) const {
	return ambient.color * ambient.brightness * intersection.hit_material->ka;
}

Color Renderer::diffuse_color(HitPoint const& hitPoint, Scene const& scene) const {
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

Color Renderer::specular_color(HitPoint const& hitPoint, Scene const& scene) const {
	Color specular_color {};

	for (PointLight const& light : scene.lights) {
		glm::vec3 light_dir = light.position - hitPoint.position;
		float distance = glm::length(light_dir);

		Ray light_ray{ hitPoint.position, glm::normalize(light_dir) };
		HitPoint light_block = find_light_block(light_ray, distance, scene);

		if (light_block.does_intersect) {
			continue;
		}
		light_dir = glm::normalize(light_dir);
		glm::vec3 reflected_light = 2 * glm::dot(hitPoint.surface_normal, light_dir) * hitPoint.surface_normal - light_dir;
		float cos_specular_angle = glm::dot(reflected_light, hitPoint.ray_direction * -1.0f);

		if (cos_specular_angle <= 0) {
			continue;
		}
		Color light_intensity = light.color * light.brightness;
		specular_color += light_intensity * hitPoint.hit_material->ks * pow(cos_specular_angle, hitPoint.hit_material->m);
	}
	return specular_color;
}

Color Renderer::reflection_color(HitPoint const& hitPoint, Scene const& scene, unsigned ray_bounces) const {
	if (ray_bounces >= max_ray_bounces_) {
		return {};
	}
	glm::vec3 ray_dir = hitPoint.ray_direction;
	float cos_incidence_angle = glm::dot(hitPoint.surface_normal, ray_dir);
	glm::vec3 reflect_dir = ray_dir - 2 * cos_incidence_angle * hitPoint.surface_normal;
	return trace_color({hitPoint.position, reflect_dir}, scene, ray_bounces + 1);
}

Color Renderer::refraction_color(HitPoint const& hit_point, Scene const& scene, unsigned ray_bounces) const {
	glm::vec3 ray_dir = hit_point.ray_direction;
	glm::vec3 normal = hit_point.surface_normal;
	float eta = 1 / hit_point.hit_material->ior;
	float cos_incoming = -glm::dot(normal, ray_dir);

	//inverts negative incoming angle and normal vector if surface is hit from behind
	if (cos_incoming < 0) {
		eta = 1 / eta;
		cos_incoming *= -1;
		normal *= -1;
	}
	float cos_outgoing_squared = 1 - eta * eta * (1 - cos_incoming * cos_incoming);

	//returns total reflection if critical angle is reached
	if (cos_outgoing_squared < 0) {
		return reflection_color(hit_point, scene, ray_bounces);
	}else {
		//glm::vec3 refract_dir = glm::refract(ray_dir, normal, eta);
		glm::vec3 refract_dir = ray_dir * eta + normal * (eta * cos_incoming - sqrtf(cos_outgoing_squared));
		Ray refract_ray {hit_point.position - normal * (2 * EPSILON), refract_dir};
		return trace_color(refract_ray, scene, ray_bounces + 1) * hit_point.hit_material->kd;
	}
}

Color Renderer::normal_color(HitPoint const& hitPoint) {
	return Color {
			(hitPoint.surface_normal.x + 1) / 2,
			(hitPoint.surface_normal.y + 1) / 2,
			(hitPoint.surface_normal.z + 1) / 2
	};
}

Color Renderer::tone_map_color(Color color) const {
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



