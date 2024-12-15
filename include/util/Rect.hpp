#pragma once
#include <util/ext/glm/vec2.hpp>

// a very simple rectangle class that just makes coordinate calculations less convoluted.
// WARNING! Y axis increases as you move up, so bottom left is at 0, 0
struct Rect {
	Rect(): xy(glm::vec2(0.f)), wh(glm::vec2(0.f)) {};
	Rect(float p_x, float p_y, float p_w, float p_h) {
		xy = glm::vec2(p_x, p_y);
		wh = glm::vec2(p_w, p_h);
	}
	Rect(glm::vec2 p_xy, glm::vec2 p_wh) {
		xy = p_xy;
		wh = p_wh;
	}

	bool operator==(const Rect& rect2) {
		return (xy == rect2.xy && wh == rect2.wh);
	}

	// Gets the top left coordinate of the rectangle as a vec2.
	glm::vec2 getBL() const {
		return xy;
	}
	// Gets the top right coordinate of the rectangle as a vec2.
	glm::vec2 getBR() const {
		return glm::vec2(xy.x + wh.x, xy.y);
	}
	// Gets the bottom right coordinate of the rectangle as a vec2.
	glm::vec2 getTL() const {
		return glm::vec2(xy.x, xy.y + wh.y);
	}
	// Gets the bottom right coordinate of the rectangle as a vec2.
	glm::vec2 getTR() const {
		return glm::vec2(xy.x + wh.x, xy.y + wh.y);
	}
	// Gets the midpoint of the box.
	glm::vec2 getCenter() const {
		return glm::vec2(xy.x + wh.x / 2.f, xy.y + wh.y / 2.f);
	}
	// Defined internally with x, y, width, and height.
	float getArea() const {
		return (wh.x * wh.y);
	}
	float getAspect() {
		return wh.x / wh.y;
	}
	bool contains(float x, float y) {
		return (xy.x < x) && (xy.y < y) && (x < xy.x + wh.x) && (y < xy.y + wh.y);
	}
	glm::vec2 xy;
	glm::vec2 wh;
};
