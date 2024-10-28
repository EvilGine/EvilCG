#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <ctime>

class timer_t {
public:
	timer_t(bool need_start = false) {
		if (need_start) start();
	}

	std::chrono::milliseconds get_delta() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_end_time - m_start_time);
	}

	void start() {
		m_start_time = std::chrono::high_resolution_clock::now();
	}

	void end() {
		m_end_time = std::chrono::high_resolution_clock::now();
	}

private:
	std::chrono::steady_clock::time_point m_start_time;
	std::chrono::steady_clock::time_point m_end_time;

	friend std::ostream& operator<<(std::ostream& stream, timer_t& timer);
};

std::ostream& operator<<(std::ostream& stream, timer_t& timer) {
	stream << timer.get_delta().count() << "ms ";
	return stream;
}

#endif