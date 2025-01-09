#include <ecg_timer.h>

custom_timer_t::custom_timer_t(bool need_start) {
	if (need_start) start();
}

std::chrono::milliseconds custom_timer_t::get_delta() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(m_end_time - m_start_time);
}

void custom_timer_t::start() {
	m_start_time = std::chrono::high_resolution_clock::now();
}

void custom_timer_t::end() {
	m_end_time = std::chrono::high_resolution_clock::now();
}

std::ostream& operator<<(std::ostream& stream, custom_timer_t& timer) {
	stream << timer.get_delta().count() << "ms ";
	return stream;
}