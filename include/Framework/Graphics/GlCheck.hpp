#ifndef GLCHECK_H
#define GLCHECK_H
#include "Framework/FrameworkConstants.hpp"
#include "GL/glew.h"
#include <filesystem>
#include <iostream>

// ADAPTED FROM SFML

#ifdef SBBB_DEBUG

#define glCheck(expr)                                      \
    do                                                     \
    {                                                      \
        expr;                                              \
        glCheckError(__FILE__, __LINE__, #expr); \
    } while (false)

#else

#define glCheck(expr) (expr)

#endif


inline void glCheckError(const std::filesystem::path& file, uint32_t line, const char* expression) {
	// Get the last error
	GLenum errorCode = glGetError();

	if (errorCode != GL_NO_ERROR)
	{
		std::string error = "Unknown error";
		std::string description = "No description";

		switch (errorCode)
		{
		case GL_INVALID_ENUM:
		{
			error = "GL_INVALID_ENUM";
			description = "An unacceptable value has been specified for an enumerated argument.";
			break;
		}

		case GL_INVALID_VALUE:
		{
			error = "GL_INVALID_VALUE";
			description = "A numeric argument is out of range.";
			break;
		}

		case GL_INVALID_OPERATION:
		{
			error = "GL_INVALID_OPERATION";
			description = "The specified operation is not allowed in the current state.";
			break;
		}

		case GL_STACK_OVERFLOW:
		{
			error = "GL_STACK_OVERFLOW";
			description = "This command would cause a stack overflow.";
			break;
		}

		case GL_STACK_UNDERFLOW:
		{
			error = "GL_STACK_UNDERFLOW";
			description = "This command would cause a stack underflow.";
			break;
		}

		case GL_OUT_OF_MEMORY:
		{
			error = "GL_OUT_OF_MEMORY";
			description = "There is not enough memory left to execute the command.";
			break;
		}

		case GL_INVALID_FRAMEBUFFER_OPERATION:
		{
			error = "GL_INVALID_FRAMEBUFFER_OPERATION";
			description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
			break;
		}
		}

		std::cout << "An internal OpenGL call failed in " << file.filename() << "(" << line << ")."
			<< "\nExpression:\n   " << expression << "\nError description:\n   " << error << "\n   " << description << '\n'
			<< std::endl;
	}

}

#endif