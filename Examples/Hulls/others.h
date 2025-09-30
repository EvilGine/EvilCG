#ifndef OTHERS_H
#define OTHERS_H
#include <fstream>
#include <sstream>
#include <string>

#include <ecg_api.h>

std::string convert_bb_to_str(const ecg::full_bounding_box* bb) {
	std::stringstream ss;
	if (bb == nullptr) return "";

	ss << "v " << bb->p0.x << " " << bb->p0.y << " " << bb->p0.z << "\n";
	ss << "v " << bb->p1.x << " " << bb->p1.y << " " << bb->p1.z << "\n";
	ss << "v " << bb->p2.x << " " << bb->p2.y << " " << bb->p2.z << "\n";
	ss << "v " << bb->p3.x << " " << bb->p3.y << " " << bb->p3.z << "\n";
	ss << "v " << bb->p4.x << " " << bb->p4.y << " " << bb->p4.z << "\n";
	ss << "v " << bb->p5.x << " " << bb->p5.y << " " << bb->p5.z << "\n";
	ss << "v " << bb->p6.x << " " << bb->p6.y << " " << bb->p6.z << "\n";
	ss << "v " << bb->p7.x << " " << bb->p7.y << " " << bb->p7.z << "\n";

	ss << "f 1 2 3 4\n";
	ss << "f 5 6 7 8\n";
	ss << "f 1 2 6 5\n";
	ss << "f 2 3 7 6\n";
	ss << "f 3 4 8 7\n";
	ss << "f 4 1 5 8\n";

	return ss.str();
}

void save_bb_to_obj(const ecg::full_bounding_box* bb, std::string obj_file) {
	std::ofstream file;
	file.open(obj_file);

	if (file.is_open()) {
		file << convert_bb_to_str(bb);
	}

	file.close();
}

void save_bb_to_obj(const ecg::bounding_box* bb, std::string obj_file) {
	ecg::full_bounding_box full_bb = ecg::hulls::expand_bb(bb);
	save_bb_to_obj(&full_bb, obj_file);
}


#endif