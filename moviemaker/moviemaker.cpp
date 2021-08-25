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
# include <map>
# include <string>
#include <sstream>
# include <vector>

struct Interval {
	float start_time = 0.0f;
	float duration = 1.0f;

	bool is_active(float current_time) {
		return current_time >= start_time && current_time < start_time + duration;
	}
};

struct Transformation {
	std::string name;
	std::string type;
	float v1;
	float v2;
	float v3;
	float time_stamp;
};

std::vector<std::string> get_transform_steps(Transformation t1, Transformation t2, float frame_duration) {
	if (t1.name != t2.name || t1.type != t2.type || t1.time_stamp > t2.time_stamp) {
		return {};
	}
	int number_of_frames = (t2.time_stamp - t1.time_stamp) / frame_duration;
	float change_1 = (t2.v1 - t1.v1) / number_of_frames;
	float change_2 = (t2.v2 - t1.v2) / number_of_frames;
	float change_3 = (t2.v3 - t1.v3) / number_of_frames;

	float new_v1 = t1.v1;
	float new_v2 = t1.v2;
	float new_v3 = t1.v3;

	std::vector<std::string> transformations;
	for (int i = 0; i < number_of_frames; ++i) {
		new_v1 += change_1;
		new_v2 += change_2;
		new_v3 += change_3;

		std::stringstream ss;
		ss << "transform " << t1.name << " " << t1.type << " ";
		ss << new_v1 << " " << new_v2 << " " << new_v3;
		transformations.push_back(ss.str());
		std::cout << ss.str() << std::endl;
	}
	return transformations;
}

void write_to_sdf(std::map<std::string, Interval> object_map, unsigned fps, float movie_duration, std::string directory) {
	float frame_duration = 1.0f / fps;
	int total_frame_count = movie_duration * fps;

	for (int frame = 0; frame < total_frame_count; ++frame) {
	
	};
}

void generate_movie() {
	float movie_duration = 20.0f;
	unsigned fps = 24;
	std::string directory = "./movie/files";

	std::map<std::string, Interval> object_map;
	std::vector<std::pair<Transformation, Transformation>> transform_vec;


	write_to_sdf(object_map, fps, movie_duration, directory);
}

int main() {
	Transformation t1{"ball", "translate", 0, 0, 0, 0.0f};
	Transformation t2{"ball", "translate", 1, 1, 1, 1.0f};
	get_transform_steps(t1, t2, (1.0f / 24.0f));
	return 0;
}