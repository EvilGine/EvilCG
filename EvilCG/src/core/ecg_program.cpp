#include <core/ecg_program.h>

namespace ecg {
	std::shared_ptr<ecg_program> ecg_program::get_program(cl::Context& cont, cl::Device& dev, cl::Program::Sources& srcs, std::string prog_name) {
		static std::unordered_map<std::string, std::shared_ptr<ecg_program>> g_cached_programs;
		auto it = g_cached_programs.find(prog_name);

		if (it != g_cached_programs.end()) return it->second;
		else {
			auto prog = std::make_shared<ecg_program>(cont, dev, srcs);
			g_cached_programs[prog_name] = prog;
		}

		return g_cached_programs[prog_name];
	}

	ecg_program::ecg_program(cl::Context& cont, cl::Device& dev, cl::Program::Sources& srcs) {
		m_is_built = true;
		try {
			cl_int op_res = CL_SUCCESS;
			m_program = cl::Program(cont, srcs, &op_res);
			op_res = m_program.build(dev);
			m_device = dev;

			if (op_res != CL_SUCCESS) {
				cl_build_status status = m_program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev);
				if (status == CL_BUILD_ERROR) {
					// Get the build log
					std::string name = dev.getInfo<CL_DEVICE_NAME>();
					std::string buildlog = m_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
					std::cout << "Build log for " << name << ":" << std::endl << buildlog << std::endl;
				}
				m_is_built = false;
			}
		}
		catch (...) {
			m_is_built = false;
		}
	}

	const bool ecg_program::is_program_was_built() const {
		return m_is_built;
	}

	cl::Program ecg_program::get_program() const {
		return m_program;
	}
}