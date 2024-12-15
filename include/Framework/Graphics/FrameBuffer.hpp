#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL/glew.h>
#include <util/ext/glm/vec2.hpp>
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/GlCheck.hpp"
#include <initializer_list>
#include <vector>
#include "util/Array2D.hpp"
#include "util/StaticArray2D.hpp"


// Wrapper class for a FBO, which enables rendering directly into textures instead of the screen. Very handy for post processing and offscreen rendering.
// NOTE:: CURRENTLY FIXED TO TEXTURE COLOR ATTACHMENTS, AND RENDERBUFFER DEPTH.
// MIGHT MAKE A SEPARATE RENDERBUFFER-ONLY DOUBLE BUFFER LATER.
class FrameBuffer : public DrawSurface {
public:
	// Defaults to a blank color texture.
	FrameBuffer();
	void init();
	// Affects the size of all color attachments
	void setDimensions(uint32_t p_width, uint32_t p_height);
	void setDimensions(glm::uvec2 p_dimensions);
	void addColorAttachment();
	void addColorAttachment(Texture& tex);
	void setColorAttachments(std::initializer_list<GLenum> p_attachments);
	Texture getColorTex(size_t p_index);
	Texture& getColorTexRef(size_t p_index);
	// Read the pixels directly from the frame buffer into system memory.
	void getPixels(size_t p_colorBufferIndex, uint8_t p_channels, Array2D<uint8_t>& o_out);
	void getPixels(size_t p_colorBufferIndex, uint8_t p_channels, StaticArray2D<uint8_t>& o_out);
	void useDepth(bool p_bool);
	void clearDepthRegion(GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height);
private:
	
	// x width, y height.
	glm::uvec2 m_dimensions;
	/// Vector that contains the color textures.
	std::vector<Texture> m_colorTextures;
	/// The GL buffer ID for the FBO's depth buffer.
	GLuint m_depthBuffer;
	bool m_useDepth = true;
	bool m_initialized = false;

};
#endif