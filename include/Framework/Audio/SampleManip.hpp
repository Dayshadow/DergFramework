#pragma once
#include <vector>

struct AudioSample {
	int16_t left = 0;
	int16_t right = 0;
};

std::vector<AudioSample> PCMStereo16ToSamples(const std::vector<uint8_t>& buffer) {
	std::vector<AudioSample> result;
	for (size_t i = 0; i < buffer.size(); i += 4) {
		AudioSample out;
		out.left = (static_cast<int16_t>(buffer[i + 0]) << 0) | (static_cast<int16_t>(buffer[i + 1]) << 8);
		out.right = (static_cast<int16_t>(buffer[i + 2]) << 0) | (static_cast<int16_t>(buffer[i + 3]) << 8);
		result.emplace_back(out);
	}
	return result;
}

std::vector<uint8_t> samplesToPCMStereo16(const std::vector<AudioSample>& buffer) {
	std::vector<uint8_t> result;
	for (AudioSample sample : buffer) {
		result.push_back((sample.left & 0x00FF) >> 0);
		result.push_back((sample.left & 0xFF00) >> 8);
		result.push_back((sample.right & 0x00FF) >> 0);
		result.push_back((sample.right & 0xFF00) >> 8);
	}
	return result;
}