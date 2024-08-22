#ifndef ECG_DEVICE_H
#define ECG_DEVICE_H
#include <ecg_global.h>

namespace ecg {
	void check_cl_result();

	/// <summary>
	/// 
	/// </summary>
	class ecg_host_ctrl {
	public:
		virtual ~ecg_host_ctrl() = default;
		static ecg_host_ctrl& get_instance();

	protected:
		cl_int default_init();
		ecg_host_ctrl();

	private:
		std::vector <cl::Device> m_cpu_devices;
		std::vector<cl::Device> m_gpu_devices;
		cl::CommandQueue m_cmd_queue;
		cl::Platform m_platform;
		cl::Context m_context;

	};
}

#endif