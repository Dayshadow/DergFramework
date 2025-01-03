#pragma once

#include "ALCheck.hpp"
#include "wav.hpp"
#include <AL/al.h>
#include <string>
#include <string_view>
#include <array>

struct ImmediateWavEntry {
	ImmediateWavEntry();
	ALuint source = 0;
	ALuint buffer = 0;
};

// I'm keeping the context as something you pass in, because I want framework to be separate from globals
// don't worry about stateChangeCount if you don't know what it is, its for making sure that it uses the right audio device.
void play_wav_immediate(std::string_view filepath, float pitch, float gain, bool is_looping, ALCcontext* context, size_t stateChangeCount = 0);