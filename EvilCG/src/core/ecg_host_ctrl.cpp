#include <core/ecg_host_ctrl.h>

namespace ecg {
	const std::unordered_map<std::string, size_t> g_vendor_score = {
		{"NVIDIA", 1'000'000},
		{"AMD", 900'000},
		{"Intel", 0}
	};

	cl::Context& ecg_host_ctrl::get_context() {
		return m_context;
	}

	cl::Device& ecg::ecg_host_ctrl::get_device() {
		return m_main_device;
	}

	cl::CommandQueue& ecg_host_ctrl::get_cmd_queue() {
		return m_cmd_queue;
	}

	cl_int ecg_host_ctrl::get_max_work_group_size() const {
		if (m_main_device == cl::Device()) return 0;
		return m_main_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
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
		std::unordered_map<cl_platform_id, cl::vector<cl::Device>> devices_of_platform;
		std::vector<cl::Platform> platforms;

		cl::Platform::get(&platforms);
		for (cl::Platform& platform : platforms) {
			cl_platform_id id = platform();
			auto& item = devices_of_platform[id];
			platform.getDevices(CL_DEVICE_TYPE_ALL, &item);
		}

		device_t score_with_device = { -1, 0, cl::Device() };
		for (auto& item : devices_of_platform) {
			for (cl::Device& dev : item.second) {
				std::string vendor = dev.getInfo<CL_DEVICE_VENDOR>();
				size_t score = get_device_score(dev);

				score += get_vendor_bonus(vendor);
				if (score_with_device.score <= score) {
					score_with_device.score = score;
					score_with_device.device = dev;
				}
			}
		}

		return score_with_device.device;
	}

	size_t ecg_host_ctrl::get_vendor_bonus(const std::string& vendor) {
		size_t vendor_bonus = 0;
		for (const auto& known_vendor : g_vendor_score) {
			if (vendor.find(known_vendor.first) != std::string::npos) {
				vendor_bonus = known_vendor.second;
				break;
			}
		}
		return vendor_bonus;
	}

	cl_int ecg_host_ctrl::default_init(int device_id) {
		cl_int op_res = CL_SUCCESS;
		cl_uint num_platforms = 0;
		m_is_initialized = false;
		cl::Device dev;

		if (device_id >= 0) {
			auto devices = get_available_devices();
			if (device_id < devices.size()) {
				auto dev_begin = devices.begin();
				std::advance(dev_begin, device_id);
				dev = (*dev_begin).device;
			}
		}

		if(dev == cl::Device())
			dev = find_best_device();

		if (dev != cl::Device()) {
			m_main_device = dev;
			m_context = cl::Context(m_main_device);
			m_cmd_queue = cl::CommandQueue(m_context,
				CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
			m_is_initialized = true;
		}

		return op_res;
	}

	ecg_host_ctrl::ecg_host_ctrl(int device_id) {
		static std::once_flag m_init_flag;
		std::call_once(m_init_flag, [this, device_id] { default_init(device_id); });
	}

	std::list<device_t> ecg_host_ctrl::get_available_devices() {
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		std::list<device_t> result;
		int id = 0;

		for (cl::Platform& platform : platforms) {
			std::vector<cl::Device> devices;
			platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
			
			for (auto& dev : devices) {
				std::string vendor = dev.getInfo<CL_DEVICE_VENDOR>();
				size_t score = get_device_score(dev) + get_vendor_bonus(vendor);
				result.push_back(device_t{ id, score, dev });
				++id;
			}
		}

		return result;
	}

	ecg_host_ctrl& ecg_host_ctrl::get_instance(int device_id) {
		static ecg_host_ctrl m_instance(device_id);
		return m_instance;
	}
}