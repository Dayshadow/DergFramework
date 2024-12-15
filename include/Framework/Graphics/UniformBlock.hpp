#pragma once
#include "Framework/Graphics/GlIDs.hpp"
#include "Framework/Graphics/GlCheck.hpp"

// uniform blocks are to be used when the data is simply arranged like a struct of type T, a little more manual than an array
template<typename T>
class UniformBlock {
public:
	UniformBlock(GLuint p_baseIndex) {
		maxSize = sizeof(T);
		glCheck(glGenBuffers(1, &UBO.ID));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glCheck(glBufferData(GL_UNIFORM_BUFFER, maxSize, nullptr, GL_DYNAMIC_DRAW));
		glCheck(glBindBufferBase(GL_UNIFORM_BUFFER, p_baseIndex, UBO.ID));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	void setData(T* p_data) {
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glCheck(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), p_data));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	void setWithByteOffset(void* p_data, size_t p_offset, size_t p_size) {
		if (p_offset + 4 >= maxSize) throw std::out_of_range("Uniform array index out of range.");
		if (p_offset % 4 != 0) throw std::invalid_argument("Specified offset violates 4-byte alignment.");

		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glBufferSubData(GL_UNIFORM_BUFFER, p_offset, p_size, p_data);
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	void bindTo(GLuint p_bufferBase) {
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glCheck(glBindBufferBase(GL_UNIFORM_BUFFER, p_bufferBase, UBO.ID));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}

private:
	glBuffer UBO;
	size_t maxSize = 0;
};