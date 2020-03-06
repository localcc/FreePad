#ifndef WAV_FILE_H_
#define WAV_FILE_H_
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <Windows.h>
#include <Audioclient.h>
class wav_file {
public:
	wav_file();
	void read_file(const char* filename);
	WAVEFORMATEXTENSIBLE fill_formatex();
	BYTE* get_wave_buf() { return this->wave_buf;  }
	DWORD WaveLength, WavePtr, WaveEnd;
	~wav_file();
private:
	BYTE* wave_buf;
	HANDLE file;
};
#endif // WAV_FILE_H_