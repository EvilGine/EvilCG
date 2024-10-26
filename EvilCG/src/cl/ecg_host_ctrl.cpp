#include <cl/ecg_host_ctrl.h>

namespace ecg {
	cl::Context& ecg_host_ctrl::get_context() {
		return m_context;
	}

	cl::Device& ecg::ecg_host_ctrl::get_device() {
		return m_main_device;
	}

	cl::CommandQueue& ecg_host_ctrl::get_cmd_queue() {
		return m_cmd_queue;
	}

	template <std::ranges::range Iterable>
	cl::Device ecg_host_ctrl::choose_device(const Iterable& devices) {
		cl::Device main_device;
		size_t curr_score = 0;
		size_t max_score = 0;

		for (const cl::Device& dev : devices) {
			curr_score = get_device_score(dev);
			if (curr_score > max_score) {
				max_score = curr_score;
				main_device = dev;
			}
		}

		return main_device;
	}

	size_t ecg_host_ctrl::get_device_score(const cl::Device& dev) {
		cl_bool is_compute_available = dev.getInfo<CL_DEVICE_COMPILER_AVAILABLE>();
		cl_bool is_available = dev.getInfo<CL_DEVICE_AVAILABLE>();

		if (!is_available || !is_compute_available) return 0;

		cl_uint compute_units = dev.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
		cl_uint max_frequency = dev.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
		size_t score = compute_units * max_frequency;
		return score;
	}

	cl::Platform ecg_host_ctrl::choose_platform() {
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);

		if (all_platforms.size() == 0)
			return cl::Platform();
		else {
			// TODO: write score function for platforms
			return all_platforms[0];
		}
	}

	cl::Device ecg_host_ctrl::find_best_device() {
		std::map<cl_platform_id, cl::vector<cl::Device>> devices_of_platform;
		std::vector<cl::Platform> platforms;

		cl::Platform::get(&platforms);
		for (cl::Platform& platform : platforms) {
			cl_platform_id id = platform();
			auto& item = devices_of_platform[id];
			platform.getDevices(CL_DEVICE_TYPE_ALL, &item);
		}

		std::pair<size_t, cl::Device> score_with_device = { 0, cl::Device() };
		for (auto& item : devices_of_platform) {
			for (cl::Device& dev : item.second) {
				size_t units = dev.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
				size_t freq = dev.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
				size_t score = units * freq;

				if (score_with_device.first <= score) {
					score_with_device.first = score;
					score_with_device.second = dev;
				}
			}
		}

		return score_with_device.second;
	}

	cl_int ecg_host_ctrl::default_init() {
		cl_int op_res = CL_SUCCESS;
		cl_uint num_platforms = 0;
		m_is_initialized = false;

		cl::Device dev = find_best_device();

		if (dev != cl::Device()) {
			m_main_device = dev;
			m_context = cl::Context(m_main_device);
			m_cmd_queue = cl::CommandQueue(m_context,
				CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
			m_is_initialized = true;
		}

		return op_res;
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