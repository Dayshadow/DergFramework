#pragma once
#include <ft2build.h>
#include <util/ext/glm/vec2.hpp>
#include "Framework/Log.hpp"
#include "util/StaticArray2D.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/TransformObject.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include <string>
#include <map>
#include <string_view>

#include FT_FREETYPE_H  


// Handles freetype text library
class Font;
class TextContext {
public:
	TextContext();
	~TextContext();

	friend class Font;
private:
	FT_Library ft;
};
struct FreeTypeCharacter {
	glm::vec2 size;
	glm::vec2 bearing;
	glm::vec2 texCoord;
	unsigned int advance;
};

class Font {
public:
	Font(const char* p_filePath, TextContext& p_host);

	// Warning: Can be a heavy operation if the atlas is already generated.
	// If you want to scale text, it's better to use transforms
	void setPixelHeight(uint16_t p_h);
	void createAtlas();
	// Can return nullptr if not successful
	Texture getTexture();

	void setMaxGlyphCount(uint8_t count);

	friend class TextContext;
	std::map<char, FreeTypeCharacter> charData;
	uint32_t texDims = 0;
	uint32_t charHeight = 0;
	uint32_t lineHeight = 0;
private:
	FT_Face m_face;
	bool m_atlasInitialized = false;
	bool m_sizeSet = false;
	uint8_t m_maxGlyphCount = 255;
	StaticArray2D<uint8_t> m_atlas;
	Texture m_atlasTexture;
};

// Drawable text
class Text : public TransformObject {
public:
	Text(Font& p_font);
	Text(Font& p_font, std::string_view p_initialText);

	void setText(std::string_view p_newText);
	void setLeftJustification(bool enabled);
	void draw(const glm::vec3& p_textColor, DrawSurface& p_target, DrawStates& p_drawStates);
	void draw(const glm::vec2& p_position, float p_pixelHeight, const glm::vec3& p_textColor, DrawSurface& p_target, bool extraLegible = false);
	float getMaxPixelWidth(float p_pixelHeight);
	float getMaxPixelHeight(float p_pixelHeight);
	// width in terms of height
	float getMaxNormalizedWidth();
	// i forgot what this one was in terms of lol
	float getMaxNormalizedHeight();
	float getNormalizedLineHeight();
private:
	void generateVBO();
	Mesh<float> m_textMesh;
	std::string m_text;
	Font& m_font;
	bool leftJustified = false;
	// this width is kinda baked in terms of text height, use handy function to convert to pixels instead
	float m_normalizedWidth = 0.f;
	float m_normalizedHeight = 0.f;
	float m_normalizedLineHeight = 0.f;
	GenericShaders& gs = GenericShaders::Get();
	//static Shader textShader;
};