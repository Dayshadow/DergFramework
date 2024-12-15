//#pragma once
//#include <vector>
//#include <mutex>
//#include <shared_mutex>
//#include <condition_variable>
//#include <optional>
//template<typename T>
//
//class SharedVector {
//public:
//	// only safe if being ran by the modifying thread
//	std::vector<T>::iterator begin() {
//		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
//		return m_data.begin();
//	}
//	// only safe if being ran by the modifying thread
//	std::vector<T>::iterator end() {
//		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
//		return m_data.end();
//	}
//
//	T operator[](size_t index) {
//		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
//		return m_data[index];
//	}
//
//	T& operator[](size_t index) {
//		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
//		return m_data[index];
//	}
//	const T& operator[](size_t index) {
//		std::shared_lock<std::shared_mutex> lock(m_accessMutex);
//		return m_data[index];
//	}
//
//	T getValueAt(size_t index) {
//
//	}
//	
//	void reserve(size_t p_reserveAmount) {
//		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
//		m_data.reserve(p_reserveAmount);
//	}
//	void clear() {
//		std::unique_lock<std::shared_mutex> lock(m_accessMutex);
//		m_data.clear();
//	}
//
//private:
//	std::vector<T> m_data
//	std::shared_mutex m_accessMutex;
//};