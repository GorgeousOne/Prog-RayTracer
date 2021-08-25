/* What do we need:
* Datenstruktur, die Strings mit Zeitintervallen speichern kann
* Funktion sdf-writer mithilfe der Datenstruktur
* Funktion, die in einem Zeitintervall die Transformationen für Objekte berechnet und z.B. in einer map speichert
* Klasse zur Darstellung von Superhot
* Funktion dieser Klasse zur Bewegung von Superhot (Muss string mit Positionen zurückgeben)
* Ostream Funktion für die Datenstrukturen zum SDF schreiben
* Kamerabewegung und Position benötigt evtl auch eigene Datenstruktur
*/

#include <iostream>
#include <iomanip>
#include <fstream> 
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

struct Interval {
	float start_time = 0.0f;
	float duration = 1.0f;

	bool is_active(float current_time) const {
		return current_time >= start_time && current_time < start_time + duration;
	}
};

struct Animation {
	std::string name;
	std::string type;
	float start1;
	float start2;
	float start3;
	float end1;
	float end2;
	float end3;
	Interval time;
};

struct Cam_Animation {
	glm::vec3 pos_start;
	glm::vec3 pos_end;
	glm::vec3 direction_start;
	glm::vec3 direction_end;
	glm::vec3 up_start;
	glm::vec3 up_end;
	Interval time;
	float fov = 60.0f;
};

std::string get_current_transform(Animation const& animation, float current_time) {

	float progress = (current_time - animation.time.start_time) / animation.time.duration;
	float change_1 = (animation.end1 - animation.start1) * progress;
	float change_2 = (animation.end2 - animation.start2) * progress;
	float change_3 = (animation.end3 - animation.start3) * progress;

	float new_v1 = animation.start1 + change_1;
	float new_v2 = animation.start2 + change_2;
	float new_v3 = animation.start3 + change_3;

	std::stringstream current_transform;
	current_transform << "transform " << animation.name << " " << animation.type << " ";
	current_transform << new_v1 << " " << new_v2 << " " << new_v3;
	
	return current_transform.str();
}

std::string get_current_cam(Cam_Animation const& cam, float current_time) {

	float progress = (current_time - cam.time.start_time) / cam.time.duration;
	glm::vec3 position = (cam.pos_end - cam.pos_start) * progress;
	glm::vec3 direction = (cam.direction_end - cam.direction_start) * progress;
	glm::vec3 up = (cam.up_end - cam.up_start) * progress;

	glm::vec3 new_pos = cam.pos_start + position;
	glm::vec3 new_dir = cam.direction_start + direction;
	glm::vec3 new_up = cam.up_start + up;

	std::stringstream current_transform;
	current_transform << "define camera eye " << cam.fov << " ";
	current_transform << new_pos.x << " " << new_pos.y << " " << new_pos.z << " ";
	current_transform << new_dir.x << " " << new_dir.y << " " << new_dir.z << " ";
	current_transform << new_up.x << " " << new_up.y << " " << new_up.z;
	
	return current_transform.str();
}



void write_to_sdf(std::map<std::string, Interval> const& object_map, std::vector<Animation> const& animations, std::vector<Cam_Animation> const& cam_animations, unsigned fps, float movie_duration, std::string const& directory) {
	float frame_duration = 1.0f / fps;
	int total_frame_count = movie_duration * fps;

	for (int frame = 0; frame < total_frame_count; ++frame) {
		float current_time = frame * frame_duration;

		std::stringstream file_name;
		file_name << "frame" << std::setfill('0') << std::setw(4) << frame+1 << ".sdf";
		std::ofstream sdf_file(directory + "/" + file_name.str());

		for (auto const& object : object_map) {
			if (object.second.is_active(current_time)) {
				sdf_file << object.first << std::endl;
			}
		}
		sdf_file << std::endl;

		for (auto const& animation : animations) {
			if (animation.time.is_active(current_time)) {
				sdf_file << get_current_transform(animation, current_time) << std::endl;
			}
		}
		sdf_file << std::endl;

		for (auto const& animation : cam_animations) {
			sdf_file << get_current_cam(animation, current_time) << std::endl;
		}
		sdf_file.close();
	};
}

void generate_movie() {
	float movie_duration = 20.0f;
	unsigned fps = 24;
	std::string directory = "./movie/files";

	std::map<std::string, Interval> object_map;
	std::vector<Animation> animations;
	std::vector<Cam_Animation> cam_animations;


	write_to_sdf(object_map, animations, cam_animations, fps, movie_duration, directory);
}

int main() {
	return 0;
}