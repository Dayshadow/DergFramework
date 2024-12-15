#pragma once
#include <string_view>
#include <AudioFile.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <Framework/Audio/ALCheck.hpp>
#include <Framework/Log.hpp>

std::vector<uint8_t> load_wav_pcm(const std::string& filename) {
	AudioFile<float> file;
	if (!file.load(filename)) {
		std::cerr << "Audio file " << filename << " could not be loaded.\n";
		return {};
	}
	file.printSummary();

	std::vector<uint8_t> file_raw;
	file.writePCMToBuffer(file_raw);

	return file_raw;
}

ALenum toOpenALFormat(const AudioFile<float>& af) {
	int bitDepth = af.getBitDepth();
	bool isStereo = af.isStereo();
	switch (bitDepth) {
	case 8:
		if (isStereo) return AL_FORMAT_STEREO8;
		return AL_FORMAT_MONO8;
	case 16:
		if (isStereo) return AL_FORMAT_STEREO16;
		return AL_FORMAT_MONO16;
	default:
		std::cerr << "Bad bit depth!\n";
		return -1;
	}
}

void create_AL_buffer(ALuint& buffer, const std::string& filename) {
	AudioFile<float> file;
	if (!file.load(filename)) {
		std::cerr << "Audio file " << filename << " could not be loaded.\n";
	}
	file.printSummary();

	std::vector<uint8_t> file_raw;
	file.writePCMToBuffer(file_raw);

	alcheck(alGenBuffers(1, &buffer));
	alcheck(alBufferData(buffer, toOpenALFormat(file), file_raw.data(), file_raw.size(), file.getSampleRate()));
	LOG("Create Buffer " << buffer);
}