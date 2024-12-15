#pragma once

#include "Framework/Graphics/GenericShaders.hpp"

GenericShaders::GenericShaders()
{
	init();
}

void GenericShaders::init() {

	imageShader = Shader(
		"./src/Shaders/ImageVS.glsl",
		"./src/Shaders/ImageFS.glsl"
);
	glm::mat4 tmp = glm::mat4(1.f);
	imageShader.addMat4Uniform("transform", tmp);
	image_imageTextureUniformLoc = imageShader.addTexUniform("imageTexture", 0);
	image_opacityUniformLoc = imageShader.addFloatUniform("opacity", 0.f);

	solidColorShader = Shader(
		"./src/Shaders/SolidColorVS.glsl",
		"./src/Shaders/SolidColorFS.glsl"
	);
	solidColorShader.addMat4Uniform("transform", tmp);
	solidColor_colorUniformLoc = solidColorShader.addVec3Uniform("col", glm::vec3(0.f));
	solidColor_opacityUniformLoc = solidColorShader.addFloatUniform("opacity", 1.f);
	
	fancyShader = Shader(
		"./src/Shaders/ImageVS.glsl",
		"./src/Shaders/FancyFS.glsl"
	);
	fancyShader.addMat4Uniform("transform", tmp);
	fancyShader_iTimeUniformLoc = fancyShader.addFloatUniform("iTime", 0.f);

	textShader = { "./src/Shaders/TextVS.glsl" , "./src/Shaders/TextFS.glsl" };
	textShader.addMat4Uniform("transform", tmp);
	text_fontAtlasUniformLoc = textShader.addTexUniform("fontAtlas", 0);
	text_textColUniformLoc = textShader.addVec3Uniform("textCol", glm::vec3(1.f));

	win95Shader = { "./src/Shaders/Win95BgVS.glsl", "./src/Shaders/Win95BgFS.glsl" };
	win95Shader.addMat4Uniform("transform", tmp);
	win95_pixelBoundsUniformLoc = win95Shader.addVec2Uniform("pixelBounds", glm::vec2(100.f));
	win95_opacityUniformLoc = win95Shader.addFloatUniform("opacity", 1.f);

}