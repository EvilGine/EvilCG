#ifdef _DEBUG
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ecg_api.h>

/// <summary>
/// Check init of OpenCL Host controller with multithreading.
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(cl_ecg, cl_init) {
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

/// <summary>
/// Check OpenCL Program Compile
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
TEST(cl_ecg, cl_program) {
	auto& host_ctrl = ecg::ecg_host_ctrl::get_instance();
	auto program = ecg::ecg_cl_program();
}
#endif