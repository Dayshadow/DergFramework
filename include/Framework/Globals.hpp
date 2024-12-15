#pragma once
#include <stdint.h>

struct KeyEvent {
	bool wasDown;
	int keyCode;
	bool valid = false; // for compatability for things that expect a conditional keyevent
};

enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

struct MouseEvent {
	float x = 0.f;
	float y = 0.f;
	// specifically for gui
	float pixelX = 0.f;
	float pixelY = 0.f;
	bool wasClick = false;
	bool wasRelease = false;
	bool wasMove = false;
	bool wasScroll = false;
	uint8_t mouseButton = 0;
	uint32_t mouseState = 0;
	
};

