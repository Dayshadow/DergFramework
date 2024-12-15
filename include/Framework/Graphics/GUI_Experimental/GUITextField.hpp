#pragma once
#include "GUIWidget.hpp"
#include "glm/vec3.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include <string>
#include <string_view>
#include "Framework/Graphics/Text.hpp"
#include "Framework/Graphics/Sprite.hpp"
#include "DefaultFonts.hpp"

// todo:: implement word wrap
class GUITextField : public Widget {
public:
	GUITextField(std::string_view p_ID) : Widget(p_ID) {
		m_backgroundSprite.attachShader(&GenericShaders::Get().solidColorShader);
	};

	void draw(DrawSurface& p_target, DrawStates& p_states);
	void setText(std::string_view p_text);
	void appendText(std::string_view p_text);
	void setTextHeight(float p_height);
	void disableBackground();
	void enableBackground();
	void disableRelativeScaling();
	void enableRelativeScaling();
	float getPixelLineHeight(float viewportHeight);

	glm::vec3 backgroundColor{ 0.2f };
	glm::vec3 textColor{ 1.0f };
	float backgroundOpacity = 1.f;
	bool centered = true;
	bool autoScreenWidth = false;
	bool autoScreenHeight = false;
protected:
	Text m_fieldText{ DefaultFonts.videotype, "" };
	GenericShaders& gs = GenericShaders::Get();
	std::string m_textString;
	Sprite m_backgroundSprite;
	float m_textHeight = 50; // if not using relative scaling, it is pixel height. otherwise, 0-1.
	bool m_useRelativeScaling = false; // false is pixel-based text height, true is local coord text height
	bool m_backgroundEnabled = false;
};