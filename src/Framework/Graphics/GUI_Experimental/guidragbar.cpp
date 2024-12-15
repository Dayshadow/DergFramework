#include "GUIDragBar.hpp"

void GUIDragBar::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_usingScreenBounds) {
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
	Widget::draw(p_target, p_states);
}

bool GUIDragBar::onUpdate(GUIEvent e)
{
	bool consumed = false;
	if (dragging) {
		// potential bug, it does not care about which mouse button was released
		if (!e.mouse.wasRelease) {
			if (m_parent->isUsingScreenBounds()) {
				m_parent->setScreenBounds(Rect(e.mouse.pixelX + initOffset.x, e.mouse.pixelY + initOffset.y, m_parent->screenBounds.wh.x, m_parent->screenBounds.wh.y));
			}
			else if (m_parent->isAbsolute()) {

				m_parent->setAbsoluteBounds(Rect(e.mouse.x + initOffset.x, e.mouse.y + initOffset.y, m_parent->absoluteBounds.wh.x, m_parent->absoluteBounds.wh.y));
			}
			else {
				glm::vec2 localPos = m_parent->toLocal(e.mouse.x + initOffset.x, e.mouse.y + initOffset.y);
				m_parent->setLocalBounds(Rect(localPos.x, localPos.y, m_parent->localBounds.wh.x, m_parent->localBounds.wh.y));
			}
			consumed = true;
		}
		else {
			dragging = false;
		}
	}
	if (absoluteBounds.contains(e.mouse.x, e.mouse.y)) {
		if (e.mouse.wasClick) {
			if (e.mouse.mouseButton == SDL_BUTTON_LEFT) {
				dragging = true;
				consumed = true;
				if (m_parent->isUsingScreenBounds()) {
					initOffset = m_parent->screenBounds.xy - glm::vec2(e.mouse.pixelX, e.mouse.pixelY);
				}
				else {
					initOffset = m_parent->absoluteBounds.xy - glm::vec2(e.mouse.x, e.mouse.y);
				}
			}
		}
	}
	return consumed || Widget::onUpdate(e);
}

void GUIDragBar::disableBackground()
{
	m_backgroundEnabled = false;
}

void GUIDragBar::enableBackground()
{
	m_backgroundEnabled = true;
}
