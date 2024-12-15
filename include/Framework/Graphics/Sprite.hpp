#ifndef SPRITE_H
#define SPRITE_H
#include "Framework/Graphics/TransformObject.hpp"
#include "Framework/Graphics/DrawSurface.hpp"
#include "Framework/Graphics/DrawStates.hpp"
#include "Framework/Graphics/Mesh.hpp"
#include "Framework/Graphics/Shader.hpp"
#include "Framework/Graphics/Texture.hpp"
#include "util/Rect.hpp"


//class DrawSurface;
// A sprite containing its own transform and mesh.
class Sprite : public TransformObject {
public:
	Sprite();
	Sprite(glm::vec3 p_position, Rect p_bounds);

	~Sprite() {
		DELETE_LOG("Sprite Destroyed");
	}

	// Shader must have the appropriate vertex attribute format
	void attachShader(Shader* p_shader);
	void attachTexture(Texture p_texture);
	Texture& getTexture();
	void draw(DrawSurface& p_target, const DrawStates& p_drawStates);

	// We don't want to override the setorigin class of the inherited class, so we name it something different
	void setOriginRelative(OriginLoc p_origin);
	void setBounds(Rect p_bounds);

	// only works with the basic image shader, unless uniform 2 is bound to opacity.
	void setOpacity(float p_opacity);

	Rect bounds;
private:
	void initForDraw(); // manual init in cases where gpu stuff can't be done right off the bat
	float opacity = 0.f;

	Mesh<GLfloat> m_spriteMesh{NO_VAO_INIT};
	// Optional attachments directly to the sprite, so you don't have to worry about setting your drawStates properly before draw().
	Shader* m_attachedShader;
	Texture m_attachedTexture;
	bool m_drawReady = false;
};

#endif