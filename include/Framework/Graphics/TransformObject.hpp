#ifndef TRANSFORMOBJECT_H
#define TRANSFORMOBJECT_H
#include <util/ext/glm/vec2.hpp>
#include <util/ext/glm/vec3.hpp>
#include <util/ext/glm/vec4.hpp>
#include <util/ext/glm/mat4x4.hpp>
#include <util/Rect.hpp>
enum class OriginLoc {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	CENTER
};
// A flat 2d object with z layering.
class TransformObject {
public:
	TransformObject();

	TransformObject& operator=(const TransformObject& other) {
		cloneTransform(other);
		return *this;
	}
	// Sets the origin to be used. Affects rotation, position, etc
	void setOrigin(glm::vec2 p_origin);
	void setOrigin(Rect p_bounds, OriginLoc p_origin);


	// 3d position
	// Overwrites old position
	void setPosition(glm::vec3 p_position);
	// Moves a specified amount
	void translate(glm::vec3 p_position);

	// 2d Rotation along the z axis
	// Overwrites old rotation
	void setRotation(float radians);

	void setRotationAxis(glm::vec3 p_axis);
	// xy scale
	void setScale(glm::vec2 p_scaleFactors);

	const glm::vec2 getOrigin();

	const glm::vec3 getPosition();

	const float getRotation();

	const glm::vec2 getScale();

	void calculateTransform();

	glm::mat4 getObjectTransform();

	void cloneTransform(const TransformObject& p_other);
	void cloneTransform(const glm::mat4& p_other);

	void enableTransformInterpolation();
	void disableTransformInterpolation();

	glm::vec3 apparentPos;
	float apparentRot;
protected:
	glm::vec2 m_origin;
	glm::vec3 m_position; // Decided to give it a z axis, as this is handy for certain effects and layering.
	float m_rotation; // Rotation does not affect the z axis. I can make a 3d transform object if needed later. Strictly radians.
	glm::vec3 m_rotationAxis;
	glm::vec2 m_scale; // xy
	glm::mat4 m_transform;
	bool m_transformOutOfDate = true;
	bool m_useInterpolation = false;
};

#endif