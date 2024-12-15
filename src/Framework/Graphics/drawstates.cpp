#include "Framework/Graphics/DrawStates.hpp"
#include <initializer_list>
DrawStates::DrawStates(glm::mat4& p_transform)
{
	m_transform = p_transform;
	m_shaderPtr = nullptr;
}

DrawStates::DrawStates(Texture& p_texture)
{
	m_transform = glm::mat4(1.0f);
	if (m_textures.size() != 0) {
		m_textures[0] = p_texture;
		return;
	}
	m_textures.push_back(p_texture);
	m_shaderPtr = nullptr;
}

DrawStates::DrawStates(std::vector<Texture>& p_textures)
{
	m_transform = glm::mat4(1.0f);
	m_textures = p_textures;
}

DrawStates::DrawStates(Shader* p_shader)
{
	m_transform = glm::mat4(1.0f);
	m_shaderPtr = p_shader;
}

DrawStates::DrawStates(BlendMode p_blendMode)
{
	m_transform = glm::mat4(1.0f);
	m_blendMode = p_blendMode;
}

DrawStates::DrawStates(glm::mat4& p_transform, Texture& p_texture, Shader* p_shader, BlendMode p_blendMode)
{
	m_shaderPtr = p_shader;
	m_transform = p_transform;
	m_blendMode = p_blendMode;

	if (m_textures.size() != 0) {
		m_textures[0] = p_texture;
		return;
	}
	m_textures.push_back(p_texture);
}
DrawStates::DrawStates(glm::mat4& p_transform, std::initializer_list<Texture> p_textures, Shader* p_shader, BlendMode p_blendMode)
{
	m_transform = p_transform;
	m_blendMode = p_blendMode;
	m_textures = p_textures;
	m_shaderPtr = p_shader;
}

void DrawStates::setTransform(glm::mat4 p_transform)
{
	m_transform = p_transform;
}
void DrawStates::attachShader(Shader* p_shader)
{

	m_shaderPtr = p_shader;
}
void DrawStates::attachTexture(Texture& p_texture)
{
	if (m_textures.size() != 0) {
		m_textures[0] = p_texture;
		return;
	}
	m_textures.push_back(p_texture);
}
void DrawStates::addTexture(Texture& p_texture)
{
	m_textures.push_back(p_texture);
}
void DrawStates::setTexture(size_t p_index, Texture& p_texture)
{
	if (p_index >= m_textures.size() || m_textures.size() == 0) {
		throw std::runtime_error("Attempted to set DrawState texture index out of range.");
		return;
	}
	m_textures[p_index] = p_texture;
}
void DrawStates::setBlendMode(BlendMode p_blendMode)
{
	m_blendMode = p_blendMode;
};

