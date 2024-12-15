#include "GUITextField.hpp"
#include "util/utils.hpp"
void GUITextField::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_usingScreenBounds) {
		if (autoScreenWidth) setScreenBounds(Rect(screenBounds.xy.x, screenBounds.xy.y, m_fieldText.getMaxPixelWidth(getPixelLineHeight(p_target.getViewportHeight())) + 20.f, screenBounds.wh.y));
		if (autoScreenHeight) setScreenBounds(Rect(screenBounds.xy.x, screenBounds.xy.y, screenBounds.wh.x, m_fieldText.getMaxPixelHeight(getPixelLineHeight(p_target.getViewportHeight())) + 20.f));
		updateScreenBounds(p_target.getViewportWidth(), p_target.getViewportHeight());
	}

	if (m_backgroundEnabled) {
		m_backgroundSprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		m_backgroundSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
		gs.solidColorShader.setVec3Uniform(gs.solidColor_colorUniformLoc, backgroundColor);
		gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, backgroundOpacity);
		m_backgroundSprite.draw(p_target, p_states);
		gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, 1.f);
	}

	if (!m_useRelativeScaling) {
		float screenHeightProportion = m_textHeight / p_target.getViewportHeight();
		m_fieldText.setScale(glm::vec2(screenHeightProportion * (1.f / p_target.getAspect()), -screenHeightProportion));
	}
	else {
		m_fieldText.setScale(glm::vec2(absoluteBounds.wh.x * m_textHeight * (1.f / p_target.getAspect()) * (1.f / absoluteBounds.getAspect()), -absoluteBounds.wh.y * m_textHeight));
	}
	if (centered) {
		float textWidthAbsolute;
		float textHeightAbsolute;
		float pixelCharHeight = 0.f;
		if (m_useRelativeScaling) { // in this case, textHeight is in local units
			textWidthAbsolute = m_fieldText.getMaxPixelWidth(pixelCharHeight) / p_target.getViewportWidth();
			textHeightAbsolute = m_textHeight * m_fieldText.getMaxNormalizedHeight() * absoluteBounds.wh.y; // the constant is a bit silly but looks nicer due to the font being bottom heavy
		}
		else { // in this case, text height is in pixels
			textWidthAbsolute = m_fieldText.getMaxPixelWidth(m_textHeight) / p_target.getViewportWidth();
			textHeightAbsolute = ((m_textHeight * m_fieldText.getMaxNormalizedHeight()) / p_target.getViewportHeight());
		}
		float newX = (absoluteBounds.wh.x - textWidthAbsolute) / 2.f + absoluteBounds.xy.x;
		// we shift it up a little to make it pretty
		float newY = (absoluteBounds.wh.y - textHeightAbsolute) / 2.f + absoluteBounds.xy.y + m_fieldText.getNormalizedLineHeight() * 0.75f * (getPixelLineHeight(p_target.getViewportHeight()) / p_target.getViewportHeight());

		m_fieldText.setPosition(glm::vec3(newX, newY, 0.f));
	}
	else {
		m_fieldText.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y + absoluteBounds.wh.y, 0.f));
	}
	m_fieldText.draw(textColor, p_target, p_states);
	float test = m_fieldText.getMaxPixelWidth(m_textHeight);

	Widget::draw(p_target, p_states);
}

void GUITextField::setText(std::string_view p_text)
{
	m_textString = p_text;
	m_fieldText.setText(p_text);
}

void GUITextField::appendText(std::string_view p_text)
{
	m_textString += p_text;
	m_fieldText.setText(m_textString);
}

void GUITextField::setTextHeight(float p_height)
{
	m_textHeight = p_height;
}

void GUITextField::disableBackground()
{
	m_backgroundEnabled = false;
}

void GUITextField::enableBackground()
{
	m_backgroundEnabled = true;
}



void GUITextField::disableRelativeScaling()
{
	m_useRelativeScaling = false;
}

void GUITextField::enableRelativeScaling()
{
	m_useRelativeScaling = true;
}

float GUITextField::getPixelLineHeight(float viewportHeight)
{
	if (m_useRelativeScaling) { // in this case, textHeight is in local units
		return m_textHeight * (absoluteBounds.wh.y * viewportHeight);
	}
	else {
		return m_textHeight;
	}
}
