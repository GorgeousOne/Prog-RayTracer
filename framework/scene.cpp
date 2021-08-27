#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

#include "scene.hpp"
#include "sphere.hpp"
#include "renderer.hpp"

std::shared_ptr<Material> Scene::find_mat(std::string const& name) const {
	return materials.find(name)->second;
}

std::shared_ptr<Shape> Scene::find_shape(std::string const& name) const {
	return root->find_child(name);
}

glm::vec3 load_vec(std::istringstream& arg_stream) {
	glm::vec3 v;
	arg_stream >> v.x >> v.y >> v.z;
	return v;
}

std::shared_ptr<Material> load_mat(std::istringstream& arg_stream) {
	std::string name;
	float m;
	float glossy;
	float opacity;
	float ior;

	arg_stream >> name;
	glm::vec3 ka = load_vec(arg_stream);
	glm::vec3 kd = load_vec(arg_stream);
	glm::vec3 ks = load_vec(arg_stream);
	arg_stream >> m;
	arg_stream >> glossy;
	arg_stream >> opacity;
	arg_stream >> ior;

	return std::make_shared<Material>(Material{name, ka, kd, ks, m, glossy, opacity, ior});
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
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;

	arg_stream >> name;
	arg_stream >> fov_x;
	arg_stream >> pos.x >> pos.y >> pos.z;
	arg_stream >> dir.x >> dir.y >> dir.z;
	arg_stream >> up.x >> up.y >> up.z;

	return {name, fov_x, pos, glm::normalize(dir), glm::normalize(up)};
}

void load_transformation(std::istringstream& arg_stream, Scene const& scene) {
	std::string name;
	
	arg_stream >> name;
	auto shape = scene.find_shape(name);

	std::string token;
	arg_stream >> token;
	if ("translate" == token) {
		float dx;
		float dy;
		float dz;

		arg_stream >> dx >> dy >> dz;
		shape->translate(dx, dy, dz);
	}
	//rotation with euler angles
	else if ("rotate" == token) {
		float pitch;
		float yaw;
		float roll;

		arg_stream >> pitch >> yaw >> roll;
		shape->rotate(glm::radians(roll), glm::radians(pitch), glm::radians(yaw));
	}
	else if ("scale" == token) {
		float scale_x;
		float scale_y;
		float scale_z;

		arg_stream >> scale_x >> scale_y >> scale_z;
		shape->scale(scale_x, scale_y, scale_z);
	}
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
	std::ifstream input_obj_file(directory_path + "/" + name + ".obj");
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

		if ('#' == arg_stream.peek()) {
			continue;
		}
		std::string token;
		arg_stream >> token;

		//loads associated material file
		if ("mtllib" == token) {
			std::string mtl_file_name;
			arg_stream >> mtl_file_name;
			materials = load_obj_materials(directory_path + "/" + mtl_file_name);
			//creates new sub object
		} else if ("o" == token) {
			//adds the previously composed mesh after all faces have been added so it's min max bounds are calculated correctly
			if (composite->get_name() != current_child->get_name()) {
				current_child->build_octree();
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
			current_child->add_child(load_obj_face(arg_stream, vertices, normals, "face" + std::to_string(face_count), child_mat));
			++face_count;
		}
	}
	if (composite->get_name() != current_child->get_name()) {
		current_child->build_octree();
		composite->add_child(current_child);
	}
	composite->build_octree();
	return composite;
};

void create_composite(std::istringstream& arg_stream, Scene& new_scene) {
	std::string composite_name;
	arg_stream >> composite_name;
	auto composite = std::make_shared<Composite>(composite_name);

	while(!arg_stream.eof()) {
		std::string child_name;
		arg_stream >> child_name;
		auto child_shape = new_scene.find_shape(child_name);
		composite->add_child(child_shape);
		new_scene.root->remove_child(child_name);
	}
	composite->build_octree();
	new_scene.root->add_child(composite);
}

void add_to_composite(std::istringstream& arg_stream, Scene& new_scene) {
	std::string composite_name;
	arg_stream >> composite_name;
	auto composite = std::dynamic_pointer_cast<Composite>(new_scene.find_shape(composite_name));
	std::cout << "adding to  " << composite->get_name() << "\n";

	if (nullptr == composite) {
		std::cerr << composite_name << " cannot be cast to composite" << std::endl;
	}
	while(!arg_stream.eof()) {
		std::string child_name;
		arg_stream >> child_name;
		auto child_shape = new_scene.find_shape(child_name);
		composite->add_child(child_shape);
		new_scene.root->remove_child(child_name);
		std::cout << "added " << child_name << "\n";
	}
	composite->build_octree();
}

void add_to_scene(std::istringstream& arg_stream, Scene& new_scene, std::string const& resource_directory) {
	std::string token;
	arg_stream >> token;

	if ("material" == token) {
		auto new_mat = load_mat(arg_stream);
		new_scene.materials.emplace(new_mat->name, new_mat);
	}
	if ("shape" == token) {
		arg_stream >> token;
		if ("box" == token) {
			new_scene.root->add_child(load_box(arg_stream, new_scene.materials));
		} else if ("sphere" == token) {
			new_scene.root->add_child(load_sphere(arg_stream, new_scene.materials));
		} else if ("triangle" == token) {
			new_scene.root->add_child(load_triangle(arg_stream, new_scene.materials));
		} else if ("obj" == token) {
			std::string obj_file_name;
			arg_stream >> obj_file_name;
			new_scene.root->add_child(load_obj(resource_directory, obj_file_name));
		} else if ("composite" == token) {
			create_composite(arg_stream, new_scene);
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

void render(std::istringstream& arg_stream, Scene const& scene, std::string const& output_directory) {
	std::string file_name;
	unsigned res_x;
	unsigned res_y;
	unsigned aa_steps;
	unsigned ray_bounces;

	arg_stream >> file_name;
	arg_stream >> res_x;
	arg_stream >> res_y;
	arg_stream >> aa_steps;
	arg_stream >> ray_bounces;

	Renderer renderer{res_x, res_y, output_directory + "/" + file_name, aa_steps, ray_bounces};
	renderer.render(scene, scene.camera);
}

Scene load_scene(
		std::string const& file_path,
		std::string const& resource_directory,
		std::string const& output_directory) {

	Scene new_scene{};
	std::ifstream input_sdf_file(file_path);
	std::string line_buffer;

	//writes sdf file line by line into line_buffer
	while (std::getline(input_sdf_file, line_buffer)) {
		std::istringstream words_stream(line_buffer);
		//streams words of each line
		std::string token;
		words_stream >> token;

		if ("#" == token) {
			continue;
		}
		if ("define" == token) {
			add_to_scene(words_stream, new_scene, resource_directory);
		} else if ("transform" == token) {
			load_transformation(words_stream, new_scene);
		} else if ("render" == token) {
			render(words_stream, new_scene, output_directory);
		} else if ("add" == token) {
			add_to_composite(words_stream, new_scene);
		}
	}
	new_scene.root->build_octree();
	return new_scene;
}
