#ifndef CONFIG_READER_H_
#define CONFIG_READER_H_
#include <filesystem>
#include <map>
#include <vector>
#include <tuple>

struct config_file {
	bool enum_show;
	bool keycode_show;
	bool force_mode;
	unsigned int selected_device;
	unsigned int selected_preset;
	std::map<int, std::map<int, std::string>> presets_keycode_path_pairs;
};

class config_reader {
public:
	config_reader();
	int read_config(std::filesystem::path path);
	config_file* get_config_file();
	static config_reader* get_instance() { return loaded_file_instance; }
private:
	config_file read_file;
	static config_reader* loaded_file_instance;
	std::tuple<std::string, std::string> get_param(std::ifstream& in);
};

#endif // CONFIG_READER_H_