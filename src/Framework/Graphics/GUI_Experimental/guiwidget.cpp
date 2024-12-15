#include "GUIWidget.hpp"

Widget::Widget()
{
}

Widget::Widget(std::string_view p_ID) :
	m_ID(p_ID)
{
}

void Widget::setID(std::string_view p_newID)
{
	m_ID = p_newID;
}

void Widget::draw(DrawSurface& p_target, DrawStates& p_states)
{
	if (m_usingPixelWidth) {
		absoluteBounds.wh.x = screenBounds.wh.x / p_target.getViewportWidth();
	}
	if (m_usingPixelHeight) {
		absoluteBounds.wh.y = screenBounds.wh.y / p_target.getViewportHeight();
	}
	if (m_usingPixelOffset) {
		absoluteBounds.xy = glm::vec2(m_parent->absoluteBounds.xy.x + screenBounds.xy.x / p_target.getViewportWidth(), m_parent->absoluteBounds.xy.y + screenBounds.xy.y / p_target.getViewportHeight());
		absOffset = glm::vec2(screenBounds.xy.x / p_target.getViewportWidth(), screenBounds.xy.y / p_target.getViewportHeight());
	}
	for (auto child : m_children) {
		child->draw(p_target, p_states);
	}
}

bool Widget::onUpdate(GUIEvent e)
{
	bool consumed = false;
	for (Widget* w : m_children) {
		consumed |= w->onUpdate(e);
	}
	return consumed;
}

void Widget::addChild(Widget* p_child)
{
	m_children.push_back(p_child);
	p_child->m_parent = this;
	if (!p_child->m_absolute)
		p_child->absoluteBounds = p_child->queryAbsoluteRect(p_child->localBounds);
}

Rect Widget::queryAbsoluteRect(Rect p_childRect)
{

	if (m_parent == nullptr) return p_childRect;
	float newX, newY, newW, newH;
	if (m_parent->isAbsolute()) {
		newX = m_parent->absoluteBounds.xy.x + m_parent->absoluteBounds.wh.x * p_childRect.xy.x;
		newY = m_parent->absoluteBounds.xy.y + m_parent->absoluteBounds.wh.y * p_childRect.xy.y;
		newW = m_parent->absoluteBounds.wh.x * p_childRect.wh.x;
		newH = m_parent->absoluteBounds.wh.y * p_childRect.wh.y;
		return Rect(newX, newY, newW, newH);
	}
	// untested coordinate escalation
	newX = m_parent->localBounds.xy.x + m_parent->localBounds.wh.x * p_childRect.xy.x;
	newY = m_parent->localBounds.xy.y + m_parent->localBounds.wh.y * p_childRect.xy.y;
	newW = m_parent->localBounds.wh.x * p_childRect.wh.x;
	newH = m_parent->localBounds.wh.y * p_childRect.wh.y;
	return m_parent->queryAbsoluteRect(Rect(newX, newY, newW, newH));
}

void Widget::setLocalBounds(Rect p_localBounds)
{
	m_usingPixelOffset = false;
	m_absolute = false;
	m_usingScreenBounds = false;
	localBounds = p_localBounds;
	absoluteBounds = queryAbsoluteRect(localBounds);
	m_usingPixelHeight = false;
	m_usingPixelWidth = false;
	updateChildBounds();
}

glm::vec2 Widget::toLocal(float x, float y)
{
	return glm::vec2((x - m_parent->absoluteBounds.xy.x) / m_parent->absoluteBounds.wh.x, (y - m_parent->absoluteBounds.xy.y) / m_parent->absoluteBounds.wh.y);
}

void Widget::setAbsoluteBounds(Rect p_absoluteBounds)
{
	m_usingPixelOffset = false;
	m_absolute = true;
	absoluteBounds = p_absoluteBounds;
	localBounds = Rect(0.f, 0.f, 1.f, 1.f);
	m_usingPixelHeight = false;
	m_usingPixelWidth = false;
	updateChildBounds();
}

void Widget::setScreenBounds(Rect p_screenBounds)
{
	m_usingPixelOffset = false;
	m_absolute = true;
	m_usingScreenBounds = true;
	// unneccesary specification now
	m_usingPixelHeight = false;
	m_usingPixelWidth = false;
	screenBounds = p_screenBounds;
}

void Widget::setPixelWidth(float p_pixelWidth)
{
	m_usingPixelWidth = true;
	screenBounds.wh.x = p_pixelWidth;
}

void Widget::setPixelHeight(float p_pixelHeight)
{
	m_usingPixelHeight = true;
	screenBounds.wh.y = p_pixelHeight;
}

void Widget::setPixelOffset(float p_pixelX, float p_pixelY)
{
	m_usingPixelOffset = true;
	screenBounds.xy.x = p_pixelX;
	screenBounds.xy.y = p_pixelY;
}

void Widget::updateScreenBounds(float p_windowWidth, float p_windowHeight)
{

	float absoluteX = screenBounds.xy.x / p_windowWidth;
	float absoluteY = screenBounds.xy.y / p_windowHeight;
	float absoluteW = screenBounds.wh.x / p_windowWidth;
	float absoluteH = screenBounds.wh.y / p_windowHeight;
	Rect newRect = Rect(absoluteX, absoluteY, absoluteW, absoluteH);
	if (newRect.xy == absoluteBounds.xy && newRect.wh == absoluteBounds.wh) return;
	absoluteBounds = newRect;
	updateChildBounds();
}

void Widget::updateChildBounds()
{
	for (auto child : m_children) {
		// only do so for relatively positioned children
		if (child->isUsingScreenBounds() || child->isAbsolute()) continue;
		auto absBounds = child->queryAbsoluteRect(child->localBounds);
		if (child->m_usingPixelOffset) {
			absBounds.xy += child->absOffset;
		}
		child->absoluteBounds.xy = absBounds.xy;
		if (!child->m_usingPixelWidth) child->absoluteBounds.wh.x = absBounds.wh.x;
		if (!child->m_usingPixelHeight) child->absoluteBounds.wh.y = absBounds.wh.y;
		child->updateChildBounds();
	}
}

Widget* Widget::findChild(std::string_view p_childID)
{
	for (auto child : m_children) {
		if (child->m_ID == p_childID) {
			return child;
		}
	}
	return nullptr;
}

Widget* Widget::findChildRecursive(std::string_view p_childID)
{
	for (auto child : m_children) {
		if (child->m_ID == p_childID) {
			return child;
		}
	}
	// if surface level search was unsuccessful
	// kinda an n^2 search though, but the tree is sparse so it's ok
	for (auto child : m_children) {
		Widget* result = child->findChild(p_childID);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

bool Widget::removeChild(std::string_view p_childID)
{
	Widget* childPtr = findChild(p_childID);
	if (!childPtr) return false;
	auto itr = std::find(m_children.begin(), m_children.end(), childPtr);
	m_children.erase(itr);
	return true;
}

bool Widget::removeChildRecursive(std::string_view p_childID)
{
	if (removeChild(p_childID)) return true;
	for (auto child : m_children) {
		if (removeChildRecursive(p_childID)) return true;
	}
	return false;
}
