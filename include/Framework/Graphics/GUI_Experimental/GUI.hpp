#pragma once
#include "Framework/Window/GameWindow.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include <Framework/Graphics/GUI_Experimental/GUIWidget.hpp>
#include <unordered_map>
#include <string>
#include <string_view>
#include <mutex>

class GUI {
public:
	void draw(DrawSurface& p_target);
	bool update(GUIEvent e);

	void addElement(Widget* p_elmBase);
	bool removeElement(std::string_view p_id);

	static GUI& Get();
private:
	GUI();
	std::mutex m_elmLock;
	// it needs to know the window for certain things like aspect ratio and pixel dimensions, future use.
	GameWindow* m_window = nullptr;
	DrawStates GUIDrawStates;
	Widget m_root{ "root" };
};