#pragma once
#include "GUIWidget.hpp"
#include "util/Rect.hpp"
#include <string>
#include <string_view>
#include <functional>
#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/Text.hpp"
#include "DefaultFonts.hpp"
class GUIButton : public Widget {
public:
	GUIButton(std::string_view p_ID, float p_x, float p_y, float p_w, float p_h);
	void onClick(std::function<void()> p_callback);
	void onHover(std::function<void(bool)> p_callback);

	void enableBackground() { m_backgroundEnabled = true; }
	void disableBackground() { m_backgroundEnabled = false; }
	void draw(DrawSurface& p_target, DrawStates& p_states);
	bool onUpdate(GUIEvent e) override;

	glm::vec3 testColor{ 0.2f };
	bool disabled = false;
protected:
	GenericShaders& gs = GenericShaders::Get();
	bool m_backgroundEnabled = false;
	std::function<void()> onClickFunc;
	std::function<void(bool)> onHoverFunc;
	Sprite m_sprite{ glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 0.f), Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y) };
};