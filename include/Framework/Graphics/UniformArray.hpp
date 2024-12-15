#pragma once
#include "Framework/Graphics/GlIDs.hpp"
#include "Framework/Graphics/GlCheck.hpp"

// Uniform arrays are to be used with uniforms of the following layout:
/*
uniform ArrayBlock {
    T elm[MAX_COUNT];
}
*/
template<typename T>
class UniformArray {
public:
	UniformArray(GLuint p_baseIndex, size_t p_maxCount) {
		maxSize = p_maxCount * sizeof(T);
		glCheck(glGenBuffers(1, &UBO.ID));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glCheck(glBufferData(GL_UNIFORM_BUFFER, maxSize, nullptr, GL_DYNAMIC_DRAW));
		glCheck(glBindBufferBase(GL_UNIFORM_BUFFER, p_baseIndex, UBO.ID));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	// it is the user's responsibility to provide an adequately sized pointer
	void setData(T* p_data, size_t p_elmCount) {
		if (p_elmCount * sizeof(T) >= maxSize) throw std::out_of_range("Uniform array was sent too much data and could not contain it all.");
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glCheck(glBufferSubData(GL_UNIFORM_BUFFER, 0, p_elmCount * sizeof(T), p_data));
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	// for the case where the uniform block consists of a simple array of type T
	void setAt(size_t p_index, const T& p_element) {
		if (p_index * sizeof(T) >= maxSize) throw std::out_of_range("Uniform array index out of range.");
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glBufferSubData(GL_UNIFORM_BUFFER, p_index * sizeof(T), sizeof(T), (void*)&p_element);
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
	// for the case where the uniform block consists of a simple array of type T
	void setAt(size_t p_index, T&& p_element) {
		if (p_index * sizeof(T) >= maxSize) throw std::out_of_range("Uniform array index out of range.");
		glCheck(glBindBuffer(GL_UNIFORM_BUFFER, UBO.ID));
		glBufferSubData(GL_UNIFORM_BUFFER, p_index * sizeof(T), sizeof(T), (void*)&p_element);
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