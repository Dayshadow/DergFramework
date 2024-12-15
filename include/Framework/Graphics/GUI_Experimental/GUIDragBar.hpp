#pragma once
#include "GUIWidget.hpp"
#include "glm/vec3.hpp"
#include "Framework/Graphics/GenericShaders.hpp"
#include "Framework/Graphics/Sprite.hpp"

// todo:: implement word wrap
class GUIDragBar : public Widget {
public:
	GUIDragBar(std::string_view p_ID) : Widget(p_ID) {
		m_backgroundSprite.attachShader(&GenericShaders::Get().solidColorShader);
	};

	void draw(DrawSurface& p_target, DrawStates& p_states);
	bool onUpdate(GUIEvent e) override;

	void disableBackground();
	void enableBackground();

	glm::vec3 backgroundColor{ 0.2f };
	float backgroundOpacity = 1.f;
protected:
	GenericShaders& gs = GenericShaders::Get();
	Sprite m_backgroundSprite;
	// offsets for keeping track of where it was first clicked relative to origin
	glm::vec2 initOffset{ 0.f };
	
	bool dragging = false;
	bool m_backgroundEnabled = false;
};