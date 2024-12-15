#pragma once
#include <forward_list>
#include <mutex>
template<typename T>
class SharedList {
public:
	static SharedList<T>& Get() {
		static SharedList instance;
		return instance;
	}

	std::forward_list<T>::iterator insert_after(std::forward_list<T>::const_iterator pos, const T& value) {
		std::unique_lock<std::mutex> lock(m_mtx);
		return m_list.insert_after(pos, value);
	}
	std::forward_list<T>::iterator insert_after(std::forward_list<T>::const_iterator pos, T&& value) {
		std::unique_lock<std::mutex> lock(m_mtx);
		return m_list.insert_after(pos, value);
	}
	template< class... Args >
	std::forward_list<T>::iterator emplace_after(std::forward_list<T>::const_iterator pos, Args&&... args) {
		std::unique_lock<std::mutex> lock(m_mtx);
		m_list.emplace_after(pos, std::forward<Args>(args)...);
	};
	template< class... Args >
	void emplace_front(Args&&... args) {
		std::unique_lock<std::mutex> lock(m_mtx);
		m_list.emplace_front(std::forward<Args>(args)...);
	};
	void push_front(const T& value) {
		std::unique_lock<std::mutex> lock(m_mtx); // could change, might as well be safe
		m_list.push_front(value);
	}
	void push_front(T&& value) {
		std::unique_lock<std::mutex> lock(m_mtx); // could change, might as well be safe
		m_list.push_front(value);
	}
	bool empty() const {
		std::unique_lock<std::mutex> lock(m_mtx);
		return m_list.empty();
	}
	auto begin() {
		std::unique_lock<std::mutex> lock(m_mtx); // could change, might as well be safe
		return m_list.begin();
	}
	auto end() {
		std::unique_lock<std::mutex> lock(m_mtx); // could change, might as well be safe
		return m_list.end();
	}

private:
	std::forward_list<T> m_list;
	mutable std::mutex m_mtx;
};