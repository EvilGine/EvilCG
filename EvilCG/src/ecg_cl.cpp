#include <ecg_cl.h>

namespace ecg {
	cl::Context& ecg_host_ctrl::get_context() {
		return m_context;
	}

	cl::Device& ecg::ecg_host_ctrl::get_main_device() {
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

#ifdef _DEBUG
		auto vec_to_str = []<typename Type>(std::vector<Type> vec) {
			std::string result;
			for (auto& item : vec)
				result += " " + std::to_string(item);
			if (vec.size() > 0) result = "[" + result + " ]";
			return result;
		};

		std::string dev_name = main_device.getInfo<CL_DEVICE_NAME>();
		auto work_item_sizes = main_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
		const size_t line_size = 28 + dev_name.size();

		std::cout << "|-" << std::setw(line_size) << std::setfill('-') << "|" << std::setfill(' ') << std::endl;
		std::cout << "| Choosen Device Name : " << dev_name << std::setw(6) << "|" << std::endl;
		std::cout << "|-" << std::setw(line_size) << std::setfill('-') << "|" << std::endl;

		std::cout << "\tMAX Compute Units: " << main_device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
		std::cout << "\tMAX Clock Frequency: " << main_device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
		std::cout << "\tMAX Work Group Size: " << main_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
		std::cout << "\tMAX Work Item Sizes: " << vec_to_str(work_item_sizes) << std::endl;
		std::cout << "|-" << std::setw(line_size) << std::setfill('-') << "|" << std::endl;
#endif

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

	cl_int ecg_host_ctrl::default_init() {
		cl_int op_res = CL_SUCCESS;
		cl_uint num_platforms = 0;

#ifdef _DEBUG
		std::vector<cl_platform_id> platform_ids;
		op_res = clGetPlatformIDs(0, nullptr, &num_platforms);
		platform_ids.resize(num_platforms);
		op_res = clGetPlatformIDs(num_platforms, platform_ids.data(), nullptr);

		for (auto platform : platform_ids) {
			std::string platform_name;
			size_t platform_name_sz;

			clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platform_name_sz);
			platform_name.resize(platform_name_sz);
			clGetPlatformInfo(platform, CL_PLATFORM_NAME, platform_name_sz, platform_name.data(), nullptr);

			std::cout << "Platform: " << platform_name << std::endl;
		}
#endif

		m_platform = choose_platform();
		if (m_platform == cl::Platform()) return CL_INVALID_PLATFORM;

		op_res = m_platform.getDevices(CL_DEVICE_TYPE_CPU, &m_cpu_devices);
		op_res = m_platform.getDevices(CL_DEVICE_TYPE_GPU, &m_gpu_devices);

#ifdef _DEBUG
		for (auto& dev : m_cpu_devices)
			std::cout << "CPU Device Name: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
		for (auto& dev : m_gpu_devices)
			std::cout << "GPU Device Name: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl;
#endif
		m_main_device = choose_device(m_gpu_devices);

		m_context = cl::Context(m_main_device);
		m_cmd_queue = cl::CommandQueue(m_context,
			CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);

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

	ecg_program::ecg_program(cl::Context& context, cl::Device& dev, cl::CommandQueue& cmd_queue, cl::Program::Sources& sources) {
		m_cmd_queue = cmd_queue;
		m_context = context;
		m_is_built = false;
		m_device = dev;
	}

	const cl::Program& ecg_program::get_program() const {
		return m_program;
	}

	const bool ecg_program::is_program_was_built() const {
		return m_is_built;
	}

	cl_int ecg_program::build_program(cl::Device dev) {
		cl_int op_res = m_program.build(dev);
		return op_res;
	}

	cl_int ecg_program::compile_program(const cl::Program::Sources& sources) {
		cl_int op_res = CL_SUCCESS;
		m_program = cl::Program(m_context, sources, &op_res);
		if (op_res == CL_SUCCESS) m_is_built = true;
		return op_res;
	}
}