#pragma once
#include "GL/glew.h"
#include "Framework/Log.hpp"

// Bringing them out into a struct, such that it can be deleted when all references are gone.
// I'm not completely certain if this is the best way of doing it, but it insures the associated GL data is purged when not in use.
struct glBuffer {
	GLuint ID = 0;
	~glBuffer() {
		// Probably unneeded
		if (ID == 0) return;
		DELETE_LOG("GL Buffer " << ID << " Deleted.");
		glDeleteBuffers(1, &ID);
	}
};

// uses a different deletion function
struct glVertexArray {
	GLuint ID = 0;
	~glVertexArray() {
		if (ID == 0) return;
		DELETE_LOG("GL Vertex Array " << ID << " Deleted.");
		glDeleteVertexArrays(1, &ID);
	}
};

struct glProgram {
	GLuint ID = 0;
	~glProgram() {
		if (ID == 0) return;
		DELETE_LOG("Deleted shader: " << ID);
		glDeleteProgram(ID);
	}
};

struct glFrameBuffer {
	GLuint ID = 0;
	~glFrameBuffer() {
		if (ID == 0) return;
		DELETE_LOG("Deleted framebuffer: " << ID);
		glDeleteFramebuffers(1, &ID);
	}
};

struct glTexture {
	GLuint ID = 0;
	~glTexture() {
		if (ID == 0) return;
		DELETE_LOG("Deleted texture: " << ID);
		glDeleteTextures(1, &ID);
	}
};