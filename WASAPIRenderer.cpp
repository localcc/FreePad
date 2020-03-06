#include "WASAPIRenderer.h"
#include "wav_file.h"

#define REFTIMES_PER_SEC  10000000.0 // 10,000,000
#define REFTIMES_PER_MILLISEC  10000 // 10,000

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

WASAPIRenderer* WASAPIRenderer::loaded_instance = NULL;

WASAPIRenderer::WASAPIRenderer():
	endpointEnumerator(NULL), endpointCollection(NULL), 
	endpoint(NULL), deviceAmount(0) {
	HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&endpointEnumerator);
	PRINT_FAILED(hr);
	loaded_instance = this;
}

void WASAPIRenderer::enumerate_devices() {
	HRESULT hr = endpointEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &endpointCollection);
	PRINT_FAILED(hr);
	deviceAmount = 0;
	hr = endpointCollection->GetCount(&deviceAmount);
	PRINT_FAILED(hr);
	printf("Devices amount: %d\n", deviceAmount);
}

void WASAPIRenderer::select_device(UINT index) {
	if (deviceAmount <= index) {
		printf("Wrong index! Check your config file!\n");
		return;
	}
	HRESULT hr = endpointCollection->Item(index, &endpoint);
	PRINT_FAILED(hr);
}

void WASAPIRenderer::show_enumerated_devices() {
	HRESULT hr = S_OK;
	for (UINT i = 0; i < deviceAmount; i++) {
		IMMDevice* device;
		hr = endpointCollection->Item(i, &device);
		PRINT_FAILED(hr);
		IPropertyStore* propertyStore;
		hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		PRINT_FAILED(hr);
		PROPVARIANT variant;
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &variant);
		PRINT_FAILED(hr);
		std::wcout << i << ": " << (const wchar_t*)(variant.bstrVal) << "\n";
		propertyStore->Release();
		device->Release();
	}
	std::cout << "\n";
}

void WASAPIRenderer::play_file(const char* filename) {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	IAudioClient* audioClient;
	HRESULT hr = endpoint->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audioClient);
	PRINT_FAILED(hr);

	wav_file* current_file = new wav_file();
	current_file->read_file(filename);
	WAVEFORMATEXTENSIBLE waveformatextensible = current_file->fill_formatex();
	WAVEFORMATEX* waveformat = &waveformatextensible.Format;
	REFERENCE_TIME device_period;
	
	audioClient->GetDevicePeriod(&device_period, NULL);
	PRINT_FAILED(hr);
	hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, device_period, 0, waveformat, NULL);
	PRINT_FAILED(hr);

	UINT32 bufferSize;
	hr = audioClient->GetBufferSize(&bufferSize);
	PRINT_FAILED(hr);

	IAudioRenderClient* renderClient;
	hr = audioClient->GetService(IID_IAudioRenderClient, (void**)&renderClient);
	PRINT_FAILED(hr);

	BYTE* buffer;
	hr = renderClient->GetBuffer(bufferSize, &buffer);
	PRINT_FAILED(hr);
	hr = renderClient->ReleaseBuffer(bufferSize, 0);
	PRINT_FAILED(hr);

	hr = audioClient->Start();
	PRINT_FAILED(hr);

	UINT32 read = 0;
	UINT32 framesAvailable;
	UINT32 padding;
	UINT32 bytesAvailable;
	UINT32 framesInFile = current_file->WaveLength / waveformat->nBlockAlign;


	for (int framesPlayed = 0; framesPlayed < framesInFile; framesPlayed += framesAvailable) {
		Sleep((DWORD)(device_period / REFTIMES_PER_MILLISEC / 2));
		hr = audioClient->GetCurrentPadding(&padding);
		framesAvailable = bufferSize - padding;
		PRINT_FAILED(hr);

		if (framesAvailable > (framesInFile - framesPlayed)) {
			framesAvailable = framesInFile - framesPlayed;
		}
		hr = renderClient->GetBuffer(framesAvailable, &buffer);
		PRINT_FAILED(hr);
		bytesAvailable = framesAvailable * waveformat->nBlockAlign;
		memcpy(buffer, &(current_file->get_wave_buf()[current_file->WavePtr]), bytesAvailable);
		current_file->WavePtr += bytesAvailable;
		hr = renderClient->ReleaseBuffer(framesAvailable, 0);
		PRINT_FAILED(hr);
	}
	audioClient->Stop();
	renderClient->Release();
	audioClient->Release();
	delete current_file;
	CoUninitialize();
}


