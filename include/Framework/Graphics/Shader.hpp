#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <util/ext/glm/glm.hpp>
#include <vector>
#include <string>
#include <string_view>
#include "Framework/Log.hpp"
#include "Framework/Graphics/Uniform.hpp"
#include "GlIDs.hpp"

/// A wrapper for an OpenGL shader program, allows simple compilation and uniform setting.
class Shader
{
public:
	/// Default void constructor. If no parameters are supplied, the program ID is set to 0, which might have adverse effects.
	Shader() {};
	/** Constructor that initializes by compiling the shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* fs_filePath);
	/** Constructor that initializes by compiling the shader, geometry shader included.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);

	/** Constructor that initializes by compiling the shader, geometry shader included, and transform feedback enabled.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	*/
	Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, const char* p_captureVariables[], GLsizei p_captureVariableCount);
	Shader& operator=(const Shader& p_other);

	// Move constructor, to handle our pointer
	Shader(Shader&& other) noexcept;
	/** Compiles and sets the Shader's program ID.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* fs_filePath);
	/** Compiles and sets the Shader's program ID. Overload for incorporating geometry shader.
	* @param vs_filePath - The filepath to the GLSL vertex shader. In standard directory notation, folders separated by "/"
	* @param gs_filePath - The filepath to the GLSL geometry shader. In standard directory notation, folders separated by "/"
	* @param fs_filePath - The filepath the the GLSL fragment shader. In standard directory notation, folders separated by "/"
	* @returns The GLuint program ID given after compilation
	*/
	GLuint compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath);

	/// Bind the shader for use with subsequent OpenGL draw calls.
	void use() const;
	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a boolean.
	GLint addBoolUniform(std::string_view p_name, bool p_value);
	void setBoolUniform(GLint p_loc, bool p_value) const;
	static void setBoolUniformStatic(GLint p_loc, bool p_value);

	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an integer.
	GLint addIntUniform(std::string_view p_name, GLint p_value);
	void setIntUniform(GLint p_loc, GLint p_value) const;
	static void setIntUniformStatic(GLint p_loc, GLint p_value);

	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a float.
	GLint addFloatUniform(std::string_view p_name, GLfloat p_value);
	void setFloatUniform(GLint p_loc, GLfloat p_value) const;
	static void setFloatUniformStatic(GLint p_loc, GLfloat p_value);

	/// A function that handles assigning a value to a uniform within the shader's program. In this case, an OpenGL texture.
	GLint addTexUniform(std::string_view p_name, GLuint p_value);
	void setTexUniform(GLint p_loc, GLuint p_value);
	static void setTexUniformStatic(GLint p_loc, GLuint p_value);

	/// A function that handles assigning a value to a uniform within the shader's program. In this case, a 4x4 matrix.
	GLint addMat4Uniform(std::string_view p_name, glm::mat4& p_value);
	void setMat4Uniform(GLint p_loc, glm::mat4& p_value);
	static void setMat4UniformStaticNamed(std::string_view p_name, glm::mat4& p_value, GLuint p_progID);
	static void setMat4UniformStatic(GLint p_loc, glm::mat4& p_value);

	GLint addVec2Uniform(std::string_view p_name, glm::vec2 p_value);
	void setVec2Uniform(GLint p_loc, glm::vec2 p_value) const;
	static void setVec2UniformStatic(GLint p_loc, glm::vec2 p_value);


	GLint addVec3Uniform(std::string_view p_name, glm::vec3 p_value);
	void setVec3Uniform(GLint p_loc, glm::vec3 p_value) const;
	static void setVec3UniformStatic(GLint p_loc, glm::vec3 p_value);

	GLint addVec4Uniform(std::string_view p_name, glm::vec4 p_value);
	void setVec4Uniform(GLint p_loc, glm::vec4 p_value) const;
	static void setVec4UniformStatic(GLint p_loc, glm::vec4 p_value);

	// Used to set a list of uniforms using uniform objects.
	// Only works with strings, no locations.
	//void setUniforms(std::vector<Uniform> p_uniforms);

	GLuint getUniformLoc(std::string_view uniformName);
	GLuint getUniformBlockIndex(std::string_view uniformBlockName);
	std::shared_ptr<glProgram> program = std::make_shared<glProgram>();
private:
	std::vector<Uniform> m_uniforms;
};

#endif