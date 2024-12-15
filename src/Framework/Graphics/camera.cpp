#include "Framework/Graphics/Camera.hpp"
#include <iostream>
#include "util/utils.hpp"

Camera::Camera():
	pos(0.0f, 0.0f, 1.0f), // starts 1 off the z axis by default
	m_target(0.0f, 0.0f, 0.0f), // temporarily set the origin as the target to calculate the correct right and up
	tileScale(20.f)
{
	glm::vec3 direction = glm::normalize(pos - m_target);
	Camera::m_right = glm::normalize(glm::cross(m_upGuide, direction));
	Camera::m_up = glm::cross(direction, m_right);
	Camera::m_forward = glm::cross(m_right, m_up); // forward calculate normalized forward facing vector
	lookForwards();
	updateFrame();

}

Camera::Camera(glm::vec3 p_pos) :

	pos(p_pos),
	m_target(0.f, 0.f, 0.f), // temporarily set the origin as the target to calculate the correct right and up
	tileScale(20.f)
{
	glm::vec3 direction = glm::normalize(pos - m_target);
	Camera::m_right = glm::normalize(glm::cross(m_upGuide, direction));
	Camera::m_up = glm::cross(direction, m_right);
	Camera::m_forward = glm::cross(m_right, m_up); // forward calculate normalized forward facing vector
	apparentPos = p_pos;
	lookForwards();
	updateFrame();
};

void Camera::lookAt(glm::vec3 p_target)
{
	glm::vec3 cur = pos;
	if (m_interpolation) cur = apparentPos;
	Camera::view = glm::lookAt(cur, p_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::lookForwards()
{
	glm::vec3 cur = pos;
	if (m_interpolation) cur = apparentPos;
	Camera::m_target = cur - m_forward; // look forwards
	Camera::view = glm::lookAt(cur, m_target, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 translate1 = glm::translate(glm::mat4(1.f), glm::vec3(cur.x + (m_frame.z - m_frame.x) / 2.f, cur.y + (m_frame.w - m_frame.y) / 2.f, 0.f));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.f), -zRotation, m_forward);
	glm::mat4 translate2 = glm::translate(glm::mat4(1.f), -glm::vec3(cur.x + (m_frame.z - m_frame.x) / 2.f, cur.y + (m_frame.w - m_frame.y) / 2.f, 0.f));
	view *= translate1;
	view *= rotate;
	view *= translate2;

}

void Camera::setDimensions(uint32_t p_windowWidth, uint32_t p_windowHeight)
{
	if (m_pixelDimensions.x == (float)p_windowHeight && m_pixelDimensions.y == (float)p_windowWidth) return;
	Camera::m_dimensions.x = 1.0f;
	Camera::m_dimensions.y = float(p_windowHeight) / float(p_windowWidth);
	m_pixelDimensions.x = (float)p_windowWidth;
	m_pixelDimensions.y = (float)p_windowHeight;
	updateFrame();
}

glm::mat4 Camera::getTransform()
{
	float screenScaling = 1.0;
	if (!m_disableAutoFrame) {
		if (m_pixelDimensions.x < m_pixelDimensions.y)
		{
			screenScaling = m_pixelDimensions.x / m_pixelDimensions.y;
		}
		else
		{
			screenScaling = 1.0f;
		}
		if (!m_perspective)
		{
			m_proj = glm::ortho(
				0.0f,
				m_dimensions.x * tileScale * screenScaling,
				0.0f,
				m_dimensions.y * tileScale * screenScaling,
				0.1f, 1000.0f);
		}
		else
		{
			m_proj = glm::perspective(glm::radians(45.0f), m_pixelDimensions.x / m_pixelDimensions.y, 0.1f, 1000.0f);
		}
		if (!m_manualView)
		{
			lookForwards(); // default behavior, can be overriden by other view setting functions
		}

		glm::vec2 screenCenterPos = glm::vec2((m_dimensions.x * screenScaling) / 2.0f, (m_dimensions.y * screenScaling) / 2.0f) * tileScale;
		setFrame(
			pos.x - screenCenterPos.x,
			pos.y - screenCenterPos.y,
			screenCenterPos.x * 2.0f,
			screenCenterPos.y * 2.0f);
		view = glm::translate(view, glm::vec3(screenCenterPos, 0.0f));

	}
	else {
		if (!m_perspective)
		{
			setGlobalPos(m_frame.x, m_frame.y);
			m_proj = glm::ortho(
				0.0f,
				getFrameDimensions().x,
				0.0f,
				-getFrameDimensions().y,
				0.1f, 1000.0f);
		}
		else
		{
			// why would you use perspective in this mode
			m_proj = glm::perspective(glm::radians(45.0f), m_pixelDimensions.x / m_pixelDimensions.y, 0.1f, 1000.0f);
		}
		if (!m_manualView)
		{
			lookForwards(); // default behavior, can be overriden by other view setting functions
		}
	}
	return m_proj * view;
}

void Camera::setFrame(float p_trX, float p_trY, float p_width, float p_height)
{
	Camera::m_frame = glm::vec4(p_trX, p_trY, p_trX + p_width, p_trY + p_height);
}

const glm::vec2 Camera::getFrameDimensions() const
{
	return glm::vec2(m_frame.z - m_frame.x, m_frame.y - m_frame.w);
}

void Camera::updateFrame()
{
	float screenScaling = 1.0;
	if (m_pixelDimensions.x < m_pixelDimensions.y)
	{
		screenScaling = m_pixelDimensions.x / m_pixelDimensions.y;
	}
	else
	{
		screenScaling = 1.0f;
	}
	glm::vec2 screenCenterPos = glm::vec2((m_dimensions.x * screenScaling) / 2.0f, (m_dimensions.y * screenScaling) / 2.0f) * tileScale;
	setFrame(
		pos.x - screenCenterPos.x,
		pos.y - screenCenterPos.y,
		screenCenterPos.x * 2.0f,
		screenCenterPos.y * 2.0f);
}

void Camera::setTileScale(float p_tileScale)
{
	Camera::tileScale = p_tileScale;
}

void Camera::setGlobalPos(glm::vec2 p_globalPos)
{ // global pos is in the unit of tiles
	pos = glm::vec3(glm::vec2(p_globalPos.x, p_globalPos.y), pos.z);
}

void Camera::setGlobalPos(float p_globalX, float p_globalY)
{ // global pos is in the unit of tiles
	pos = glm::vec3(p_globalX, p_globalY, pos.z);
}
void Camera::setApparentPos(glm::vec2 p_globalPos) {
	apparentPos = glm::vec3(p_globalPos.x, p_globalPos.y, pos.z);
}
void Camera::setApparentPos(float p_globalX, float p_globalY)
{ // global pos is in the unit of tiles
	apparentPos = glm::vec3(p_globalX, p_globalY, pos.z);
}

void Camera::interpolate(float rate)
{
	apparentPos = utils::lerp(apparentPos, pos, rate);
}

//void Camera::interpolate(float p_alpha, float dt) {
//	// simple lerp
//	glm::vec3 newPos = pos + lastVelocity * p_alpha * dt;
//	setApparentPos(newPos.x, newPos.y);
//}
glm::vec2 Camera::pixelToTileCoordinates(float p_pixelX, float p_pixelY)
{
	getTransform();
	// normalized screen coordinates
	glm::vec4 out = glm::vec4((p_pixelX / m_pixelDimensions.x) * 2.f - 1.f, -(p_pixelY / m_pixelDimensions.y) * 2.f + 1.f, 0.f, 1.f);
	glm::mat4 transform = glm::inverse(view) * glm::inverse(m_proj);

	out = transform * out;
	return glm::vec2(out.x, out.y);
}

glm::vec2 Camera::tileToPixelCoordinates(float p_tileX, float p_tileY)
{
	getTransform();
	glm::mat4 scaling = glm::scale(glm::mat4(1.f), glm::vec3(m_pixelDimensions.x / (m_frame.z - m_frame.x), m_pixelDimensions.y / (m_frame.w - m_frame.y), 0.f));
	glm::mat4 transform = scaling * view;
	glm::vec4 out = transform * glm::vec4(p_tileX, p_tileY, 0.f, 1.f);
	return glm::vec2(out.x, out.y);

}
const glm::vec4 Camera::getFrame() const {
	return m_frame;
}