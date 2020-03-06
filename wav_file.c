#include "wav_file.h"




uint8_t bytes_to_uint8_le(unsigned char* bytes) {
	return (bytes[1] << 8) | bytes[0];
}

uint32_t bytes_to_uint32_le(unsigned char* bytes) {
	return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

void read_empty(FILE* file, size_t size) {
	char* empty = (char*)malloc(size);
	memset(empty, 0, size);
	fread(empty, 1, size, file);
	free(empty);
}

struct wav_file read_file(char* path) {
	struct wav_file s;
	FILE* file = fopen(path, "rb");

	read_empty(file, 4);

	unsigned char* _chunk_size = (unsigned char*)malloc(4);
	memset(_chunk_size, 0, 4);
	fread(_chunk_size, 1, 4, file);
	uint32_t chunk_size = bytes_to_uint32_le(_chunk_size);
	free(_chunk_size);
	s.chunk_size = chunk_size;

	char* audio_format = (char*)malloc(5);
	memset(audio_format, 0, 5);
	fread(audio_format, 1, 4, file);
	s.audio_format = audio_format;

	read_empty(file, 4);

	unsigned char* _fmt_subchunk1_size = (unsigned char*)malloc(4);
	memset(_fmt_subchunk1_size, 0, 4);
	fread(_fmt_subchunk1_size, 1, 4, file);

	uint32_t fmt_subchunk1_size = bytes_to_uint32_le(_fmt_subchunk1_size);
	free(_fmt_subchunk1_size);
	s.fmt_subchunk1_size = fmt_subchunk1_size;

	unsigned char* _fmt_audioformat = (unsigned char*)malloc(2);
	memset(_fmt_audioformat, 0, 2);
	fread(_fmt_audioformat, 1, 2, file);

	uint8_t fmt_audioformat = bytes_to_uint8_le(_fmt_audioformat);
	free(_fmt_audioformat);
	s.fmt_audioformat = fmt_audioformat;

	unsigned char* _fmt_numaudiochannels = (unsigned char*)malloc(2);
	memset(_fmt_numaudiochannels, 0, 2);
	fread(_fmt_numaudiochannels, 1, 2, file);

	uint8_t fmt_numaudiochannels = bytes_to_uint8_le(_fmt_numaudiochannels);
	free(_fmt_numaudiochannels);
	s.fmt_numaudiochannels = fmt_numaudiochannels;

	unsigned char* _fmt_samplerate = (unsigned char*)malloc(4);
	memset(_fmt_samplerate, 0, 4);
	fread(_fmt_samplerate, 1, 4, file);

	uint32_t fmt_samplerate = bytes_to_uint32_le(_fmt_samplerate);
	free(_fmt_samplerate);
	s.fmt_samplerate = fmt_samplerate;

	unsigned char* _fmt_byterate = (unsigned char*)malloc(4);
	memset(_fmt_byterate, 0, 4);
	fread(_fmt_byterate, 1, 4, file);

	uint32_t fmt_byterate = bytes_to_uint32_le(_fmt_byterate);
	free(_fmt_byterate);
	s.fmt_byterate = fmt_byterate;

	unsigned char* _fmt_blockalign = (unsigned char*)malloc(2);
	memset(_fmt_blockalign, 0, 2);
	fread(_fmt_blockalign, 1, 2, file);

	uint8_t fmt_blockalign = bytes_to_uint8_le(_fmt_blockalign);
	free(_fmt_blockalign);
	s.fmt_blockalign = fmt_blockalign;

	unsigned char* _fmt_bitspersample = (unsigned char*)malloc(2);
	memset(_fmt_bitspersample, 0, 2);
	fread(_fmt_bitspersample, 1, 2, file);

	uint8_t fmt_bitspersample = bytes_to_uint8_le(_fmt_bitspersample);
	free(_fmt_bitspersample);
	s.fmt_bitspersample = fmt_bitspersample;

	char* data_chunk = (char*)malloc(5);
	memset(data_chunk, 0, 5);
	fread(data_chunk, 1, 4, file);
	free(data_chunk);

	unsigned char* _data_subchunk2size = (unsigned char*)malloc(4);
	memset(_data_subchunk2size, 0, 4);
	fread(_data_subchunk2size, 1, 4, file);

	uint32_t data_subchunk2size = bytes_to_uint32_le(_data_subchunk2size);
	free(_data_subchunk2size);
	s.data_subchunk2size = data_subchunk2size;
	s.file = file;
	return s;
}


void read_samples(struct wav_file file, uint32_t size, unsigned char* buffer, int offset) {
	for (int i = 0; i < size; i ++) {

		unsigned char* _f_sample = (unsigned char*)malloc(2);
		memset(_f_sample, 0, 2);
		fread(_f_sample, 1, 2, file.file);
		uint8_t f_sample = bytes_to_uint8_le(_f_sample);
		free(_f_sample);

		unsigned char* _s_sample = (unsigned char*)malloc(2);
		memset(_s_sample, 0, 2);
		fread(_s_sample, 1, 2, file.file);
		uint8_t s_sample = bytes_to_uint8_le(_s_sample);
		free(_s_sample);
		
		*(buffer++) = f_sample;
		*buffer = s_sample;
		//buffer += file.fmt_blockalign;
	}
}