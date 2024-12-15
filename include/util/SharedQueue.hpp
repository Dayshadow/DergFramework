#pragma once
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <optional>

template<typename T>
// T must be default constructable 
class SharedQueue {
public:
	void push(const T& p_value) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.push(p_value);
		m_cv.notify_one();
	}
	auto pop() {
		std::unique_lock<std::mutex> lock(m_mutex);
		// kinda scuffed but good enough
		m_cv.wait(lock, [&]() { return !m_queue.empty() || m_terminateWaiting; });
		if (m_terminateWaiting) return T();
		T out = m_queue.front();
		m_queue.pop();
		return out;
	}
	bool empty() {
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}
	std::optional<T> tryPop() {
		if (!empty()) {
			std::unique_lock<std::mutex> lock(m_mutex);
			T out = m_queue.front();
			m_queue.pop();
			return out;
		}
		return std::nullopt;
	}
	// force threads to exit
	void forceAllThreadsToPop() {
		m_terminateWaiting = true;
		//for (int i = 0; i < 5; i++) // lol
		m_cv.notify_all();
	}

	void clear() {
		std::unique_lock<std::mutex> lock(m_mutex);
		std::queue<T> empty;
		m_queue.swap(empty);
	}

	size_t length() {
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.size();
	}

	static SharedQueue<T>& Get() {
		static SharedQueue<T> instance;
		return instance;
	}
private:
	std::mutex m_mutex;
	std::queue<T> m_queue;
	std::condition_variable m_cv;
	std::atomic<bool> m_terminateWaiting = false;
};