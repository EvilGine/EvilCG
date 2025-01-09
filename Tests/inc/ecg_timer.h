#ifndef TIMER_H
#define TIMER_H
#include <fstream>
#include <chrono>
#include <ctime>

class custom_timer_t {
public:
	custom_timer_t(bool need_start = false);
	std::chrono::milliseconds get_delta();
	void start();
	void end();

private:
	#ifdef _WIN32
	std::chrono::steady_clock::time_point m_start_time;
	std::chrono::steady_clock::time_point m_end_time;
	#else
	std::chrono::_V2::system_clock::time_point m_start_time;
	std::chrono::_V2::system_clock::time_point m_end_time;
	#endif

	friend std::ostream& operator<<(std::ostream& stream, custom_timer_t& timer);
};

#endif