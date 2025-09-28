#include <core/ecg_program.h>

namespace ecg {
	std::shared_ptr<ecg_program_wrapper> ecg_program_wrapper::get_program(
		cl::Context& context, cl::Device& device,
		cl::Program::Sources& sources, std::string name
	) {
		static std::unordered_map<std::string, std::shared_ptr<ecg_program_wrapper>> cache;
		auto it = cache.find(name);
		if (it != cache.end()) return it->second;

		auto prog = std::make_shared<ecg_program_wrapper>(context, device, sources);
		cache[name] = prog;
		return prog;
	}

	ecg_program_wrapper::ecg_program_wrapper(
		cl::Context& context, cl::Device& device, cl::Program::Sources& sources
	) : m_device(device), m_is_built(false) {
		try {
			cl_int err = CL_SUCCESS;
			m_program = cl::Program(context, sources, &err);
			if (err != CL_SUCCESS) return;

			err = m_program.build(m_device);
			m_is_built = (err == CL_SUCCESS);

			if (!m_is_built) {
				std::string log = m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
				std::cerr << "[OpenCL Build Error] Device: "
					<< m_device.getInfo<CL_DEVICE_NAME>() << "\n"
					<< log << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "[Exception] OpenCL build failed: " << e.what() << std::endl;
			m_is_built = false;
		}
		catch (...) {
			std::cerr << "[Unknown Exception] OpenCL build failed" << std::endl;
			m_is_built = false;
		}
	}

	const bool ecg_program_wrapper::is_program_was_built() const {
		return m_is_built;
	}

	cl::Program ecg_program_wrapper::get_program() const {
		return m_program;
	}
}