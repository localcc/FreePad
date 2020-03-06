#include "WASAPIRenderer.h"
#include "config_reader.h"

#define FILENAME "rotzakryl48.wav"

LRESULT CALLBACK KeyCodeShowProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		std::cout << "KeyCode: " << ((KBDLLHOOKSTRUCT*)lParam)->vkCode << "\n";
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK KeybindProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN) {
		config_file* file = config_reader::get_instance()->get_config_file();
		KBDLLHOOKSTRUCT* hookStruct = (KBDLLHOOKSTRUCT*)lParam;
		auto switch_preset = file->presets_keycode_path_pairs.find(hookStruct->vkCode);
		if (switch_preset != file->presets_keycode_path_pairs.end()) {
			file->selected_preset = hookStruct->vkCode;
			printf("%d\n", file->selected_preset);
			return file->force_mode ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam);
		}
		
		std::thread([&] {
			auto* config_reader_instance = config_reader::get_instance();
			config_file* file = config_reader_instance->get_config_file();
			auto crr = file->presets_keycode_path_pairs.find(file->selected_preset);
			if (crr == file->presets_keycode_path_pairs.end()) {
				printf("FUCK!\n");
			}
			auto cur_preset = crr->second;
			auto iterator = cur_preset.find(((KBDLLHOOKSTRUCT*)lParam)->vkCode);
			if (iterator != cur_preset.end()) {
				WASAPIRenderer::get_instance()->play_file(iterator->second.c_str());
			}
		}).detach();
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	config_reader* reader = new config_reader();
	int res = reader->read_config(std::filesystem::path("config_file.cfg"));
	if (res < 0) {
		exit(-1);
	}
	config_file* file = reader->get_config_file();
	WASAPIRenderer* renderer = new WASAPIRenderer();

	renderer->enumerate_devices();
	if (file->enum_show) {
		renderer->show_enumerated_devices();
	}
	HHOOK keyboard_hook = nullptr;
	if (file->keycode_show) {
		keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyCodeShowProc, NULL, NULL);
		if (keyboard_hook == NULL) {
			std::cout << "Failed to hook!\n";
			exit(-2);
		}
	}
	else {
		renderer->select_device(file->selected_device);
		keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeybindProc, NULL, NULL);
		if (keyboard_hook == NULL) {
			std::cout << "Failed to hook!\n";
			exit(-2);
		}
	}
	MSG message;
	while (GetMessage(&message, NULL, 0, 0) != 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	UnhookWindowsHookEx(keyboard_hook);
	
	system("pause");
}