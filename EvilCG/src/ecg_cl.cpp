#include <ecg_cl.h>

namespace ecg {
	cl_int ecg_host_ctrl::default_init() {
		std::vector<cl_platform_id> platform_ids;
		cl_int result = CL_SUCCESS;
		cl_uint num_platforms = 0;

#ifdef _DEBUG
		result = clGetPlatformIDs(0, nullptr, &num_platforms);
		platform_ids.resize(num_platforms);
		result = clGetPlatformIDs(num_platforms, platform_ids.data(), nullptr);

		for (auto platform : platform_ids) {
			std::string platform_name;
			size_t platform_name_sz;

			clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platform_name_sz);
			platform_name.resize(platform_name_sz);
			clGetPlatformInfo(platform, CL_PLATFORM_NAME, platform_name_sz, platform_name.data(), nullptr);

			std::cout << "Platform: " << platform_name << std::endl;
		}
#endif

		m_platform = cl::Platform::getDefault();
		result = m_platform.getDevices(CL_DEVICE_TYPE_CPU, &m_cpu_devices);
		result = m_platform.getDevices(CL_DEVICE_TYPE_GPU, &m_gpu_devices);

#ifdef _DEBUG
		for (auto& dev : m_cpu_devices)
			std::cout << "CPU Device Name: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
		for (auto& dev : m_gpu_devices)
			std::cout << "GPU Device Name: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
#endif

		m_context = cl::Context(CL_DEVICE_TYPE_GPU);
		m_cmd_queue = cl::CommandQueue(m_context,
			CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

		return result;
	}

	ecg_host_ctrl::ecg_host_ctrl() {
		static std::once_flag m_init_flag;
		std::call_once(m_init_flag, [this] { default_init(); });
	}

	ecg_host_ctrl& ecg_host_ctrl::get_instance() {
		static ecg_host_ctrl m_instance;
		return m_instance;
	}
}