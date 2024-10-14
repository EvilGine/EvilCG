#ifndef ECG_HOST_CTRL_H
#define ECG_HOST_CTRL_H
#include <ecg_global.h>

namespace ecg {
	/// <summary>
	/// Global OpenCL Host Controller.
	/// Thread-Safe - Singleton.
	/// </summary>
	class ecg_host_ctrl {
	public:
		virtual ~ecg_host_ctrl() = default;
		static ecg_host_ctrl& get_instance();

		cl::Context& get_context();
		cl::Device& get_main_device();
		cl::CommandQueue& get_cmd_queue();

	protected:
		template <std::ranges::range Iterable>
		cl::Device choose_device(const Iterable& devices);
		size_t get_device_score(const cl::Device& dev);
		cl::Platform choose_platform();
		cl::Device find_best_device();
		cl_int default_init();
		ecg_host_ctrl();

	private:
		std::vector <cl::Device> m_cpu_devices;
		std::vector<cl::Device> m_gpu_devices;
		cl::CommandQueue m_cmd_queue;
		cl::Device m_main_device;
		cl::Context m_context;

	};
}

#endif