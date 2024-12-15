#include "GUI.hpp"
#include "util/ext/glm/gtc/matrix_transform.hpp"
GUI& GUI::Get()
{
	static GUI instance;
	return instance;
}
GUI::GUI()
{
	glm::mat4 mat = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, 1.f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(2.f, -2.f, 1.f));
	GUIDrawStates.setTransform(mat);
	m_root.setAbsoluteBounds(Rect(0.f, 0.f, 1.f, 1.f));
}

void GUI::draw(DrawSurface& p_target)
{
	glDisable(GL_DEPTH_TEST);
	m_root.draw(p_target, GUIDrawStates);
}

bool GUI::update(GUIEvent e)
{
	return m_root.onUpdate(e);
}

void GUI::addElement(Widget* p_elmBase)
{
	std::unique_lock<std::mutex> lock(m_elmLock);
	m_root.addChild(p_elmBase);
}

bool GUI::removeElement(std::string_view p_id)
{
	std::unique_lock<std::mutex> lock(m_elmLock);
	return m_root.removeChild(p_id);
}
