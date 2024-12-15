#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>

#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Globals.hpp"
#include "util/utils.hpp"
#include "Framework/Globals.hpp"
#include "Framework/Input/InputHandler.hpp"
#include <iostream>
#include <math.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

class GameWindow : public DrawSurface
{
public:
	GameWindow();
	/** Constructor, 
	* Defaulted to 1280x720.
	* @param p_title - The text displayed on the top window bar
	* @param p_w, p_h - Width and height of the window, defaults to 720p
	*/
	GameWindow(const char* p_title, uint32_t p_w = 1280, uint32_t p_h = 720);

	// to fix a threading bug
	void create(const char* p_title, uint32_t p_w, uint32_t p_h, int p_flags);
	/// Attaches GL to SDL window
	void initGL();
	/// Deletes the window when we are done using it.
	void cleanUp();
	/**
	* @returns The refresh rate of the screen the window is on.
	*/
	int getRefreshRate();

	// Used to enable and disable the framerate limit.
	void setVSync(bool p_enabled);
	// Swaps the doublebuffer, and shows the new frame.
	void displayNewFrame();

	void toggleFullscreen();

	bool hasChangedFullscreenState();

	void bindToThisThread();
	void unbindFromThisThread();

	uint32_t getWindowID();

	int width;
	int height;
	uint32_t screenWidth;
	uint32_t screenHeight;

	/// SDL managed OpenGL context. Not used for much outside of the constructor.
	SDL_GLContext m_glContext;

	/// A pointer to the SDL window, used internally in most cases.
	SDL_Window* m_window;
private:
	bool fullscreenChanged = false;

};
#endif