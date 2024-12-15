#ifndef PIXMAP_H
#define PIXMAP_H
#include <util/ext/glm/glm.hpp>
#include <iostream>
#include "util/Array2D.hpp"

/// A class that allows for simple modification of image data using the CPU.
class Pixmap
{
public:
	/** Default constructor. 
	* Width and height of 0px, uninitialized data array.
	*/
	Pixmap();
	/** Constructor with initialized width and height. 
	* By default, every pixel is set to black.
	* @param p_width, p_height - The width and height of the pixmap.
	*/
	Pixmap(uint32_t p_width, uint32_t p_height);

	uint32_t width = 0;
	uint32_t height = 0;

	void resize(uint32_t p_width, uint32_t p_height);
	/// Returns an RGBA vec4 based on a given x and y position.
	/// Attempting to get a pixel outside of the image's dimensions returns the out of bounds color (default: black and transparent).
	glm::vec4 getPixel(uint32_t p_x, uint32_t p_y) const;
	// Appends given data to the bottom of the pixmap
	// Warning: ensure that the data you pass in has the same width as the pixmap.
	void appendImage(unsigned char* p_data, size_t p_size);

	void appendEmpty(size_t rows);
	void prependEmpty(size_t rows);

	/// Changes the top and bottom of the array because opengl is stinky
	void reverse();

	void toPNG(const std::string& name);
	/// Sets a pixel at a given x and y based on the provided RGBA vec4.
	/// Attempting to set a pixel outside of the image's dimensions does nothing.
	void setPixel(uint32_t p_x, uint32_t p_y, glm::vec4 p_color);
	// UNSAFE, susceptable to buffer overruns if pixmap is missized
	void setData(glm::vec4* p_data);
	/// Sets the color of every pixel to opaque black.
	void clear();
	/// Fills the image with the supplied color.
	void fill(glm::vec4 p_color);
	/// @returns A pointer to the Pixmap data on the heap. Used by OpenGL textures.
	glm::vec4* getData();
	/// A function for testing purposes, prints every row and column of the pixel data to the console. Can lag.
	void logPixmap();

	glm::vec4 outOfBoundsColor = glm::vec4(0);
private:
	/// An array of pixels, where each pixel is an RGBA vec4, and every value is between 0 and 1.
	Array2D<glm::vec4> m_pixels;
};

#endif