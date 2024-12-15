#include "Framework/Graphics/Pixmap.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

Pixmap::Pixmap()
{
}

Pixmap::Pixmap(uint32_t p_width, uint32_t p_height) :
	width(p_width),
	height(p_height),
	m_pixels(p_width, p_height)
{
}

void Pixmap::resize(uint32_t p_width, uint32_t p_height)
{
	m_pixels.resize(p_width, p_height);
	width = p_width;
	height = p_height;
}

glm::vec4 Pixmap::getPixel(uint32_t p_x, uint32_t p_y) const
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return outOfBoundsColor;
	}
	return m_pixels(p_x, p_y);
}
void Pixmap::appendImage(unsigned char* p_data, size_t p_size)
{
	if (p_size < 4) return;
	if (p_size % 4 != 0)
		throw std::exception("Pixel component mismatch!");
	if ((p_size / 4) % width != 0)
		throw std::exception("Width mismatch!");
	// p_size is the number of byte components, and there is one byte component per float component
	glm::vec4* tmp = (glm::vec4*)malloc(p_size * sizeof(float));
	if (!tmp) {
		throw std::exception("Malloc failed! Texture not appended.");
		return;
	}
	memset(tmp, 0, p_size * sizeof(float));
	for (size_t i = 0; i < p_size / 4; i++) {
		tmp[i].r = (float)p_data[i * 4 + 0] / 255.f;
		tmp[i].g = (float)p_data[i * 4 + 1] / 255.f;
		tmp[i].b = (float)p_data[i * 4 + 2] / 255.f;
		tmp[i].a = (float)p_data[i * 4 + 3] / 255.f;
	}
	m_pixels.append(tmp, p_size / 4);
	height = (uint32_t)m_pixels.height;
	free(tmp);
}
void Pixmap::appendEmpty(size_t rows)
{
	unsigned char* buf = (unsigned char*)calloc(rows, width * sizeof(float) * 4);
	m_pixels.append((glm::vec4*)buf, width * rows);
	free(buf);
	height += rows;
}
void Pixmap::prependEmpty(size_t rows)
{
	unsigned char* buf = (unsigned char*)calloc(rows, width * sizeof(float) * 4);
	m_pixels.prepend((glm::vec4*)buf, width * rows);
	free(buf);
	height += rows;
}
void Pixmap::reverse() {
	m_pixels.reverse();
}
void Pixmap::toPNG(const std::string& name)
{
	char* buf = (char*)malloc(width * height * 4);
	if (buf == nullptr) {
		ERROR_LOG("Unable to write PNG! could not allocate heap memory");
		return;
	}
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			auto pix = getPixel(x, y);
			int ind = (y * width + x) * 4;
			buf[ind + 0] = char(pix.r * 255.f);
			buf[ind + 1] = char(pix.g * 255.f);
			buf[ind + 2] = char(pix.b * 255.f);
			buf[ind + 3] = char(pix.a * 255.f);
		}
	}

	stbi_write_png(name.c_str(), width, height, 4, buf, width * 4);
	free(buf);
}
void Pixmap::setPixel(uint32_t p_x, uint32_t p_y, glm::vec4 p_color)
{
	if ((p_x < 0) || (p_x > width - 1) || (p_y < 0) || (p_y > height - 1)) {
		return;
	}
	m_pixels(p_x, p_y) = p_color;
}
void Pixmap::setData(glm::vec4* p_data)
{
	m_pixels.setData(p_data);
}
void Pixmap::clear() {
	m_pixels.fill(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}
void Pixmap::fill(glm::vec4 p_color)
{
	m_pixels.fill(p_color);
}
glm::vec4* Pixmap::getData() {
	return m_pixels.getData().data();
}
void Pixmap::logPixmap() {
	for (uint32_t i = 0; i < height; i++) {
		for (uint32_t j = 0; j < width; j++) {
			std::cout << m_pixels(j, i).r;
			std::cout << " ";
			std::cout << m_pixels(j, i).g;
			std::cout << " ";
			std::cout << m_pixels(j, i).b;
			std::cout << " ";
			std::cout << m_pixels(j, i).a;
			std::cout << ", ";
		}
		std::cout << std::endl;
	}
}