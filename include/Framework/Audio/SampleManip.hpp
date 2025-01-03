#pragma once
#include <vector>

struct AudioSample {
	int16_t left = 0;
	int16_t right = 0;
};

std::vector<AudioSample> PCMStereo16ToSamples(const std::vector<uint8_t>& buffer);

std::vector<uint8_t> samplesToPCMStereo16(const std::vector<AudioSample>& buffer);