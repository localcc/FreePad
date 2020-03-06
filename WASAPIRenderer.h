#ifndef WASAPIRenderer_H_
#define WASAPIRenderer_H_
#include <iostream>
#include <cstdio>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <fstream>
#include <thread>
#include <comdef.h>
#include <Functiondiscoverykeys_devpkey.h>

#define PRINT_FAILED(hr) if(FAILED(hr)) std::wcout << _com_error(hr).ErrorMessage()


class WASAPIRenderer {
public:
	WASAPIRenderer();
	void select_device(UINT index);
	void enumerate_devices();
	void show_enumerated_devices();
	void play_file(const char* filename);
	static WASAPIRenderer* get_instance() { return loaded_instance; }
private:
	IMMDeviceEnumerator* endpointEnumerator;
	IMMDeviceCollection* endpointCollection;
	IMMDevice* endpoint;
	UINT deviceAmount;
	static WASAPIRenderer* loaded_instance;
};

#endif // WASAPIRenderer_H_