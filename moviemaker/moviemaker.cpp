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
# include <map>
# include <string>
#include <sstream>
# include <vector>

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

std::string get_current_transform(Animation const& a1, float current_time) {

	float progress = (current_time - a1.time.start_time) / a1.time.duration;
	float change_1 = (a1.end1 - a1.start1) * progress;
	float change_2 = (a1.end2 - a1.start2) * progress;
	float change_3 = (a1.end3 - a1.start3) * progress;

	float new_v1 = a1.start1 + change_1;
	float new_v2 = a1.start2 + change_2;
	float new_v3 = a1.start3 + change_3;

	std::stringstream current_transform;
	current_transform << "transform " << a1.name << " " << a1.type << " ";
	current_transform << new_v1 << " " << new_v2 << " " << new_v3;
	
	return current_transform.str();
}

void write_to_sdf(std::map<std::string, Interval> const& object_map, std::vector<Animation> const& animations, unsigned fps, float movie_duration, std::string const& directory) {
	float frame_duration = 1.0f / fps;
	int total_frame_count = movie_duration * fps;

	for (int frame = 0; frame < total_frame_count; ++frame) {
		float current_time = frame * frame_duration;

		std::stringstream file_name;
		file_name << "frame" << std::setfill('0') << std::setw(4) << frame+1 << ".sdf";
		std::ofstream sdf_file(directory + "/" + file_name.str());

		for (auto object : object_map) {
			if (object.second.is_active(current_time)) {
				sdf_file << object.first << std::endl;
			}
		}

		for (auto animation : animations) {
			if (animation.time.is_active(current_time)) {
				sdf_file << get_current_transform(animation, current_time) << std::endl;
			}
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


	write_to_sdf(object_map, animations, fps, movie_duration, directory);
}

int main() {
	return 0;
}