#ifndef ECG_PROGRAM_H
#define ECG_PROGRAM_H
#include <core/ecg_cl_version.h>
#include <ecg_api_define.h>
#include <ecg_global.h>

namespace ecg {
	template <typename T>
	constexpr bool is_opencl_type =
		std::is_same_v<T, cl_char> || std::is_same_v<T, cl_uchar> ||
		std::is_same_v<T, cl_short> || std::is_same_v<T, cl_ushort> ||
		std::is_same_v<T, cl_int> || std::is_same_v<T, cl_uint> ||
		std::is_same_v<T, cl_long> || std::is_same_v<T, cl_ulong> ||
		std::is_same_v<T, cl_float> || std::is_same_v<T, cl_double> ||
		std::is_same_v<T, cl_float2> || std::is_same_v<T, cl_float3> ||
		std::is_same_v<T, cl_float4> || std::is_same_v<T, cl_float8> ||
		std::is_same_v<T, cl_half> || std::is_same_v<T, cl_mem> ||
		std::is_same_v<T, cl_sampler> || std::is_same_v<T, cl_event> ||
		std::is_same_v<T, cl::Buffer> || std::is_same_v<T, std::nullptr_t>;

	/// <summary>
	/// Program wrapper for easy work with OpenCL program.
	/// </summary>
	class ECG_API ecg_program_wrapper {
	public:
		virtual ~ecg_program_wrapper() = default;
		ecg_program_wrapper(const ecg_program_wrapper& prog) = delete;
		ecg_program_wrapper(cl::Context& cont, cl::Device& dev, cl::Program::Sources& srcs);

		static std::shared_ptr<ecg_program_wrapper> get_program(
			cl::Context& context, cl::Device& device,
			cl::Program::Sources& sources, std::string name
		);

		const bool is_program_was_built() const;
		cl::Program get_program() const;

		template <typename... Args>
		cl_int execute(cl::CommandQueue& queue, const std::string& kernel_name,
			cl::NDRange& global_range, cl::NDRange& local_range,
			const Args&... args
		) {
			if (!m_is_built) return CL_BUILD_PROGRAM_FAILURE;

			cl_int result = CL_SUCCESS;
			cl::Kernel kernel(m_program, kernel_name.c_str(), &result);

			int arg_index = 0;
			auto set_arg_with_check = [&](auto& arg) {
				using ArgType = std::decay_t<decltype(arg)>;
				static_assert(is_opencl_type<ArgType>,
					"Unsupported argument type: must be an OpenCL type or nullptr");
				return kernel.setArg(arg_index++, arg);
			};

			(set_arg_with_check(args), ...);

			result = queue.enqueueNDRangeKernel(kernel, cl::NullRange, global_range, local_range);
			queue.finish();
			return result;
		}

	private:
		cl::Program m_program;
		cl::Device m_device;
		bool m_is_built;

	};
}

#endif