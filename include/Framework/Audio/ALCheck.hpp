#pragma once
#include <AL/al.h>
#include <AL/alc.h>

#define OpenAL_ErrorCheck(message)\
{\
    ALenum err = alGetError();\
    if (err != AL_NO_ERROR) {\
        std::cerr << "OpenAL Error: " << err << " when calling " << #message << "\n";\
        assert(false);\
    }\
}

#define alcheck(CALL)\
CALL;\
OpenAL_ErrorCheck(CALL)

#define OpenALC_ErrorCheck(message, device)\
{\
    ALenum err = alcGetError(device);\
    if (err != AL_NO_ERROR) {\
        std::cerr << "OpenALC Error: " << err << " when calling " << #message << "\n";\
        assert(false);\
    }\
}

#define alc_check(CALL, device)\
CALL;\
OpenALC_ErrorCheck(CALL, static_cast<ALCdevice*>(device))