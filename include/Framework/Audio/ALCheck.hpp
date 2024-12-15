#pragma once
#include <AL/al.h>
#include <AL/alc.h>

#define OpenAL_ErrorCheck(message)\
{\
    ALenum err = alGetError();\
    if (err != AL_NO_ERROR) {\
        std::cerr << "OpenAL Error: " << err << " when calling " << #message << "\n";\
    }\
}

#define alcheck(CALL)\
CALL;\
OpenAL_ErrorCheck(CALL)