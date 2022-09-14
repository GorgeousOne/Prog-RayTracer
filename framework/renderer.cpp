// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#include <chrono>
#include <iomanip>
#include <thread>
#include "renderer.hpp"

#define PI 3.14159265f
#define EPSILON 0.001f

Renderer::Renderer(unsigned w, unsigned h, std::string const& file, unsigned aa_steps, unsigned ray_bounces, unsigned int samples) :
		width_(w),
		height_(h),
		color_buffer_(w * h, Color{0.0, 0.0, 0.0}),
		filename_(file),
		ppm_(w, h),
		aa_steps_(aa_steps),
		max_ray_bounces_(ray_bounces),
		samples_(samples) {

	gen_ = std::minstd_rand(std::random_device{}());
	dist_ = std::uniform_real_distribution<float>(-1, 1);
}

void Renderer::render(Scene const& scene, Camera const& cam) {
	float img_plane_dist = (height_ / 2.0f) / tan(cam.fov_x / 2);

	std::cout << cam.fov_x << " fov\n" ;
	glm::vec3 u = glm::cross(cam.direction, cam.up);
	glm::vec3 v = glm::cross(u, cam.direction);
	glm::mat4 trans_mat{
			glm::vec4{u, 0},
			glm::vec4{v, 0},
			glm::vec4{cam.direction, 0},
			glm::vec4{cam.position, 1}
	};

	// gets amount of parallel threads supported by hardware
	size_t core_count = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	threads.resize(core_count);
	std::cout << "using " << core_count << " threads to render\n";
	std::cout << std::fixed;
	std::cout << std::setprecision(2);

	pixel_index_ = 0;
	progress = 0;

	for (std::thread& t : threads) {
		t = std::thread(&Renderer::thread_function, this, scene, img_plane_dist, trans_mat);
	}
	for (std::thread& t : threads) {
		t.join();
	}
	ppm_.save(filename_);
}

void Renderer::thread_function(Scene const& scene, float img_plane_dist, glm::mat4 const& trans_mat) {
	float aa_unit = 1.0f / aa_steps_;

	while(true) {
		unsigned current_pixel = pixel_index_++;
		unsigned x = current_pixel % width_;
		unsigned y = current_pixel / width_;

		if (current_pixel >= width_ * height_) {
			return;
		}
		Pixel pixel{ x, y };

		for (int aax = 0; aax < aa_steps_; ++aax) {
			for (int aay = 0; aay < aa_steps_; ++aay) {
				glm::vec3 pixel_pos = glm::vec3{
						x + aax * aa_unit - width_ * 0.5f,
						y + aay * aa_unit - height_ * 0.5f,
						img_plane_dist};

				glm::vec4 trans_ray_dir = trans_mat * glm::vec4{glm::normalize(pixel_pos), 0};
				Ray ray{glm::vec3{trans_mat[3]}, glm::vec3{trans_ray_dir}};
				pixel.color += primary_trace(ray, scene, samples_);
			}
		}
		pixel.color *= 1.0f / (samples_ * aa_steps_ * aa_steps_);
		pixel.color = tone_map_color(pixel.color);
		write(pixel);

		if (x == 0 && y >= (progress + 0.01) * height_) {
			std::cout << static_cast<int>(100 * progress) << std::endl;
			progress = progress + 0.01f;
		}
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

Color Renderer::primary_trace(Ray const& ray, Scene const& scene, unsigned int samples) {
	HitPoint closest_hit = get_closest_hit(ray, scene);

	if (!closest_hit.does_intersect) {
		return Color{};
	}
//	std::cout << shade(closest_hit, scene, samples_, 0) << "hit\n";
	return shade(closest_hit, scene, samples, 0);
}

Color Renderer::trace(Ray const& ray, Scene const& scene, unsigned samples, unsigned ray_bounces) {
	HitPoint closest_hit = get_closest_hit(ray, scene);
	return closest_hit.does_intersect ? shade(closest_hit, scene, samples, ray_bounces) : Color {};
}

HitPoint Renderer::get_closest_hit(Ray const& ray, Scene const& scene) const {
	HitPoint closest_hit{};
	HitPoint hit = scene.root->intersect(ray);

	if (!hit.does_intersect) {
		return hit;
	}
	if (!closest_hit.does_intersect || hit.distance < closest_hit.distance) {
		closest_hit = hit;
	}
	return closest_hit;
}

template <class T, class T2>
glm::vec3 uniform_normal(T& distribution, T2& generator) {
	float yaw = distribution(generator) * PI;
	float pitch = asin(distribution(generator));
	float cos_pitch = cos(pitch);

	return glm::vec3{
			cos_pitch * cos(yaw),
			sin(pitch),
			cos_pitch * sin(yaw)
	};
}

Color Renderer::shade(HitPoint const& hit_point, Scene const& scene, unsigned samples, unsigned ray_bounces) {
	if (ray_bounces >= max_ray_bounces_) {
		return {};
	}
	auto material = hit_point.hit_material;
	Color bounced_light {}; // = diffuse(hit_point, scene, samples, ray_bounces);

	if (material->glossy > 0 && material->opacity < 1) {
		float reflectance = schlick_reflection_ratio(hit_point.ray_direction, hit_point.surface_normal, material->ior);
		bounced_light += reflection(hit_point, scene, samples, ray_bounces) * reflectance;
		bounced_light += refraction(hit_point, scene, samples, ray_bounces) * (1 - reflectance) * (1 - material->opacity);
	} else if (material->glossy > 0) {
		float reflectance = schlick_reflection_ratio(hit_point.ray_direction, hit_point.surface_normal, material->ior);
		reflectance = material->glossy + (1 - material->glossy) * reflectance;

		if (reflectance < 1) {
			bounced_light += diffuse(hit_point, scene, samples, ray_bounces);
			bounced_light *= 1 - reflectance;
		}
		bounced_light += reflection(hit_point, scene, samples, ray_bounces) * reflectance;
	} else if (material->opacity < 1) {
		bounced_light += refraction(hit_point, scene, samples, ray_bounces) * (1 - material->opacity);
	}else {
		bounced_light += diffuse(hit_point, scene, samples, ray_bounces);
	}
	return bounced_light;
}

Color Renderer::diffuse(HitPoint const& hit_point, Scene const& scene, unsigned samples, unsigned ray_bounces) {
	Color bounced_light {};
	auto material = hit_point.hit_material;

	for (unsigned i = 0; i < samples; ++i) {
		glm::vec3 bounce_dir = uniform_normal(dist_, gen_);
		float cos_theta = glm::dot(hit_point.surface_normal, bounce_dir);

		if (cos_theta < 0) {
			bounce_dir *= -1;
			cos_theta *= -1;
		}
		bounced_light += trace({hit_point.position, bounce_dir}, scene, 1, ray_bounces + 1) * 2 * cos_theta;
	}
	if (samples > 1) {
		bounced_light *= 1.0f / samples;
	}
	return material->emit_color + bounced_light * material->kd;
}

Color Renderer::reflection(HitPoint const& hit_point, Scene const& scene, unsigned samples, unsigned ray_bounces) {
	glm::vec3 ray_dir = hit_point.ray_direction;
	glm::vec3 normal = hit_point.surface_normal;

	float cos_incoming = -glm::dot(normal, ray_dir);
	glm::vec3 reflect_dir = ray_dir + (normal * cos_incoming * 2.0f);

	Color reflected_light {};
	Ray reflect_ray = {hit_point.position, reflect_dir};
	reflected_light += trace(reflect_ray, scene, samples, ray_bounces + 1);
	return reflected_light * hit_point.hit_material->ks;
}

Color Renderer::refraction(HitPoint const& hit_point, Scene const& scene, unsigned samples, unsigned ray_bounces) {
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
	Color refracted_light {};

	//returns total reflection if critical angle is reached
	if (cos_outgoing_squared < 0) {
		refracted_light += reflection(hit_point, scene, samples, ray_bounces);
	}else {
		glm::vec3 refract_dir = ray_dir * eta + normal * (eta * cos_incoming - sqrtf(cos_outgoing_squared));
		Ray refract_ray {hit_point.position - normal * (2 * EPSILON), refract_dir};
		refracted_light += trace(refract_ray, scene, samples, ray_bounces + 1);
	}
	return refracted_light * hit_point.hit_material->kd;
}

//https://blog.demofox.org/2017/01/09/raytracing-reflection-refraction-fresnel-total-internal-reflection-and-beers-law/
//https://en.wikipedia.org/wiki/Schlick%27s_approximation
float Renderer::schlick_reflection_ratio(glm::vec3 const& ray_dir, glm::vec3 const& normal, float const& ior) const {
	float n1 = 1;
	float n2 = ior;
	float cos_incoming = -glm::dot(normal, ray_dir);

	if (cos_incoming < 0) {
		std::swap(n1, n2);
	}
	if (n1 > n2) {
		float eta = n1 / n2;
		float sin_outgoing_squared = eta * eta * (1 - cos_incoming * cos_incoming);

		if (sin_outgoing_squared >= 1) {
			return 1;
		}
		cos_incoming = sqrtf(1 - sin_outgoing_squared);
	}

	float min_reflectance = (1 - ior) / (1 + ior);
	min_reflectance *= min_reflectance;

	float factor = 1 - cos_incoming;
	float ratio = min_reflectance + (1 - min_reflectance) * factor * factor * factor * factor * factor;
	return ratio;
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