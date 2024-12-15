#include "Framework/Window/GameWindow.hpp"

GameWindow::GameWindow() 
	: m_window(NULL),
	width(0),
	height(0)
{
}

GameWindow::GameWindow(const char* p_title, uint32_t p_w, uint32_t p_h)
	:m_window(NULL),
	width(p_w),
	height(p_h)
{

	m_window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (m_window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}


	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode m_mode;
	SDL_GetDisplayMode(displayIndex, 0, &m_mode);

	screenWidth = m_mode.w;
	screenHeight = m_mode.h;

	GameWindow::initGL();
}

void GameWindow::create(const char* p_title, uint32_t p_w, uint32_t p_h, int p_flags)
{
	width = p_w;
	height = p_h;

	m_window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, p_flags);
	if (m_window == NULL) {
		std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
	}
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
	}


	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode m_mode;
	SDL_GetDisplayMode(displayIndex, 0, &m_mode);

	screenWidth = m_mode.w;
	screenHeight = m_mode.h;

	m_glContext = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_glContext); // Attach OpenGL context to the window

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	m_DrawBuffers[0] = GL_BACK_LEFT; // Back of double buffer
	setViewport(0, 0, p_w, p_h);
	//GameWindow::initGL();
}

void GameWindow::initGL() {


	LOAD_LOG("Initializing OpenGL 4.6...");

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Bit depth of 8 bits
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // OpenGL 4.6
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	m_glContext = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_glContext); // Attach OpenGL context to the window
	SDL_GL_SetSwapInterval(1);

	glewExperimental = GL_TRUE;
	auto init_res = glewInit();
	if (init_res != GLEW_OK)
	{
		//std::cout << glewGetErrorString(glewInit()) << std::endl;
		throw std::exception((const char*)glewGetErrorString(glewInit()));
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// init inherited class's variables
	m_DrawBuffers[0] = GL_BACK_LEFT; // Back of double buffer
	setViewport(0, 0, width, height);

}

void GameWindow::cleanUp() {
	DELETE_LOG("GL context deleted and window destroyed.");
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
}
void GameWindow::setVSync(bool p_enabled)
{
	SDL_GL_SetSwapInterval((int)p_enabled);
}
void GameWindow::displayNewFrame()
{
	SDL_GL_SwapWindow(m_window); // Swap the back of the double buffer with the front.
}
void GameWindow::toggleFullscreen() {
	static bool isFullscreen = false;
	isFullscreen = !isFullscreen;

	if (isFullscreen) {
		SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
		SDL_SetWindowSize(m_window, screenWidth, screenHeight);
		width = screenWidth;
		height = screenHeight;
	}
	else {
		SDL_SetWindowFullscreen(m_window, 0);
		SDL_SetWindowSize(m_window, screenWidth / 2, screenHeight / 2);
		width = screenWidth / 2;
		height = screenHeight / 2;
	}
	fullscreenChanged = true;
}

bool GameWindow::hasChangedFullscreenState() {
	// sdl is untrustworthy
	//std::cout << fullscreenChanged << '\n';
	if (fullscreenChanged) {
		fullscreenChanged = false;
		return true;
	}
	return false;
}
int GameWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(m_window);
	SDL_DisplayMode m_mode;
	SDL_GetDisplayMode(displayIndex, 0, &m_mode);
	return m_mode.refresh_rate;
}


void GameWindow::bindToThisThread() {
	if (SDL_GL_MakeCurrent(m_window, m_glContext) != 0) {
		ERROR_LOG(SDL_GetError());
	}
}

void GameWindow::unbindFromThisThread() {
	if (SDL_GL_MakeCurrent(m_window, NULL) != 0) {
		ERROR_LOG(SDL_GetError());
	}
}

uint32_t GameWindow::getWindowID()
{
	return SDL_GetWindowID(m_window);
}
