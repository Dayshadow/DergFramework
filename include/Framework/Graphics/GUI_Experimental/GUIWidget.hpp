#pragma once
#include <vector>
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Globals.hpp"
#include "util/Rect.hpp"
#include <stdbool.h>
class GUI;
struct GUIEvent {
	MouseEvent mouse;
	KeyEvent key;
};
// A single element of a GUI in its most basic form. A widget can hold other widgets, and has a few virual methods.
class Widget {
public:
	Widget();
	Widget(std::string_view p_ID);

	void setID(std::string_view p_newID);

	// default behaviour, recursively draw children, should be called at the end of overridden draw functions.
	virtual void draw(DrawSurface& p_target, DrawStates& p_states);
	// default behaviour, recursively update children, return true to consume click.
	virtual bool onUpdate(GUIEvent e);

	void addChild(Widget* p_child);

	Rect queryAbsoluteRect(Rect p_childRect);

	void setLocalBounds(Rect p_localBounds);
	glm::vec2 toLocal(float absX, float absY);
	void setAbsoluteBounds(Rect p_absoluteBounds);
	// Used to set a widgets position and size in screen pixels, not normalized.
	// these two are a bit confusing, but they are just used whenever you want precise pixel positions for gui.
	void setScreenBounds(Rect p_screenBounds);
	void setPixelWidth(float p_pixelWidth);
	void setPixelHeight(float p_pixelHeight);
	void setPixelOffset(float p_pixelX, float p_pixelY);
	void updateScreenBounds(float p_windowHeight, float p_windowWidth);

	void updateChildBounds();
	Widget* findChild(std::string_view p_childID);
	Widget* findChildRecursive(std::string_view p_childID);
	// only removes surface level children, returns false if not found.
	bool removeChild(std::string_view p_childID);
	bool removeChildRecursive(std::string_view p_childID);

	bool isUsingScreenBounds() { return m_usingScreenBounds; }
	bool isAbsolute() { return m_absolute; }
	Rect localBounds;
	Rect absoluteBounds;
	Rect screenBounds;

	glm::vec2 absOffset; // fixes a bug
protected:
	bool m_absolute = false;
	bool m_usingScreenBounds = false;
	bool m_usingPixelWidth = false;
	bool m_usingPixelHeight = false;
	bool m_usingPixelOffset = false;
	std::string m_ID;
	std::vector<Widget*> m_children;
	Widget* m_parent = nullptr;
};