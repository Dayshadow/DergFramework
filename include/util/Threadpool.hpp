#pragma once
#include <vector>
#include <thread>
#include "SharedQueue.hpp"
#include <functional>
#include <type_traits>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
	ThreadPool() = delete;
	ThreadPool(uint16_t numThreads) {
		for (uint16_t i = 0; i < numThreads; i++) {
			m_idleFlags.push_back(false);
			m_workers.emplace_back([this, i] {
				size_t worker_id = i;
				while (true) {
					std::function<void()> task;
					waitingCount++;
					cv.notify_one();
					task = std::move(m_tasks.pop());
					waitingCount--;
					if (m_stopping) return;

					task();
				}
				});
		}
	};
	~ThreadPool() {
		m_stopping = true;
		m_tasks.clear();
		m_tasks.forceAllThreadsToPop();

		for (auto& worker : m_workers) {
			worker.join();
		}

	}

	template <typename F, typename... Args>
	auto assign(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
		using return_type = typename std::invoke_result_t<F, Args...>;

		if (m_stopping) throw std::runtime_error("Cannot enqueue a task on a stopped threadpool. How did this happen?");

		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> result = task->get_future();

		m_tasks.push([task] {(*task)(); });

		return result;
	}

	void waitUntilIdle() {
		std::unique_lock lk(m);
		cv.wait(lk, [this] { return waitingCount == m_workers.size(); });
	}
	std::atomic<uint32_t> waitingCount{0};
private:
	SharedQueue<std::function<void()>> m_tasks;
	bool m_stopping = false;

	std::condition_variable cv;
	std::mutex m;
	std::vector<std::thread> m_workers;
	std::vector<bool> m_idleFlags;
};
