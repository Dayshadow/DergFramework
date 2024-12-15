#include "Framework/Graphics/Sprite.hpp"
#include "Framework/Graphics/GenericShaders.hpp"

Sprite::Sprite()
{
	m_attachedShader = nullptr;
	m_attachedTexture = Texture();
}

Sprite::Sprite(glm::vec3 p_position, Rect p_bounds)
{
	LOAD_LOG("Created Sprite at x: " << p_position.x << " y: " << p_position.y);
	setPosition(p_position);

	bounds = p_bounds;
	// Default origin:
	setOrigin(bounds.getCenter());
	m_attachedShader = nullptr;
	m_attachedTexture = Texture();
}


void Sprite::initForDraw()
{
	// Keep it as dynamic for now, 
	m_spriteMesh.setStreamType(GL_DYNAMIC_DRAW);
	m_spriteMesh.addFloatAttrib(3); // Position
	m_spriteMesh.addFloatAttrib(2); // Texcoord

	// The four corner coordinates of the bounding rectangle. Note that the rectangle is in model space and not world space.
	glm::vec2 tl = bounds.getTL();
	glm::vec2 tr = bounds.getTR();
	glm::vec2 bl = bounds.getBL();
	glm::vec2 br = bounds.getBR();

	m_spriteMesh.pushVertices({
		tl.x, tl.y, 0.0f, 0.0f, 0.0f, // vertex 1
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 2
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 3
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 4
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 5
		br.x, br.y, 0.0f, 1.0f, 1.0f // vertex 6
		});

	auto& gs = GenericShaders::Get();
	// default shader
	if (!m_attachedShader)
		m_attachedShader = &gs.imageShader;

	m_spriteMesh.pushVBOToGPU();
	m_drawReady = true;
}

void Sprite::attachShader(Shader* p_shader)
{

	m_attachedShader = p_shader;
}

void Sprite::attachTexture(Texture p_texture)
{
	m_attachedTexture = p_texture;
}

Texture& Sprite::getTexture()
{
	return m_attachedTexture;
}



void Sprite::draw(DrawSurface& p_target, const DrawStates& p_drawStates)
{
	if (!m_drawReady) initForDraw();
	if (!m_spriteMesh.VBOInitialized) {
		m_spriteMesh.pushVBOToGPU();
	}
	DrawStates newStates = DrawStates(p_drawStates);
	if (m_attachedShader != nullptr) {
		newStates.attachShader(m_attachedShader);
	}
	if (m_attachedTexture.initialized) {
		newStates.attachTexture(m_attachedTexture);
	}

	if (m_attachedShader) {
		m_attachedShader->use();
		if (opacity != 0.f) { // scuffed check, this is kinda bad
			m_attachedShader->setFloatUniform(m_attachedShader->getUniformLoc("opacity"), opacity);
		}
	}

	// Multiply it with the existing state transform, just in case it's relative to another coordinate system. Model space to world space.
	newStates.setTransform(p_drawStates.m_transform * getObjectTransform());

	p_target.draw(m_spriteMesh, GL_TRIANGLES, newStates);
	if (m_attachedShader && opacity != 0.f)
		m_attachedShader->setFloatUniform(m_attachedShader->getUniformLoc("opacity"), 0.f); // keep it defaulted for other things using the shader.
}

void Sprite::setOriginRelative(OriginLoc p_origin)
{
	setOrigin(bounds, p_origin);
}

void Sprite::setBounds(Rect p_bounds)
{
	if (bounds.xy == p_bounds.xy && bounds.wh == p_bounds.wh) return;
	bounds = p_bounds;
	glm::vec2 tl = bounds.getTL();
	glm::vec2 tr = bounds.getTR();
	glm::vec2 bl = bounds.getBL();
	glm::vec2 br = bounds.getBR();

	m_spriteMesh.remove();
	m_spriteMesh.pushVertices({
		tl.x, tl.y, 0.0f, 0.0f, 0.0f, // vertex 1
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 2
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 3
		bl.x, bl.y, 0.0f, 0.0f, 1.0f, // vertex 4
		tr.x, tr.y, 0.0f, 1.0f, 0.0f, // vertex 5
		br.x, br.y, 0.0f, 1.0f, 1.0f // vertex 6
	});
	if (m_drawReady)
		m_spriteMesh.pushVBOToGPU();

}

void Sprite::setOpacity(float p_opacity)
{
	opacity = p_opacity;
}
