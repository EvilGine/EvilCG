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
	auto& cmd_queue = host_ctrl.get_cmd_queue();
	auto& device = host_ctrl.get_main_device();
	auto& context = host_ctrl.get_context();
	const size_t size = 1024 * 1024 * 1024;

	std::srand(time(0));
	auto fill_array = [](std::vector<int>& vec, size_t sz) {
		if (vec.size() != sz) vec.resize(sz);
		for (int id = 0; id < sz; ++id)
			vec[id] = rand() % 100 + 1;
	};

	const std::string vec_add_kernel =
		"kernel void vec_add(global int* A, global int* B, global int* C )"
		"{"
		"const int idx = get_global_id(0);"
		"	C[idx] = A[idx] + B[idx];"
		"}";

	std::vector<int> a(size);
	std::vector<int> b(size);
	std::vector<int> c(size);

	std::thread a_fill_thread;
	std::thread b_fill_thread;

	try {
		a_fill_thread = std::thread(fill_array, std::ref(a), size);
		b_fill_thread = std::thread(fill_array, std::ref(b), size);
		c.resize(size);

		cl::Buffer buffer_a = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(int));
		cl::Buffer buffer_b = cl::Buffer(context, CL_MEM_READ_ONLY, size * sizeof(int));
		cl::Buffer buffer_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, size * sizeof(int));

		auto program = ecg::ecg_cl_program();
		std::vector<std::string> sources = { vec_add_kernel };
		program.compile_program(sources);
		program.build_program(device);

		if (a_fill_thread.joinable()) a_fill_thread.join();
		if (b_fill_thread.joinable()) b_fill_thread.join();

		cmd_queue.enqueueWriteBuffer(buffer_a, CL_FALSE, 0, size * sizeof(int), a.data());
		cmd_queue.enqueueWriteBuffer(buffer_b, CL_FALSE, 0, size * sizeof(int), b.data());
		cl::Kernel vecadd_kernel(program.get_program(), "vec_add");

		// Set the kernel arguments
		vecadd_kernel.setArg(0, buffer_a);
		vecadd_kernel.setArg(1, buffer_b);
		vecadd_kernel.setArg(2, buffer_c);

		// Execute the kernel
		auto start = std::chrono::high_resolution_clock::now();
			cl::NDRange global(size);
			cl::NDRange local(256);
			cmd_queue.enqueueNDRangeKernel(vecadd_kernel, cl::NullRange, global, local);
		auto end = std::chrono::high_resolution_clock::now();    
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);    
		std::chrono::duration<double, std::milli> ms_double = end - start;
		std::cout << "Main op time (int): " << ms_int.count() << " ms" << std::endl;
		std::cout << "Main op time (double): " << ms_double.count() << " ms" << std::endl;

		// Copy the output data back to the host
		cmd_queue.enqueueReadBuffer(buffer_c, CL_TRUE, 0, size * sizeof(int), c.data());
	}
	catch (...) {
		if (a_fill_thread.joinable()) a_fill_thread.join();
		if (b_fill_thread.joinable()) b_fill_thread.join();
	}
	
	bool is_func_success = true;
	for (int id = 0; id < size; ++id) {
		if (a[id] + b[id] != c[id]) {
			is_func_success = false;
			break;
		}
	}

	ASSERT_TRUE(is_func_success);
}
#endif