#pragma once

#include "ALCheck.hpp"
#include "wav.hpp"
#include <AL/al.h>
#include <string>
#include <string_view>
#include <array>

struct ImmediateWavEntry {
	ImmediateWavEntry() {
		// prevent needing to constantly remake the source
		alcheck(alGenSources(1, &source));
	};
	ALuint source = 0;
	ALuint buffer = 0;
};

// I'm keeping the context as something you pass in, because I want framework to be separate from globals
void play_wav_immediate(std::string_view filepath, float pitch, float gain, bool is_looping, ALCcontext* context) {
	// to prevent resource leaks
	static std::array<ImmediateWavEntry, 16> cache;
	// rolling buffer index
	static uint16_t current = 0;
	static size_t audio_device_tracker = 0;

	if (audio_device_tracker != DSKGlobals::audioStateChangeCount) {
		// Audio device has changed! repopulate context.
		for (auto& e : cache) {
			alcheck(alGenSources(1, &e.source));
		}
	}

	if (cache[current].buffer != 0) {
		alSourceStop(cache[current].source);
		alcheck(alSourcei(cache[current].source, AL_BUFFER, 0));
		alcheck(alDeleteBuffers(1, &cache[current].buffer));
		create_AL_buffer(cache[current].buffer, filepath.data());
		alcheck(alSource3f(cache[current].source, AL_POSITION, 0.f, 0.f, 0.f));
		alcheck(alSource3f(cache[current].source, AL_VELOCITY, 0.f, 0.f, 0.f));
		alcheck(alSourcef(cache[current].source, AL_PITCH, pitch));
		alcheck(alSourcef(cache[current].source, AL_GAIN, gain));
		alcheck(alSourcei(cache[current].source, AL_LOOPING, is_looping));
		alcheck(alSourcei(cache[current].source, AL_BUFFER, cache[current].buffer));
	}
	else {
		alSourceStop(cache[current].source);
		alcheck(alSourcei(cache[current].source, AL_BUFFER, 0));
		create_AL_buffer(cache[current].buffer, filepath.data());
		alcheck(alSource3f(cache[current].source, AL_POSITION, 0.f, 0.f, 0.f));
		alcheck(alSource3f(cache[current].source, AL_VELOCITY, 0.f, 0.f, 0.f));
		alcheck(alSourcef(cache[current].source, AL_PITCH, pitch));
		alcheck(alSourcef(cache[current].source, AL_GAIN, gain));
		alcheck(alSourcei(cache[current].source, AL_LOOPING, is_looping));
		alcheck(alSourcei(cache[current].source, AL_BUFFER, cache[current].buffer));
	}

	alcheck(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));
	alcheck(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));

	ALfloat basis[] = {
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f
	};

	alcheck(alListenerfv(AL_ORIENTATION, basis));
	alcheck(alSourcePlay(cache[current].source));
	current = (current + 1) % cache.size();
}