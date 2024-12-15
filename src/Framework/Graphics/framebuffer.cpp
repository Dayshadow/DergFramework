#include "Framework/Graphics/FrameBuffer.hpp"


FrameBuffer::FrameBuffer() : m_dimensions(glm::uvec2(0, 0)), m_depthBuffer(GL_NONE) {
	m_colorTextures.emplace_back();

}

void FrameBuffer::setDimensions(uint32_t p_width, uint32_t p_height)
{
	if (m_dimensions.x == p_width && m_dimensions.y == p_height) return;
	m_dimensions.x = p_width;
	m_dimensions.y = p_height;
	m_viewport = glm::ivec4(0, 0, p_width, p_height);
	if (!m_initialized) {
		init();
	}
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));
	for (int i = 0; i < m_colorTextures.size(); i++) {
		// By default, every color texture will be the same size as the entire frame
		m_colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTextures[i].glID->ID, 0));
	}

	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();

}

void FrameBuffer::setDimensions(glm::uvec2 p_dimensions)
{
	m_dimensions = p_dimensions;
	m_viewport = glm::ivec4(0, 0, p_dimensions.x, p_dimensions.y);
	if (!m_initialized) {
		init();
	}
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));
	for (int i = 0; i < m_colorTextures.size(); i++) {
		m_colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		// only supports 2d color attachments for now. logical assumption.
		glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTextures[i].glID->ID, 0));
	}

	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y)); // rescale depth buffer
		glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");
	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	useViewport();
}

void FrameBuffer::addColorAttachment()
{
	m_colorTextures.emplace_back();
}

void FrameBuffer::addColorAttachment(Texture& tex)
{
	m_colorTextures.emplace_back(tex);
}

void FrameBuffer::init() {
	glCheck(glGenFramebuffers(1, &m_frameBuffer->ID));

	if (m_useDepth) {
		glCheck(glGenRenderbuffers(1, &m_depthBuffer));
	}

	glCheck(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->ID));

	for (int i = 0; i < m_colorTextures.size(); i++) {
		// Allocates texture
		m_colorTextures[i].changeDimensions(m_dimensions.x, m_dimensions.y);
		// constant for now
		m_colorTextures[i].setFiltering(GL_NEAREST, GL_NEAREST);
		glCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTextures[i].glID->ID, 0));

	}

	if (m_useDepth) {
		glCheck(glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer));
		glCheck(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_dimensions.x, m_dimensions.y));
		glCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer));
	}

	glCheck(glDrawBuffers((GLsizei)m_DrawBuffers.size(), m_DrawBuffers.data()));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::exception("Frame buffer is not okie dokie");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_initialized = true;
}

void FrameBuffer::setColorAttachments(std::initializer_list<GLenum> p_attachments)
{
	m_DrawBuffers = p_attachments;
}


Texture FrameBuffer::getColorTex(size_t p_index)
{
	if (p_index > m_colorTextures.size() - 1) throw std::exception("Tried to get frame buffer color attachment outside of range.");
	return m_colorTextures[p_index];
}
Texture& FrameBuffer::getColorTexRef(size_t p_index)
{
	if (p_index > m_colorTextures.size() - 1) throw std::exception("Tried to get frame buffer color attachment outside of range.");
	return m_colorTextures[p_index];
}

void FrameBuffer::getPixels(size_t p_colorBufferIndex, uint8_t p_channels, Array2D<uint8_t>& o_out)
{
	// Set the width
	o_out.resize(m_colorTextures[p_colorBufferIndex].width * p_channels, 0);
	o_out.reserve(m_dimensions.x * m_dimensions.y * p_channels);

	uint8_t* tmp = (uint8_t*)malloc(m_dimensions.x * m_dimensions.y * p_channels);
	bind();
	glReadPixels(0, 0, m_dimensions.x, m_dimensions.y, m_colorTextures[p_colorBufferIndex].channels, GL_UNSIGNED_BYTE, tmp);
	o_out.append(tmp, m_dimensions.x * m_dimensions.y * p_channels);
	free(tmp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::getPixels(size_t p_colorBufferIndex, uint8_t p_channels, StaticArray2D<uint8_t>& o_out)
{
	// Set the width
	o_out.resize(m_colorTextures[p_colorBufferIndex].width * p_channels, m_colorTextures[p_colorBufferIndex].height);

	uint8_t* tmp = (uint8_t*)malloc(m_dimensions.x * m_dimensions.y * (uint32_t)p_channels);
	bind();
	glReadPixels(0, 0, m_dimensions.x, m_dimensions.y, m_colorTextures[p_colorBufferIndex].channels, GL_UNSIGNED_BYTE, tmp);
	o_out.setData(tmp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::useDepth(bool p_bool)
{
	m_useDepth = p_bool;
}

void FrameBuffer::clearDepthRegion(GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height)
{
	if (!m_useDepth) throw std::exception("Cannot clear depth buffer of FBO that doesn't use one.");
	bind();
	glCheck(glEnable(GL_SCISSOR_TEST));
	glCheck(glScissor(p_x, p_y, p_width, p_height));
	glCheck(glClear(GL_DEPTH_BUFFER_BIT));
	glCheck(glDisable(GL_SCISSOR_TEST));
}
