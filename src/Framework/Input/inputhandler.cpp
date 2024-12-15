
#include "Framework/Input/InputHandler.hpp"

void InputHandler::processKeyDown(int p_sym) {
	std::unique_lock<std::mutex> lock(m_inputMutex);
	if (std::find(m_keys.begin(), m_keys.end(), p_sym) == m_keys.end()) {
		//m_keySubject.notifyAll(KeyEvent(true, p_sym));
		m_keys.push_back(p_sym);
		m_keyDowns.push_back(p_sym);
	}
};
void InputHandler::processKeyUp(int p_sym) {
	std::unique_lock<std::mutex> lock(m_inputMutex);
	for (int i = 0; i < m_keys.size(); i++) {
		if (m_keys[i] == p_sym) {
			m_keys.erase(m_keys.begin() + i);
			//m_keySubject.notifyAll(KeyEvent(false, p_sym));
			break;
		}
	}
};
bool InputHandler::testKey(int p_sym) {
	std::unique_lock<std::mutex> lock(m_inputMutex);
	if (std::find(m_keys.begin(), m_keys.end(), p_sym) != m_keys.end()) {
		return true;
	}
	return false;
}
bool InputHandler::testKeyDown(int p_sym)
{
	std::unique_lock<std::mutex> lock(m_inputMutex);
	for (int i = 0; i < m_keyDowns.size(); i++) {
		if (m_keyDowns[i] == p_sym) {
			m_keyDowns.erase(m_keyDowns.begin() + i);
			return true;
		}
	}
	return false;
};