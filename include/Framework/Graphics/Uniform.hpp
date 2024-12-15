#pragma once
#include <string>
#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>

// shrimple dimple
// used for initilizing uniforms in the constructor of shaders
// kinda depreciated but don't want to get rid of it
enum class UniformTypes {
	UNDEFINED,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	TEX,
	MAT4,
	INT,
	BOOL
};
struct Uniform {
	// separated constructors for the different types
	// the wall of text
	Uniform(std::string_view p_name, UniformTypes p_type, GLint p_value) : name(p_name), type(p_type), intValue(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, GLuint p_value) : name(p_name), type(p_type), uintValue(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, glm::vec2 p_value) : name(p_name), type(p_type), vec2Value(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, glm::vec3 p_value) : name(p_name), type(p_type), vec3Value(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, glm::vec4 p_value) : name(p_name), type(p_type), vec4Value(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, GLfloat p_value) : name(p_name), type(p_type), floatValue(p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, bool p_value) : name(p_name), type(p_type), intValue((int)p_value) {};
	Uniform(std::string_view p_name, UniformTypes p_type, glm::mat4 p_value) : name(p_name), type(p_type), mat4Value(p_value) {};

	// god this is scuffed but it's the best way I can think of doing it because this class needs to be a fixed type because I need to be able to put multiple of different value types in the same array
	// the int value is shared by tex, bool, and int types because they are indistinguishable
	// Might make this a union later, but mat4 is substantially bigger so idk
	union {
		GLint intValue;
		GLuint uintValue;
		GLfloat floatValue;
		glm::vec2 vec2Value;
		glm::vec3 vec3Value;
		glm::vec4 vec4Value;
		glm::mat4 mat4Value = glm::mat4(0.f);
	};

	const std::string name;
	GLint loc = 0;
	// Type determines which internal type the shader should fetch. 
	UniformTypes type = UniformTypes::UNDEFINED;
};
