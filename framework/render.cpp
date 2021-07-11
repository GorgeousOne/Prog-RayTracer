#include "render.hpp"

#define PI 3.14159265f

void Render::render(Scene const& scene) const {
	float aspect_ratio = 1.0f * res_x / res_y;
	float fov_radians = camera.fov_x / 180 * PI;
	float width = 2 * tan(fov_radians / 2);
	float height = width / aspect_ratio;

	glm::vec3 min_corner {-width / 2, -height / 2, -1};
	glm::vec3 pixel_width {width / res_x, 0, 0};
	glm::vec3 pixel_height {0, height / res_y, 0};

	for (float x = 0; x < res_x; ++x) {
		for (float y = 0; y < res_y; ++y) {
			glm::vec3 ray_dir {min_corner + pixel_width * x + pixel_height * y};
			Ray ray {camera.position, glm::normalize(ray_dir)};
		}
	}
}
