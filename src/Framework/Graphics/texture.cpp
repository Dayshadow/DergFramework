#include "Framework/Graphics/Texture.hpp"
#include "util/utils.hpp"
#include "Framework/Graphics/GlCheck.hpp"

#include "Framework/Log.hpp"
#include "Framework/Graphics/GlCheck.hpp"

Texture::Texture()
{

}

Texture::Texture(std::string_view p_assignedID) :
	texID(p_assignedID)
{
}
Texture::Texture(uint32_t p_width, uint32_t p_height, glm::vec4* p_data) :
	width(p_width),
	height(p_height)
{
	fromVec4Data(width, height, p_data);
}
Texture::Texture(uint32_t p_width, uint32_t p_height, glm::vec4* p_data, GLenum p_type) :
	width(p_width),
	height(p_height),
	type(p_type)
{
	fromVec4Data(width, height, p_data);
}

void Texture::setFiltering(GLenum p_min, GLenum p_mag) {
	m_filteringMag = p_mag;
	m_filteringMin = p_min;

	if (initialized) {
		glBindTexture(type, glID->ID);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag);
		glBindTexture(type, 0);
	}
}
void Texture::setWrapping(GLint p_mode) {
	m_wrappingMode = p_mode;
	if (initialized) {
		glBindTexture(type, glID->ID);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode);
		glBindTexture(type, 0);
	}
}

void Texture::setType(GLenum p_type)
{
	type = p_type;
}

void Texture::setChannels(GLenum p_channels)
{
	if (!initialized) glGenTextures(1, &glID->ID);
	glBindTexture(type, glID->ID); // into the main texture buffer
	channels = p_channels;
	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		channels,
		width,
		height,
		0,
		channels,
		GL_UNSIGNED_BYTE,
		nullptr);
	glBindTexture(type, 0);
}

void Texture::fromByteData(uint32_t p_width, uint32_t p_height, unsigned char* p_data)
{
	width = p_width;
	height = p_height;
	if (!initialized) glGenTextures(1, &glID->ID);
	glBindTexture(type, glID->ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag);

	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		channels,
		p_width,
		p_height,
		0,
		channels,
		GL_UNSIGNED_BYTE,
		p_data);
	glBindTexture(type, 0);
	initialized = true;
}

void Texture::fromVec4Data(uint32_t p_width, uint32_t p_height, glm::vec4* p_data)
{
	width = p_width;
	height = p_height;
	if (!initialized) glGenTextures(1, &glID->ID);
	glBindTexture(type, glID->ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag);

	glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		channels,
		p_width,
		p_height,
		0,
		channels,
		GL_FLOAT,
		p_data);
	glBindTexture(type, 0);
	initialized = true;
}

void Texture::useMipmaps(int p_count)
{
	if (!initialized) {
		changeDimensions(0, 0);
	}
	glCheck(glBindTexture(type, glID->ID)); // into the main texture buffer
	glCheck(glGenerateMipmap(type));
	glCheck(glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, p_count));
	glCheck(glBindTexture(type, 0));

}

void Texture::genMipMapsBytes(uint8_t p_level, uint32_t p_width, uint32_t p_height, uint8_t* p_data) {
	if (channels != GL_RGBA) {
		ERROR_LOG("Sorry, this function does not support textures with non-RGBA format at the moment.");
		return;
	}
	width = p_width;
	height = p_height;
	if (!initialized) glGenTextures(1, &glID->ID);
	glBindTexture(type, glID->ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode));
	glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode));
	glCheck(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin));
	glCheck(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag));

	for (int i = 0; i <= p_level; i++) {
		int n = (int)std::pow(2.0, (double)i);

		uint8_t* newDat = utils::divideRes(n, width, width * height * 4, p_data);
		//stbir_resize_uint8(pngdat, width, height, width * 4, newDat, newW, newH, newW * 4, 4);

		//char txt[] = "Filex.png";
		//txt[4] = '0' + i;
		//stbi_write_png(txt, width / n, height / n, 4, newDat, (width * 4) / n);
		glCheck(glTexImage2D(type, i, channels, width / n, height / n, 0, channels, GL_UNSIGNED_BYTE, newDat));

		free(newDat);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, p_level);

	initialized = true;

	glBindTexture(type, 0);
}
void Texture::genMipMapsFloat(uint8_t p_level, uint32_t p_width, uint32_t p_height, float* p_data) {
	if (channels != GL_RGBA) {
		ERROR_LOG("Sorry, this function does not support textures with non-RGBA format at the moment.");
		return;
	}
	width = p_width;
	height = p_height;
	if (!initialized) glGenTextures(1, &glID->ID);
	glBindTexture(type, glID->ID); // into the main texture buffer

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode));
	glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode));
	glCheck(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin));
	glCheck(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag));

	for (int i = 0; i <= p_level; i++) {
		uint8_t* fullImage = utils::toRGBAUnsignedCharArray(p_data, width * height * 4);
		int n = (int)std::pow(2.0, (double)i);

		uint8_t* newDat = utils::divideRes(n, width, height, fullImage);
		glCheck(glTexImage2D(type, i, channels, width / n, height / n, 0, channels, GL_UNSIGNED_BYTE, newDat));

		free(newDat);
		free(fullImage);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, p_level);

	initialized = true;

	glBindTexture(type, 0);
}

void Texture::changeDimensions(uint32_t p_width, uint32_t p_height)
{
	width = p_width;
	height = p_height;

	if (!initialized) {
		glCheck(glGenTextures(1, &glID->ID));
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_S, m_wrappingMode));
		glCheck(glTexParameteri(type, GL_TEXTURE_WRAP_T, m_wrappingMode));
		glCheck(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, m_filteringMin));
		glCheck(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_filteringMag));
	}
	glCheck(glBindTexture(type, glID->ID)); // into the main texture buffer

	glCheck(glTexImage2D( // actually put the image data into the texture buffer
		type,
		0,
		channels,
		p_width,
		p_height,
		0,
		channels,
		GL_UNSIGNED_BYTE,
		NULL));

	// WARNING:: very picky about if an image in in RGB format or RBGA format. Try to keep them all RGBA with a bit depth of 8
	glBindTexture(type, 0);
	initialized = true; // data is allocated, so we'll just say it's initialized even if the data is undefined. Solves a bug.
}

void Texture::subVec4Data(glm::vec4* p_data) {
	CONDITIONAL_LOG(!initialized, "Texture not initialized, so data cannot be substituted.");
	if (!initialized) return;
	glBindTexture(type, glID->ID);
	glTexSubImage2D(type, 0, 0, 0, width, height, channels, GL_FLOAT, p_data);
	glBindTexture(type, 0);
}

void Texture::remove() {
	initialized = false;
}

uint32_t Texture::getPixelCount()
{
	return width * height;
}
