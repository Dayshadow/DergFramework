#include "GUIContainer.hpp"

GUIContainer::GUIContainer(std::string_view p_ID) : Widget(p_ID)
{
	m_backgroundSprite.attachShader(&gs.solidColorShader);
}

void GUIContainer::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_usingScreenBounds) {
		updateScreenBounds(p_target.getViewportWidth(), p_target.getViewportHeight());
	}
	if (m_backgroundEnabled) {
		m_backgroundSprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		m_backgroundSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
		if (!m_win95Bg) {
			gs.solidColorShader.setVec3Uniform(gs.solidColor_colorUniformLoc, backgroundColor);
			gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, backgroundOpacity);
			m_backgroundSprite.draw(p_target, p_states);
			gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, 1.f);
		}
		else {
			gs.win95Shader.setVec2Uniform(gs.win95_pixelBoundsUniformLoc, glm::vec2(absoluteBounds.wh.x * p_target.getViewportWidth(), absoluteBounds.wh.y * p_target.getViewportHeight()));
			gs.win95Shader.setFloatUniform(gs.win95_opacityUniformLoc, backgroundOpacity);
			m_backgroundSprite.attachShader(&gs.win95Shader);
			m_backgroundSprite.draw(p_target, p_states);
		}
	}
	if (m_imageAttached) {
		if (m_stretchImage) {
			m_imageSprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		}
		else {
			Texture& spriteTex = m_imageSprite.getTexture();
			m_imageSprite.setBounds(Rect(0.f, 0.f, spriteTex.width / p_target.getViewportWidth(), spriteTex.height / p_target.getViewportHeight()));
		}
		switch (m_imageCorner) {
		case Corner::TOP_LEFT:
			m_imageSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
			m_imageSprite.setOriginRelative(OriginLoc::TOP_LEFT);
			break;
		case Corner::TOP_RIGHT:
			m_imageSprite.setPosition(glm::vec3(absoluteBounds.xy.x + absoluteBounds.wh.x, absoluteBounds.xy.y, 1.f));
			m_imageSprite.setOriginRelative(OriginLoc::TOP_RIGHT);
			break;
		case Corner::BOTTOM_LEFT:
			m_imageSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y + absoluteBounds.wh.y, 1.f));
			m_imageSprite.setOriginRelative(OriginLoc::BOTTOM_LEFT);
			break;
		case Corner::BOTTOM_RIGHT:
			m_imageSprite.setPosition(glm::vec3(absoluteBounds.xy.x + absoluteBounds.wh.x, absoluteBounds.xy.y + absoluteBounds.wh.y, 1.f));
			m_imageSprite.setOriginRelative(OriginLoc::BOTTOM_RIGHT);
			break;
		}

		m_imageSprite.draw(p_target, p_states);
	}
	Widget::draw(p_target, p_states);
}

bool GUIContainer::onUpdate(GUIEvent e)
{
	return Widget::onUpdate(e);
}

void GUIContainer::setImage(Texture& p_image, bool stretchToFit)
{
	m_imageAttached = true;
	m_imageSprite.attachTexture(p_image);
	m_imageSprite.attachShader(&gs.imageShader);
	if (stretchToFit) {
		m_imageSprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		m_stretchImage = true;
	}
	else {
		m_stretchImage = false;
	}
	m_imageSprite.setScale(glm::vec2(1.f, -1.f));
	m_imageSprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));

}

void GUIContainer::setImageJustification(Corner p_corner)
{
	m_imageCorner = p_corner;
}

void GUIContainer::enableBackground()
{
	m_backgroundEnabled = true;
}

void GUIContainer::disableBackground()
{
	m_backgroundEnabled = false;
}

void GUIContainer::useWin95Background()
{
	m_backgroundEnabled = true;
	m_win95Bg = true;
}
