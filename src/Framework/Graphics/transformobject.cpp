#include "Framework/Graphics/TransformObject.hpp"
#include <cmath>
#include <util/ext/glm/gtc/matrix_transform.hpp>


const float fwrapUnsigned(float x, float r) {
	float aR = std::fabs(r); // The wrapping point should always be positive or else weird things happen
	// Piecewise fmod transformation function
	return x < 0 ? (std::fmodf(x, aR) + r) : std::fmodf(x, aR);
}
// wraps float values to the range [-r, r] 
const float fwrapSigned(float x, float r) {
	// Just apply a transform to the unsigned function
	return 2.f * fwrapUnsigned(0.5f * (x + r), r) - r;
}

TransformObject::TransformObject() :
	m_origin(glm::vec2(0.f, 0.f)),
	m_position(glm::vec3(0.f, 0.f, 0.f)),
	apparentPos(glm::vec3(0.f)),
	m_rotation(0.f),
	m_rotationAxis(glm::vec3(0.f, 0.f, 1.f)),
	m_scale(glm::vec2(1.f, 1.f)),
	m_transform(glm::mat4(1.f))
{}
void TransformObject::setOrigin(glm::vec2 p_origin)
{
	m_origin = p_origin;
	m_transformOutOfDate = true;
}
void TransformObject::setOrigin(Rect p_bounds, OriginLoc p_origin)
{
	switch (p_origin) {
	case OriginLoc::TOP_LEFT:
		m_origin = p_bounds.getTL();
		break;
	case OriginLoc::TOP_RIGHT:
		m_origin = p_bounds.getTR();
		break;
	case OriginLoc::BOTTOM_LEFT:
		m_origin = p_bounds.getBL();
		break;
	case OriginLoc::BOTTOM_RIGHT:
		m_origin = p_bounds.getBR();
		break;
	case OriginLoc::CENTER:
		m_origin = p_bounds.getCenter();
		break;
	default:
		m_origin = p_bounds.getTL();
	}
	m_transformOutOfDate = true;
}
void TransformObject::setPosition(glm::vec3 p_position)
{
	m_position = p_position;
	m_transformOutOfDate = true;
}
void TransformObject::translate(glm::vec3 p_position)
{
	m_position += p_position;
	m_transformOutOfDate = true;
}
void TransformObject::setRotation(float radians)
{
	m_rotation = fwrapUnsigned(radians, 3.141592653589f * 2.f);
	m_transformOutOfDate = true;
}
void TransformObject::setRotationAxis(glm::vec3 p_axis)
{
	m_rotationAxis = glm::vec3(p_axis);
}
void TransformObject::setScale(glm::vec2 p_scaleFactors)
{
	m_scale = p_scaleFactors;
	m_transformOutOfDate = true;
}
const glm::vec2 TransformObject::getOrigin()
{
	return m_origin;
}
const glm::vec3 TransformObject::getPosition()
{
	return m_position;
}
const float TransformObject::getRotation()
{
	return m_rotation;
}
const glm::vec2 TransformObject::getScale()
{
	return m_scale;
}
void TransformObject::calculateTransform()
{
	if (m_transformOutOfDate) {
		// Matrix multiplication is non-commutative, so it needs to be done in TRS order in this case. It's always the reverse order than you expect it to be.

		// Translate to world space.
		if (!m_useInterpolation) {
			m_transform = glm::translate(glm::mat4(1.0f), m_position);
		}
		else {
			m_transform = glm::translate(glm::mat4(1.0f), apparentPos);
		}

		// Rotate along axis
		if (!m_useInterpolation) {
			m_transform = glm::rotate(m_transform, m_rotation, m_rotationAxis);
		}
		else {
			m_transform = glm::rotate(m_transform, apparentRot, m_rotationAxis);
		}
		// Do the scaling.
		m_transform = glm::scale(m_transform, glm::vec3(m_scale, 1.f));
		// Transform to the origin, *technically* done first
		m_transform = glm::translate(m_transform, glm::vec3(-m_origin, 0.f));

		m_transformOutOfDate = false;
	}
}

glm::mat4 TransformObject::getObjectTransform()
{
	calculateTransform();
	return m_transform;
}

void TransformObject::cloneTransform(const TransformObject& p_other)
{
	m_origin = p_other.m_origin;
	m_position = p_other.m_position;
	m_rotation = p_other.m_rotation;
	m_rotationAxis = p_other.m_rotationAxis;
	m_scale = p_other.m_scale;
	m_transform = p_other.m_transform;
}

void TransformObject::enableTransformInterpolation()
{
	m_useInterpolation = true;
}

void TransformObject::disableTransformInterpolation()
{
	m_useInterpolation = false;
}
