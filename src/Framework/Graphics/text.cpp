#include "Framework/Graphics/Text.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <util/ext/glm/gtc/matrix_transform.hpp>

TextContext::TextContext() {

	if (FT_Init_FreeType(&ft))
		ERROR_LOG("FreeType init failed. That sucks.");
}

TextContext::~TextContext() {
	FT_Done_FreeType(ft);
}

Font::Font(const char* p_filePath, TextContext& p_host) {
	if (FT_New_Face(p_host.ft, p_filePath, 0, &m_face))
		ERROR_LOG("FreeType face load failed. That sucks.");
}

void Font::setPixelHeight(uint16_t p_h) {
	m_sizeSet = true;
	FT_Set_Pixel_Sizes(m_face, 0, p_h);
	lineHeight = m_face->size->metrics.height >> 6;
	if (m_atlasInitialized) {
		m_atlas.reset();
		createAtlas();
	}
	else {
		createAtlas();
	}
	charHeight = p_h;
}

void Font::createAtlas() {
	if (!m_sizeSet) {
		ERROR_LOG("Tried to create a font atlas without specifying character size.");
	}
	int dimEstimate = (1 + (m_face->size->metrics.height >> 6)) * (int)std::ceilf(std::sqrtf((float)m_maxGlyphCount));
	texDims = 1;
	while (texDims < (uint32_t)dimEstimate) texDims <<= 1;

	uint8_t* pixelDataRaw = (uint8_t*)calloc(texDims * texDims, 1);
	m_atlas.resize(texDims, texDims);
	m_atlas.setData(pixelDataRaw);

	// The position of the current character within the atlas
	int charX = 0;
	int charY = 0;
	for (uint8_t c = 0; c < m_maxGlyphCount; c++) {
		if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
			ERROR_LOG("FreeType glyph load failed. That sucks.");

		FT_Bitmap* bmp = &m_face->glyph->bitmap;

		if (charX + bmp->width > (int)texDims) {
			charX = 0;
			charY += ((m_face->size->metrics.height >> 6) + 1);
		}

		// A simple 2d array paste.
		// x and y in this context are coordinates within the character's bitmap.
		for (unsigned int y = 0; y < bmp->rows; y++) {
			for (unsigned int x = 0; x < bmp->width; x++) {
				m_atlas(charX + x, charY + y) = bmp->buffer[y * bmp->pitch + x];
			}
		}

		charData[c] = FreeTypeCharacter{
			glm::vec2(bmp->width, bmp->rows),
			glm::vec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
			glm::vec2(charX, charY) / (float)texDims,
			uint32_t(m_face->glyph->advance.x >> 6)
		};
		charX += bmp->width + 1;
	}
	m_atlasInitialized = true;
	//StaticArray2D<uint8_t> out;
	//uint8_t* pngData = (uint8_t*)calloc(texDims * texDims * 4, 1);
	//out.resize(texDims * 4, texDims);
	//out.setData(pngData);
	//for (int y = 0; y < texDims; y++) {
	//	for (int x = 0; x < texDims * 4; x += 4) {
	//		out(x, y) = m_atlas(x / 4, y);
	//		out(x + 1, y) = m_atlas(x / 4, y);
	//		out(x + 2, y) = m_atlas(x / 4, y);
	//		out(x + 3, y) = 255;
	//	}
	//}
	//stbi_write_png("testfont.png", texDims, texDims, 4, out.getData(), texDims * 4);
}

Texture Font::getTexture() {
	if (m_atlasTexture.initialized) return m_atlasTexture;
	if (!m_atlasInitialized) {
		if (!m_sizeSet) {
			ERROR_LOG("Can't get texture. Font pixel size not set.");
			return Texture();
		}
		createAtlas();
	}
	m_atlasTexture.setChannels(GL_RED);
	m_atlasTexture.fromByteData((uint32_t)m_atlas.width, (uint32_t)m_atlas.height, m_atlas.getData());
	m_atlasTexture.setFiltering(GL_LINEAR, GL_LINEAR);
	return m_atlasTexture;
}

void Font::setMaxGlyphCount(uint8_t p_count) {
	m_maxGlyphCount = p_count;
}


Text::Text(Font& p_font) : m_font(p_font)
{
	m_textMesh.setStreamType(GL_DYNAMIC_DRAW);
	m_textMesh.addFloatAttrib(2); // xy
	m_textMesh.addFloatAttrib(2); // texture coordinates

}

Text::Text(Font& p_font, std::string_view p_initialText) : m_font(p_font)
{
	m_textMesh.setStreamType(GL_DYNAMIC_DRAW);
	m_textMesh.addFloatAttrib(3); // xyz
	m_textMesh.addFloatAttrib(2); // texture coordinates

	m_text = p_initialText;
	//generateVBO();
}

void Text::setText(std::string_view p_newText) {
	if (m_text == p_newText) return;
	m_text = p_newText;
	m_textMesh.remove();
}
void Text::setLeftJustification(bool enabled)
{
	leftJustified = enabled;
}
void Text::generateVBO() {
	std::string::const_iterator it;
	float recordWidth = 0.f;
	float recordHeight = 0.f;
	float lineX = 0;
	float lineY = 0;
	uint32_t i = 0;
	const float scale = 1.f/float(m_font.charHeight);
	m_normalizedLineHeight = (float)m_font.lineHeight * scale;

	if (!leftJustified) {
		for (it = m_text.begin(); it != m_text.end(); it++) {
			if (*it == '\n') {
				lineX = 0;
				lineY -= (float)m_font.lineHeight * scale;
				continue;
			}
			FreeTypeCharacter ch = m_font.charData[*it];

			float x = lineX + ch.bearing.x * scale;
			float y = lineY - (ch.size.y - ch.bearing.y) * scale;
			recordWidth = std::max(std::abs(x), recordWidth);
			recordHeight = std::max(std::abs(lineY), recordHeight);

			float w = ch.size.x * scale;
			float h = ch.size.y * scale;

			float tw = ch.size.x / (float)m_font.texDims;
			float th = ch.size.y / (float)m_font.texDims;
			//float tw = 0.0;
			//float th = 0.0;

			m_textMesh.pushVertices({
			x    , y    , 0.f, ch.texCoord.x     , ch.texCoord.y + th,
			x + w, y    , 0.f, ch.texCoord.x + tw, ch.texCoord.y + th,
			x    , y + h, 0.f, ch.texCoord.x     , ch.texCoord.y,
			x    , y + h, 0.f, ch.texCoord.x     , ch.texCoord.y,
			x + w, y    , 0.f, ch.texCoord.x + tw, ch.texCoord.y + th,
			x + w, y + h, 0.f, ch.texCoord.x + tw, ch.texCoord.y
				});

			i += 6;
			lineX += ch.advance * scale;
		}
	}
	else {
		float lineStart = 0.f;
		for (it = m_text.begin(); it != m_text.end(); it++) {
			if (*it == '\n') break;
			lineStart -= m_font.charData[*it].advance * scale;
		}
		for (it = m_text.begin(); it != m_text.end(); it++) {
			if (*it == '\n') {
				lineX = 0;
				lineY -= (float)m_font.lineHeight * scale;
				lineStart = 0.f; 
				auto newIt = it + 1;
				while (newIt != m_text.end()) {
					if (*newIt == '\n') {
						break;
					}
					newIt++;
				}
				newIt--;
				while (newIt != m_text.begin()) {
					if (*newIt == '\n') break;
					lineStart -= m_font.charData[*newIt].advance * scale;
					newIt--;
				}
				continue;
			}
			FreeTypeCharacter ch = m_font.charData[*it];

			float x = lineX + ch.bearing.x * scale + lineStart;
			float y = lineY - (ch.size.y - ch.bearing.y) * scale;
			recordWidth = std::max(std::abs(x), recordWidth);
			recordHeight = std::max(std::abs(y), recordHeight);

			float w = ch.size.x * scale;
			float h = ch.size.y * scale;

			float tw = ch.size.x / (float)m_font.texDims;
			float th = ch.size.y / (float)m_font.texDims;
			//float tw = 0.0;
			//float th = 0.0;

			m_textMesh.pushVertices({
			x    , y    , 0.f, ch.texCoord.x     , ch.texCoord.y + th,
			x + w, y    , 0.f, ch.texCoord.x + tw, ch.texCoord.y + th,
			x    , y + h, 0.f, ch.texCoord.x     , ch.texCoord.y,
			x    , y + h, 0.f, ch.texCoord.x     , ch.texCoord.y,
			x + w, y    , 0.f, ch.texCoord.x + tw, ch.texCoord.y + th,
			x + w, y + h, 0.f, ch.texCoord.x + tw, ch.texCoord.y
				});

			i += 6;
			lineX += ch.advance * scale;
		}
	}
	m_normalizedWidth = recordWidth;
	m_normalizedHeight = recordHeight + m_font.lineHeight * scale; // i donno why this is needed
	//m_textMesh.genIBO();
	m_textMesh.pushVBOToGPU();
}
void Text::draw(const glm::vec3& p_textColor, DrawSurface& p_target, DrawStates& p_drawStates) {
	if (!m_textMesh.hasData()) {
		generateVBO();
	}
	gs.textShader.setTexUniform(gs.text_fontAtlasUniformLoc, 0);

	p_drawStates.setTransform(p_drawStates.m_transform * getObjectTransform());
	Texture tex = m_font.getTexture();
	if (!tex.initialized) {
		ERROR_LOG("Unable to draw text. Invalid font state.");
		return;
	}

	p_drawStates.attachShader(&gs.textShader);
	gs.textShader.setVec3Uniform(gs.text_textColUniformLoc, p_textColor);
	Texture fontTex = m_font.getTexture();
	p_drawStates.attachTexture(fontTex);
	p_target.draw(m_textMesh, GL_TRIANGLES, p_drawStates);
	// don't corrupt the state
	p_drawStates.setTransform(p_drawStates.m_transform * glm::inverse(getObjectTransform()));
}
// position is in screen coordinates, -1...0...1
void Text::draw(const glm::vec2& p_position, float p_pixelHeight, const glm::vec3& p_textColor, DrawSurface& p_target, bool extraLegible) {
	DrawStates d;

	glm::ivec4 viewport = p_target.getViewport();
	float aspect = (float)viewport.w / (float)viewport.z;
	glm::mat4 transform = glm::ortho((float)viewport.x, (float)viewport.z, (float)viewport.y, (float)viewport.w, 0.1f, 1000.0f);
	//setPosition(glm::vec3(p_position.x, p_position.y, 0.f));
	float scale = p_pixelHeight / viewport.w;
	setScale(glm::vec2(2.f * aspect * scale, 2.f * scale));
	setPosition(glm::vec3(p_position.x, p_position.y, 0));
	if (extraLegible) {
		BlendMode b;
		b.srcRGB = GL_ONE_MINUS_DST_COLOR;
		b.srcAlpha = GL_SRC_ALPHA;
		b.dstRGB = GL_ONE_MINUS_SRC_ALPHA;
		b.dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
		b.RGBequation = GL_FUNC_ADD;
		b.AlphaEquation = GL_FUNC_ADD;
		d.setBlendMode(b);
		draw(glm::vec3(1.f), p_target, d);
		setPosition(glm::vec3(0));
		setScale(glm::vec2(1.f));
		return;
	}
	draw(p_textColor, p_target, d);
	setPosition(glm::vec3(0));
	setScale(glm::vec2(1));

}

float Text::getMaxPixelWidth(float p_pixelHeight)
{
	return m_normalizedWidth * p_pixelHeight;
}

float Text::getMaxPixelHeight(float p_pixelHeight)
{
	return m_normalizedHeight * p_pixelHeight;
}

float Text::getMaxNormalizedWidth()
{
	return m_normalizedWidth;
}

float Text::getMaxNormalizedHeight()
{
	return m_normalizedHeight;
}

float Text::getNormalizedLineHeight()
{
	return m_normalizedLineHeight;
}
