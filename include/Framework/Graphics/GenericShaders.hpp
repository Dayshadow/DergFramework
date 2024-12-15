#pragma once
#include "Framework/Graphics/Shader.hpp"
#include <memory>

// I need to wrap all the generic shaders in a class so I can instantiate it after opengl initilizes and re-use them in various places without recompiling.
class GenericShaders {
public:
	GenericShaders(const GenericShaders&) = delete;

	static GenericShaders& Get() {
		static GenericShaders instance;
		return instance;
	}

	// Basic Image Shader
	// Very simple, draws the texture bound to unit 0 based on the texuture coordinates provided.
	// Attributes to use:
	// vec3 position
	// vec2 texcoord
	// Uniforms:
	// imageTexture: 0
	Shader imageShader;
	GLint image_imageTextureUniformLoc = 0;
	GLint image_opacityUniformLoc = 0;
	// Basic Solid Color Shader
	// Fills fragments with a flat color
	// Attributes to use:
	// vec3 position
	// vec2 texcoord (ignored, but there)
	// Uniforms:
	// vec3 color
	Shader solidColorShader;
	GLint solidColor_colorUniformLoc = 0;
	GLint solidColor_opacityUniformLoc = 0;
	// for funsies
	// attribs:
	// vec3 pos
	// vec2 texCoord
	// uniform is time at location 1
	Shader fancyShader;
	GLint fancyShader_iTimeUniformLoc = 0;

	Shader textShader;
	GLint text_fontAtlasUniformLoc = 0;
	GLint text_textColUniformLoc = 0;

	Shader win95Shader;
	GLint win95_pixelBoundsUniformLoc = 0;
	GLint win95_opacityUniformLoc = 0;
private:
	GenericShaders();
	void init();
};