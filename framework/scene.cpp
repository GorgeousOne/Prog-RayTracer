#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#include "scene.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "composite.hpp"
std::shared_ptr<Material> Scene::find_mat(std::string const& name) const {
	return materials.find(name)->second;
}

glm::vec3 load_vec(std::istringstream& arg_stream) {
	glm::vec3 v;
	arg_stream >> v.x >> v.y >> v.z;
	return v;
}

std::shared_ptr<Material> load_mat(std::istringstream& arg_stream) {
	std::string name;
	float brightness;

	arg_stream >> name;
	glm::vec3 ka = load_vec(arg_stream);
	glm::vec3 kd = load_vec(arg_stream);
	glm::vec3 ks = load_vec(arg_stream);
	arg_stream >> brightness;
	return std::make_shared<Material>(Material{name, ka, kd, ks, brightness});
}

std::shared_ptr<Box> load_box(std::istringstream& arg_stream, std::map<std::string, std::shared_ptr<Material>> const& materials) {
	std::string name;
	std::string mat_name;

	arg_stream >> name;
	glm::vec3 min = load_vec(arg_stream);
	glm::vec3 max = load_vec(arg_stream);
	arg_stream >> mat_name;

	auto it = materials.find(mat_name);
	return std::make_shared<Box>(min, max, name, it->second);
}

std::shared_ptr<Sphere> load_sphere(std::istringstream& arg_stream, std::map<std::string, std::shared_ptr<Material>> const& materials) {
	std::string name;
	std::string mat_name;
	float radius;

	arg_stream >> name;
	glm::vec3 center = load_vec(arg_stream);
	arg_stream >> radius;
	arg_stream >> mat_name;

	auto it = materials.find(mat_name);
	return std::make_shared<Sphere>(radius, center, name, it->second);
}

std::shared_ptr<Triangle> load_triangle(std::istringstream& arg_stream, std::map<std::string, std::shared_ptr<Material>> const& materials) {
	std::string name;
	std::string mat_name;

	arg_stream >> name;
	glm::vec3 v0 = load_vec(arg_stream);
	glm::vec3 v1 = load_vec(arg_stream);
	glm::vec3 v2 = load_vec(arg_stream);
	arg_stream >> mat_name;

	auto it = materials.find(mat_name);
	return std::make_shared<Triangle>(v0, v1, v2, name, it->second);
}

PointLight load_point_light(std::istringstream& arg_stream) {
	std::string name;
	Color color;
	float brightness;

	arg_stream >> name;
	glm::vec3 pos = load_vec(arg_stream);
	arg_stream >> color.r >> color.g >> color.b;
	arg_stream >> brightness;

	return {name, color, brightness, pos};
}

Light load_ambient(std::istringstream& arg_stream) {
	std::string name;
	Color color;
	float brightness;

	arg_stream >> name;
	arg_stream >> color.r >> color.b >> color.g;
	arg_stream >> brightness;

	return {name, color, brightness};
}

Camera load_camera(std::istringstream& arg_stream) {
	std::string name;
	float fov_x;

	arg_stream >> name;
	arg_stream >> fov_x;

	return {name, fov_x};
}

std::map<std::string, std::shared_ptr<Material>> load_obj_materials(std::string const& file_path) {
	std::ifstream input_mtl_file(file_path);
	std::string line_buffer;

	auto materials = std::map<std::string, std::shared_ptr<Material>>{};
	std::shared_ptr<Material> current_mat = nullptr;

	while (std::getline(input_mtl_file, line_buffer)) {
		std::istringstream arg_stream(line_buffer);
		std::string token;
		arg_stream >> token;

		if ("#" == token) {
			continue;
		}
		if ("newmtl" == token) {
			current_mat = std::make_shared<Material>();
			arg_stream >> current_mat->name;
			materials.emplace(current_mat->name, current_mat);
		} else if ("Ka" == token) {
			current_mat->ka = load_vec(arg_stream);
		} else if ("Kd" == token) {
			current_mat->kd = load_vec(arg_stream);
		} else if ("Ks" == token) {
			current_mat->ks = load_vec(arg_stream);
		} else if ("Ns" == token) {
			arg_stream >> current_mat->m;
		}
	}
	return materials;
}

std::shared_ptr<Triangle> load_obj_face(
		std::istringstream& arg_stream,
		std::vector<glm::vec3> const& vertices,
		std::vector<glm::vec3> const& normals,
		std::string name,
		std::shared_ptr<Material> mat) {
	unsigned indices_v[3];
	unsigned indices_vt[3];
	unsigned indices_vn[3];
	bool has_normals = false;

	for (int i = 0; i < 3; ++i) {
		std::string index_group;
		arg_stream >> index_group;
		std::stringstream index_stream(index_group);

		index_stream >> indices_v[i];
		index_stream.ignore();
		//skips texture coordinates if non provided for a face
		if ('/' != index_stream.peek()) {
			index_stream >> indices_vt[i];
		}
		//reads face normals if provided
		if (-1 != index_stream.peek()) {
			index_stream.ignore();
			index_stream >> indices_vn[i];
			has_normals = true;
		}
	}
	if (has_normals) {
		return std::make_shared<Triangle>(
				vertices[indices_v[0] - 1],
				vertices[indices_v[1] - 1],
				vertices[indices_v[2] - 1],
				normals[indices_vn[0] - 1],
				name,
				mat);
	}else {
		return std::make_shared<Triangle>(
				vertices[indices_v[0] - 1],
				vertices[indices_v[1] - 1],
				vertices[indices_v[2] - 1],
				name,
				mat);
	}
}

/**
 * Loads blender generate .obj files where the order of inputs is vertices, normals, used material then faces
 * @param directory_path directory of the .obj file
 * @param name name of the .obj file
 * @return
 */
std::shared_ptr<Composite> load_obj(std::string const& directory_path, std::string const& name) {
	std::ifstream input_obj_file(directory_path + name + ".obj");
	std::string line_buffer;

	std::map<std::string, std::shared_ptr<Material>> materials;
	auto composite = std::make_shared<Composite>(name, nullptr);
	std::shared_ptr<Composite> current_child = composite;
	std::shared_ptr<Material> child_mat = std::make_shared<Material>();
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	int face_count = 0;

	while (std::getline(input_obj_file, line_buffer)) {
		std::istringstream arg_stream(line_buffer);
		std::string token;
		arg_stream >> token;

		if ("#" == token) {
			continue;
		}
		//loads associated material file
		if ("mtllib" == token) {
			std::string mtl_file_name;
			arg_stream >> mtl_file_name;
			materials = load_obj_materials(directory_path + mtl_file_name);
			//creates new sub object
		} else if ("o" == token) {
			//adds the previously composed mesh after all faces have been added so it's min max bounds are calculated correctly
			if (composite->get_name() != current_child->get_name()) {
				composite->add_child(current_child);
			}
			std::string child_name;
			arg_stream >> child_name;
			current_child = std::make_shared<Composite>(child_name, child_mat);
			//adds vertex
		} else if ("v" == token) {
			vertices.push_back(load_vec(arg_stream));
			//adds normal
		} else if ("vn" == token) {
			normals.push_back(load_vec(arg_stream));
			//selects material for following faces
		} else if ("usemtl" == token) {
			std::string mat_name;
			arg_stream >> mat_name;
			child_mat = materials.find(mat_name)->second;
			//adds a triangle face
		} else if ("f" == token) {
			current_child->add_child(load_obj_face(arg_stream, vertices, normals, std::to_string(face_count), child_mat));
			++face_count;
		}
	}
	if (composite->get_name() != current_child->get_name()) {
		composite->add_child(current_child);
	}
	return composite;
};

void render(std::istringstream& arg_stream) {
	std::string cam_name;
	std::string file_name;
	unsigned int res_x;
	unsigned int res_y;

	arg_stream >> cam_name;
	arg_stream >> file_name;
	arg_stream >> res_x;
	arg_stream >> res_y;

	//NOT IMPLEMENTED FOR THIS ASSIGNMENT
}

void add_to_scene(std::istringstream& arg_stream, Scene& new_scene) {
	std::string token;
	arg_stream >> token;

	if ("material" == token) {
		auto new_mat = load_mat(arg_stream);
		new_scene.materials.emplace(new_mat->name, new_mat);
	}
	if ("shape" == token) {
		arg_stream >> token;
		if ("box" == token) {
			auto new_box = load_box(arg_stream, new_scene.materials);
			new_scene.shapes.emplace(new_box->get_name(), new_box);
		} else if ("sphere" == token) {
			auto new_sphere = load_sphere(arg_stream, new_scene.materials);
			new_scene.shapes.emplace(new_sphere->get_name(), new_sphere);
		} else if ("triangle" == token) {
			auto new_triangle = load_triangle(arg_stream, new_scene.materials);
			new_scene.shapes.emplace(new_triangle->get_name(), new_triangle);
		} else if ("obj" == token) {
			std::string obj_file_name;
			arg_stream >> obj_file_name;
			auto new_composite = load_obj("../../sdf/", obj_file_name);
			new_scene.shapes.emplace(new_composite->get_name(), new_composite);
		}
	} else if ("light" == token) {
		PointLight new_light{load_point_light(arg_stream)};
		new_scene.lights.push_back(new_light);
	} else if ("ambient" == token) {
		new_scene.ambient = {load_ambient(arg_stream)};
	} else if ("camera" == token) {
		new_scene.camera = {load_camera(arg_stream)};
	}
}

Scene load_scene(std::string const& file_path) {
	Scene new_scene{};
	std::ifstream input_sdf_file(file_path);
	std::string line_buffer;

	//writes sdf file line by line into line_buffer
	while (std::getline(input_sdf_file, line_buffer)) {
		std::istringstream words_stream(line_buffer);
		//streams words of each line
		std::string token_str;
		words_stream >> token_str;

		if ("#" == token_str) {
			continue;
		}
		if ("define" == token_str) {
			add_to_scene(words_stream, new_scene);
		} else if ("render") {
			render(words_stream);
		}
	}
	return new_scene;
}
