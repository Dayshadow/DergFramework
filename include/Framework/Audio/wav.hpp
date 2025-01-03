#pragma once
#include <string_view>
#include <AudioFile.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <Framework/Audio/ALCheck.hpp>
#include <Framework/Log.hpp>

std::vector<uint8_t> load_wav_pcm(const std::string& filename);

ALenum toOpenALFormat(const AudioFile<float>& af);

void create_AL_buffer(ALuint& buffer, const std::string& filename);