#include "config_reader.h"
#include <fstream>
#include <iostream>
#include <string>

config_reader* config_reader::loaded_file_instance = nullptr;
config_reader::config_reader() : read_file() {
	loaded_file_instance = this;
}

int config_reader::read_config(std::filesystem::path path) {
	std::ifstream in(path);
	try {
		

		if (!read_file.keycode_show) {
			std::tuple<std::string, std::string> param;
			std::map<int, std::string> cur_preset;
			do {
				param = get_param(in);

				//std::cout << std::get<0>(param) << "\n";
				if (std::get<0>(param) == "config" || std::get<0>(param) == "") continue;
				if (std::get<0>(param) == "EnumShowMode") {
					read_file.enum_show = std::stoi(std::get<1>(param));
				}
				else if (std::get<0>(param) == "KeyCodeShowMode") {
					read_file.keycode_show = std::stoi(std::get<1>(param));
				}
				else if (std::get<0>(param) == "SelectedDevice") {
					read_file.selected_device = std::stoi(std::get<1>(param));
				}
				else if (std::get<0>(param) == "ForceMode") {
					read_file.force_mode = std::stoi(std::get<1>(param));
				}
				else if (std::get<0>(param) == "DefaultSelectedPreset") {
					read_file.selected_preset = std::stoi(std::get<1>(param));
				}
				else if (std::get<0>(param) == "EndPreset") {
					read_file.presets_keycode_path_pairs.emplace(std::stoi(std::get<1>(param)), cur_preset);
					cur_preset.clear();
				}
				else {
					cur_preset.emplace(std::stoi(std::get<0>(param)), std::get<1>(param));
				}
			} while (std::get<0>(param) != "");
		}
	}
	catch (std::exception & e) {
		std::cout << e.what() << "\n";
		return -1;
	}
	return 0;
}

config_file* config_reader::get_config_file() {
	return &this->read_file;
}

std::tuple<std::string, std::string> config_reader::get_param(std::ifstream& in) {
	std::string cur_line;
	if (std::getline(in, cur_line)) {
		size_t comment = cur_line.find("//");
		if (comment != std::string::npos) return { "config", "" };
		size_t pos = cur_line.find("=");
		if (pos != std::string::npos) {
			return { cur_line.substr(0, pos), cur_line.substr(pos + 1) };
		}
	}
	return {"", ""};
}