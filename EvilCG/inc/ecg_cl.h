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

		cl::Context& get_context();
		cl::Device& get_main_device();
		cl::CommandQueue& get_cmd_queue();

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
	class ecg_program {
	public:
		virtual ~ecg_program() = default;
		ecg_program(cl::Context& context, cl::Device& dev, cl::CommandQueue& cmd_queue, cl::Program::Sources& sources);

		const cl::Program& get_program() const;
		const bool is_program_was_built() const;

		cl_int build_program(cl::Device dev);
		cl_int compile_program(const cl::Program::Sources& sources);
		
		template <typename ... Arg>
		cl_int execute_program(std::string func, cl::NDRange global, cl::NDRange local, const Arg& ... args) {
			if (!m_is_built) 
				return CL_INVALID_PROGRAM;
			
			cl::Kernel kernel(m_program, func.c_str());
			cl_int op_res = CL_SUCCESS;
			cl_int arg_index = 0;

			auto set_kernel_arg = [&kernel, &arg_index](const auto& arg) {
				cl_int op_res = kernel.setArg(arg_index++, arg);
				return op_res;
			};

			auto list = { args... };
			for(auto item : list) op_res = set_kernel_arg(item);
			op_res = m_cmd_queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
			return op_res;
		}

	private:
		cl::CommandQueue m_cmd_queue;
		cl::Context m_context;
		cl::Device m_device;

		cl::Program m_program;
		bool m_is_built;

	};
}

#endif