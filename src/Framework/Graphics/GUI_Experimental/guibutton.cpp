#include "GUIButton.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "util/ext/glm/gtc/matrix_transform.hpp"
GUIButton::GUIButton(std::string_view p_ID, float p_x, float p_y, float p_w, float p_h)
{
	localBounds = Rect(p_x, p_y, p_w, p_h);
	absoluteBounds = queryAbsoluteRect(localBounds);
	m_sprite.attachShader(&GenericShaders::Get().solidColorShader);
	onClickFunc = [](void) {};
	onHoverFunc = [](bool) {};
	m_ID = p_ID;
}

void GUIButton::onClick(std::function<void()> p_callback)
{
	onClickFunc = p_callback;
}

void GUIButton::onHover(std::function<void(bool)> p_callback)
{
	onHoverFunc = p_callback;
}

void GUIButton::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_backgroundEnabled) {
		m_sprite.setBounds(Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y));
		m_sprite.setPosition(glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 1.f));
		gs.solidColorShader.setVec3Uniform(gs.solidColor_colorUniformLoc, testColor);
		gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, 0.5f);
	}
	//GenericShaders::Get().fancyShader.setFloatUniform(1, SDL_GetTicks() / 1000.f);

	m_sprite.draw(p_target, p_states);
	gs.solidColorShader.setFloatUniform(gs.solidColor_opacityUniformLoc, 1.f);
	Widget::draw(p_target, p_states);
}

bool GUIButton::onUpdate(GUIEvent e)
{
	bool consumed = false;
	// ignore click if child already ate it
	if (Widget::onUpdate(e)) return true;

	if (!disabled) {
		if (absoluteBounds.contains(e.mouse.x, e.mouse.y)) {
			onHoverFunc(true);
		}
		else {
			onHoverFunc(false);
		}
		if (absoluteBounds.contains(e.mouse.x, e.mouse.y)) {

			if (e.mouse.wasClick && e.mouse.mouseButton == SDL_BUTTON_LEFT) {
				onClickFunc();
			}
			consumed = true;
		}
	}
	return consumed;
}
