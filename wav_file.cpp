#include "wav_file.h"

wav_file::wav_file() {

}

wav_file::~wav_file() {
	delete[] wave_buf;
}

void wav_file::read_file(const char* filename) {
	FILE* file = fopen(filename, "rb");
	long file_size = 0;
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);
	wave_buf = new BYTE[file_size];
	fread(wave_buf, 1, file_size, file);
	fclose(file);
}

WAVEFORMATEXTENSIBLE wav_file::fill_formatex(){ 
	WAVEFORMATEXTENSIBLE formatex = {};
	if (*(DWORD*)&this->wave_buf[8] == 0x45564157) {// "WAVE"
		printf("Wave\n");
		if ((*(WORD*)&this->wave_buf[20] == 1) || (*(WORD*)&this->wave_buf[20] == 3) || (*(WORD*)&this->wave_buf[20] == 0xFFFE)) {// PCM or WAVE_FORMAT_IEEE_FLOAT
			printf("Pcm\n");
			DWORD subchunksize = *(DWORD*)&this->wave_buf[16];
			DWORD nextchunk = subchunksize + 20;
			if (*(DWORD*)&this->wave_buf[nextchunk] == 0x74636166)// "fact"
				nextchunk += 12; // ignore fact chunk
			if (*(DWORD*)&this->wave_buf[nextchunk] == 0x61746164) {// "data"
				printf("REading\n");
				WaveLength = *(DWORD*)&this->wave_buf[nextchunk + 4];
				WavePtr = nextchunk + 8;
				WaveEnd = WavePtr + WaveLength;
				formatex.Format.wFormatTag = *(WORD*)&this->wave_buf[20];
				formatex.Format.nChannels = *(WORD*)&this->wave_buf[22];
				formatex.Format.nSamplesPerSec = *(DWORD*)&this->wave_buf[24];
				formatex.Format.nAvgBytesPerSec = *(DWORD*)&this->wave_buf[28];
				formatex.Format.nBlockAlign = *(WORD*)&this->wave_buf[32];
				formatex.Format.wBitsPerSample = *(WORD*)&this->wave_buf[34];
				if ((formatex.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE))
					formatex.Format.cbSize = 0;
				else {
					formatex.Format.cbSize = 22;
					formatex.Samples.wValidBitsPerSample = *(WORD*)&this->wave_buf[38];
					formatex.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
					formatex.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
				}
			}
		}
	}
	return formatex;
}
