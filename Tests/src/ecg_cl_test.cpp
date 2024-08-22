#ifdef _DEBUG
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ecg_api.h>

TEST(cl_ecg, cl) {
	std::vector<std::thread> m_threads;
	
	auto default_join = [](std::thread& th) {
		if (th.joinable()) th.join();
	};
	
	auto test_init_func = []() {
		std::this_thread::yield();
		auto& inst = ecg::ecg_host_ctrl::get_instance();
	};

	m_threads.resize(std::thread::hardware_concurrency());
	try {
		for (auto& th : m_threads) {
			th = std::thread(test_init_func);
			if (th.joinable()) th.join();
		}
	}
	catch (...) {
		std::for_each(m_threads.begin(), m_threads.end(), default_join);
	}
	std::for_each(m_threads.begin(), m_threads.end(), default_join);
}
#endif