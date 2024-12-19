#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include "util/utils.hpp"
#include <util/ext/glm/gtc/type_ptr.hpp>
#include <exception>

Shader::Shader(const char* vs_filePath, const char* fs_filePath)
{
	program->ID = Shader::compileShaders(vs_filePath, fs_filePath);
	GLGEN_LOG("Created Shader Program " << program->ID);
}

Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	program->ID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);
	GLGEN_LOG("Created Shader Program " << program->ID);
}

Shader::Shader(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath, const char* p_captureVariables[], GLsizei p_captureVariableCount)
{
	program->ID = Shader::compileShaders(vs_filePath, gs_filePath, fs_filePath);
	glTransformFeedbackVaryings(program->ID, p_captureVariableCount, p_captureVariables, GL_INTERLEAVED_ATTRIBS);
	glCheck(glLinkProgram(program->ID));

	GLint linkStatus;
	glGetProgramiv(program->ID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE) { // check for failed transform feedback link
		GLchar infoLog[512];
		glGetProgramInfoLog(program->ID, 512, nullptr, infoLog);
		ERROR_LOG(infoLog);
	}
	GLGEN_LOG("Created Shader Program " << program->ID);
}

Shader& Shader::operator=(const Shader& p_other) {
	program = p_other.program;
	return *this;
}

Shader::Shader(Shader&& other) noexcept {
	LOG("Moved Shader! Program ID: " << other.program->ID);
	other.program = std::move(program);
}
GLuint Shader::compileShaders(const char* vs_filePath, const char* fs_filePath) {
	GLuint vs, fs;

	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	LOAD_LOG("Compiling shaders: Vertex - " << vs_filePath << " Fragment - " << fs_filePath);

	std::string vertexShader = utils::readFile(vs_filePath);
	GLint vsLength = (GLint)vertexShader.size();
	const char* vertexShader_cstr = vertexShader.c_str();
	glCheck(glShaderSource(vs, 1, (const GLchar**)&vertexShader_cstr, &vsLength));
	glCheck(glCompileShader(vs));

	GLint status;

	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
		ERROR_LOG("Vertex shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glCheck(glDeleteShader(vs)); // Don't leak the shader.

		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	GLint fsLength = (GLint)fragmentShader.length();
	const char* fragmentShader_cstr = fragmentShader.c_str();
	glCheck(glShaderSource(fs, 1, (const GLchar**)&fragmentShader_cstr, &fsLength));
	glCheck(glCompileShader(fs));

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

		ERROR_LOG("Fragment shader compilation error" << std::endl);
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glCheck(glDeleteShader(fs)); // Don't leak the shader.
		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	LOAD_LOG("Shader compilation successful. Attaching to program...");

	program->ID = glCreateProgram();
	glCheck(glAttachShader(program->ID, vs));
	glCheck(glAttachShader(program->ID, fs));

	glCheck(glDeleteShader(vs));
	glCheck(glDeleteShader(fs));

	glCheck(glLinkProgram(program->ID));
	glGetProgramiv(program->ID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program->ID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program->ID, maxLength, &maxLength, &errorLog[0]);

		ERROR_LOG("Shader linking error: ");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		// Exit with failure.
		glDeleteProgram(program->ID); // Don't leak the program.
		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}
	glCheck(glUseProgram(program->ID));

	return program->ID;
}

GLuint Shader::compileShaders(const char* vs_filePath, const char* gs_filePath, const char* fs_filePath)
{
	GLuint vs, gs, fs;

	// Throwaway
	vs = glCreateShader(GL_VERTEX_SHADER);
	gs = glCreateShader(GL_GEOMETRY_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	LOAD_LOG("Compiling shaders: Vertex - " << vs_filePath << " Geometry - " << gs_filePath << " Fragment - " << fs_filePath);

	std::string vertexShader = utils::readFile(vs_filePath);
	GLint vsLength = (GLint)vertexShader.size();
	const char* vertexShader_cstr = vertexShader.c_str();
	glShaderSource(vs, 1, (const GLchar**)&vertexShader_cstr, &vsLength);
	glCompileShader(vs);

	GLint status;

	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
		ERROR_LOG("Vertex shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(vs); // Don't leak the shader.

		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string geometryShader = utils::readFile(gs_filePath);
	GLint gsLength = (GLint)geometryShader.size();
	const char* geometryShader_cstr = geometryShader.c_str();
	glShaderSource(gs, 1, (const GLchar**)&geometryShader_cstr, &gsLength);
	glCompileShader(gs);


	glGetShaderiv(gs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(gs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(gs, maxLength, &maxLength, &errorLog[0]);
		ERROR_LOG("Geometry shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(gs); // Don't leak the shader.

		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	std::string fragmentShader = utils::readFile(fs_filePath);
	GLint fsLength = (GLint)fragmentShader.length();
	const char* fragmentShader_cstr = fragmentShader.c_str();
	glShaderSource(fs, 1, (const GLchar**)&fragmentShader_cstr, &fsLength);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);

		ERROR_LOG("Fragment shader compilation error");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}
		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(fs); // Don't leak the shader.
		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	LOAD_LOG("Shader compilation successful. Attaching to program...");

	program->ID = glCreateProgram();
	glAttachShader(program->ID, vs);
	glAttachShader(program->ID, gs);
	glAttachShader(program->ID, fs);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	glLinkProgram(program->ID);
	glGetProgramiv(program->ID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program->ID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program->ID, maxLength, &maxLength, &errorLog[0]);

		ERROR_LOG("Shader linking error: ");
		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		// Exit with failure.
		glDeleteProgram(program->ID); // Don't leak the program.
		throw std::runtime_error("Shader was unable to compile.");
		return -1;
	}

	glUseProgram(program->ID);

	return program->ID;
}

void Shader::use() const { 
	glCheck(glUseProgram(program->ID));
}

//void Shader::setUniforms(std::vector<Uniform> p_uniforms) {
//	for (size_t i = 0; i < p_uniforms.size(); i++) {
//		switch (p_uniforms[i].type) {
//		case UniformTypes::TEX:
//			Shader::setTexUniform(p_uniforms[i].name, p_uniforms[i].intValue);
//			break;
//		case UniformTypes::INT:
//			Shader::setIntUniform(p_uniforms[i].name, p_uniforms[i].intValue);
//			break;
//		case UniformTypes::BOOL:
//			Shader::setBoolUniform(p_uniforms[i].name, (bool)p_uniforms[i].intValue);
//			break;
//		case UniformTypes::FLOAT:
//			Shader::setFloatUniform(p_uniforms[i].name, p_uniforms[i].floatValue);
//			break;
//		case UniformTypes::MAT4:
//			Shader::setMat4Uniform(p_uniforms[i].name, p_uniforms[i].mat4Value);
//			break;
//		default:
//			throw std::runtime_error("Attempted to set uniform with invalid type.");
//		}
//	}
//}
GLint Shader::addBoolUniform(std::string_view p_name, bool p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
#ifdef LOADLOGGING_ENABLEDD
	if (loc != -1) {
		LOAD_LOG("addBoolUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		ERROR_LOG("addBoolUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	Uniform u = Uniform{ p_name, UniformTypes::BOOL, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform1i(loc, (int)p_value));
	return loc;
}
void Shader::setBoolUniform(GLint p_loc, bool p_value) const
{
	use();
	glCheck(glUniform1i(p_loc, (int)p_value));
}
void Shader::setBoolUniformStatic(GLint p_loc, bool p_value)
{
	glCheck(glUniform1i(p_loc, (int)p_value));
}
GLint Shader::addIntUniform(std::string_view p_name, GLint p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::INT, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform1i(loc, p_value));
	return loc;
}
void Shader::setIntUniform(GLint p_loc, GLint p_value) const
{
	use();
	glCheck(glUniform1i(p_loc, p_value));
}
void Shader::setIntUniformStatic(GLint p_loc, GLint p_value)
{
	glCheck(glUniform1i(p_loc, p_value));
}
GLint Shader::addTexUniform(std::string_view p_name, GLuint p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::TEX, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform1i(loc, p_value));
	return loc;
}

void Shader::setTexUniform(GLint p_loc, GLuint p_value)
{
	use();
	glCheck(glUniform1i(p_loc, p_value));
}

void Shader::setTexUniformStatic(GLint p_loc, GLuint p_value)
{
	glCheck(glUniform1i(p_loc, p_value));
}

GLint Shader::addFloatUniform(std::string_view p_name, GLfloat p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
#ifdef LOADLOGGING_ENABLEDD
	if (loc != -1) {
		LOAD_LOG("addFloatUniform: glGetUniformLocation returned \"" << loc << "\" for " << p_name << ".");
	}
	else {
		ERROR_LOG("addFloatUniform: Uniform location not found for " << p_name << ".");
	}
#endif
	Uniform u = Uniform{ p_name, UniformTypes::FLOAT, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform1f(loc, p_value));
	return loc;
}
void Shader::setFloatUniform(GLint p_loc, GLfloat p_value) const
{
	use();
	glCheck(glUniform1f(p_loc, p_value));
}
void Shader::setFloatUniformStatic(GLint p_loc, GLfloat p_value)
{
	glCheck(glUniform1f(p_loc, p_value));
}
GLint Shader::addMat4Uniform(std::string_view p_name, glm::mat4& p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::MAT4, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p_value)));
	return loc;
	}

void Shader::setMat4Uniform(GLint p_loc, glm::mat4& p_value)
{
	use();
	glCheck(glUniformMatrix4fv(p_loc, 1, GL_FALSE, glm::value_ptr(p_value)));
}

void Shader::setMat4UniformStaticNamed(std::string_view p_name, glm::mat4& p_value, GLuint p_progID)
{
	GLint loc = glGetUniformLocation(p_progID, p_name.data());
	glCheck(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p_value)));
}

void Shader::setMat4UniformStatic(GLint p_loc, glm::mat4& p_value)
{
	glCheck(glUniformMatrix4fv(p_loc, 1, GL_FALSE, glm::value_ptr(p_value)));
}

GLint Shader::addVec2Uniform(std::string_view p_name, glm::vec2 p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::VEC2, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform2f(loc, p_value.x, p_value.y));
	return loc;
}

void Shader::setVec2Uniform(GLint p_loc, glm::vec2 p_value) const
{
	use();
	glCheck(glUniform2f(p_loc, p_value.x, p_value.y));
}

void Shader::setVec2UniformStatic(GLint p_loc, glm::vec2 p_value)
{
	glCheck(glUniform2f(p_loc, p_value.x, p_value.y));
}

GLint Shader::addVec3Uniform(std::string_view p_name, glm::vec3 p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::VEC3, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform3f(loc, p_value.x, p_value.y, p_value.z));
	return loc;
}

void Shader::setVec3Uniform(GLint p_loc, glm::vec3 p_value) const
{
	use();
	glCheck(glUniform3f(p_loc, p_value.x, p_value.y, p_value.z));
}

void Shader::setVec3UniformStatic(GLint p_loc, glm::vec3 p_value)
{
	glCheck(glUniform3f(p_loc, p_value.x, p_value.y, p_value.z));
}

GLint Shader::addVec4Uniform(std::string_view p_name, glm::vec4 p_value)
{
	use();
	GLint loc = glGetUniformLocation(program->ID, p_name.data());
	Uniform u = Uniform{ p_name, UniformTypes::VEC4, p_value };
	u.loc = loc;
	m_uniforms.push_back(u);
	glCheck(glUniform4f(loc, p_value.x, p_value.y, p_value.z, p_value.w));
	return loc;
}

void Shader::setVec4Uniform(GLint p_loc, glm::vec4 p_value) const
{
	use();
	glCheck(glUniform4f(p_loc, p_value.x, p_value.y, p_value.z, p_value.w));
}

void Shader::setVec4UniformStatic(GLint p_loc, glm::vec4 p_value)
{
	glCheck(glUniform4f(p_loc, p_value.x, p_value.y, p_value.z, p_value.w));
}

GLuint Shader::getUniformLoc(std::string_view uniformName)
{
	GLint loc = -1;
	for (auto& u : m_uniforms) {
		if (u.name == uniformName) loc = u.loc;
	}
	return loc;
}

GLuint Shader::getUniformBlockIndex(std::string_view uniformBlockName)
{
	GLuint loc = glGetUniformBlockIndex(program->ID, uniformBlockName.data());
	if (loc == GL_INVALID_INDEX) {
		ERROR_LOG("Uniform block not found.");
	}
	return loc;
}
