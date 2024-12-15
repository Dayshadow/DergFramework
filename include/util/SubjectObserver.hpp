#pragma once

#include "util/SharedQueue.hpp"
#include <vector>

template<typename T>
class Observer;

template<typename T>
class Subject {
	friend class Observer<T>;
public:
	Subject() {};

	void notifyAll(const T& p_msg) {
		for (Observer<T>* obs : m_observers) {
			obs->recieve(p_msg);
		}
	}

	//static Subject<T>& Get() {
	//	static Subject<T> s_instance;
	//	return s_instance;
	//}
private:
	void subscribe(Observer<T>* p_obs) {
		std::unique_lock lock(m_obsLock);
		for (auto i = m_observers.begin(); i != m_observers.end(); i++) {
			if (*i == p_obs) {
				return;
			}
		}
		m_observers.push_back(p_obs);
	}
	void unsubscribe(Observer<T>* p_obs) {
		std::unique_lock lock(m_obsLock);
		for (auto i = m_observers.begin(); i != m_observers.end(); i++) {
			if (*i == p_obs) {
				m_observers.erase(i);
				break;
			}
		}
	}
	std::vector<Observer<T>*> m_observers;
	std::mutex m_obsLock;

};

template<typename T>
class Observer {
	friend class Subject<T>;
public:
	Observer(Subject<T>& p_subject) :
	m_subject(p_subject) {
		p_subject.subscribe(this);
	};
	~Observer() {
		m_subject.unsubscribe(this);
	}
	Observer(const Observer<T>&) = delete;

	void subscribe() {
		m_subject.subscribe(this);
	};

	void unsubscribe() {
		m_subject.unsubscribe(this);
	};
	std::optional<T> observe() {
		return m_inbound.tryPop();
	}

	void clear() {
		m_inbound.clear();
	}

private:
	void recieve(const T& p_msg) {
		// if nobody is listening, don't talk
		if (m_inbound.length() < 0x100)
			m_inbound.push(p_msg);
	}
	Subject<T>& m_subject;
	SharedQueue<T> m_inbound;
};