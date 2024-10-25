#ifndef ECG_PROGRAM_H
#define ECG_PROGRAM_H
#include <CL/opencl.hpp>
#include <ecg_global.h>
#include <api_define.h>

namespace ecg {
	/// <summary>
	/// Program wrapper for easy work with OpenCL program.
	/// </summary>
	class ECG_API ecg_program {
	public:
		virtual ~ecg_program() = default;
		ecg_program(const ecg_program& prog) = delete;
		ecg_program(cl::Context& cont, cl::Device& dev, cl::Program::Sources& srcs);

		const bool is_program_was_built() const;
		cl::Program get_program() const;

		template <typename ... Args>
		cl_int execute(
			cl::CommandQueue& queue, std::string kernel_name,
			cl::NDRange& global, cl::NDRange& local,
			const Args& ... args) {
			cl_int op_res = CL_SUCCESS;
			cl_int arg_index = 0;

			auto set_arg_with_log = [&](cl::Kernel& kernel, auto& arg) {
				return kernel.setArg(arg_index++, arg);
				};

			if (!m_is_built)
				return CL_BUILD_PROGRAM_FAILURE;

			cl::Kernel kernel(m_program, kernel_name.c_str(), &op_res);
			(set_arg_with_log(kernel, args), ...);
			op_res = queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
			queue.finish();
			return op_res;
		}

	private:
		cl::Program m_program;
		cl::Device m_device;
		bool m_is_built;

	};
}

#endif