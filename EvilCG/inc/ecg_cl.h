#ifndef ECG_DEVICE_H
#define ECG_DEVICE_H
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

		const cl::Context& get_context() const;
		const cl::CommandQueue& get_cmd_queue() const;

	protected:
		template <std::ranges::range Iterable>
		cl::Device choose_device(const Iterable& devices);
		size_t get_device_score(const cl::Device& dev);
		cl::Platform choose_platform();
		cl_int default_init();
		ecg_host_ctrl();

	private:
		std::vector <cl::Device> m_cpu_devices;
		std::vector<cl::Device> m_gpu_devices;
		cl::CommandQueue m_cmd_queue;
		cl::Device m_main_device;
		cl::Platform m_platform;
		cl::Context m_context;

	};

	/// <summary>
	/// Program wrapper for easy work with OpenCL program.
	/// </summary>
	class ecg_cl_program {
	public:
		ecg_cl_program();
		virtual ~ecg_cl_program() = default;

		template <std::ranges::range Iterable>
		cl_int compile_program(const Iterable& sources);

	private:
		cl::Program m_program;
		bool m_is_compiled;

	};
}

#endif