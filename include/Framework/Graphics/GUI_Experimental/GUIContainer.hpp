#pragma once
#include "GUIWidget.hpp"
#include "util/Rect.hpp"
#include <string>
#include <string_view>
#include <functional>
#include "Framework/Graphics/Sprite.hpp"
#include "DefaultFonts.hpp"

class GUIContainer : public Widget {
public:
	GUIContainer(std::string_view p_ID);

	void draw(DrawSurface& p_target, DrawStates& p_states);
	bool onUpdate(GUIEvent e) override;
	void setImage(Texture& p_image, bool stretchToFit = false);
	void setImageJustification(Corner p_justification);
	void enableBackground();
	void disableBackground();
	void useWin95Background();

	glm::vec3 backgroundColor{ 0.2f };
	float backgroundOpacity = 1.f;
protected:
	bool m_backgroundEnabled = false;
	bool m_imageAttached = false;
	bool m_stretchImage = false;
	bool m_win95Bg = false;
	Corner m_imageCorner;
	GenericShaders& gs = GenericShaders::Get();
	Sprite m_backgroundSprite{ glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 0.f), Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y) };
	Sprite m_imageSprite{ glm::vec3(absoluteBounds.xy.x, absoluteBounds.xy.y, 0.f), Rect(0.f, 0.f, absoluteBounds.wh.x, absoluteBounds.wh.y) };
};