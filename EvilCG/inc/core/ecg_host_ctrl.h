#ifndef ECG_HOST_CTRL_H
#define ECG_HOST_CTRL_H
#include <ecg_api_define.h>
#include <ecg_global.h>

namespace ecg {
	const int default_id = -1;

	struct device_t {
		int id;
		size_t score;
		cl::Device device;
	};

	/// <summary>
	/// Global OpenCL Host Controller.
	/// Thread-Safe - Singleton.
	/// </summary>
	class ECG_API ecg_host_ctrl {
	public:
		virtual ~ecg_host_ctrl() = default;
		static std::list<device_t> get_available_devices();
		static ecg_host_ctrl& get_instance(int device_id = default_id);

		cl::Device& get_device();
		cl::Context& get_context();
		cl::CommandQueue& get_cmd_queue();
		cl_int get_max_work_group_size() const;

	protected:
		ecg_host_ctrl(int device_id = default_id);
		cl_int default_init(int device_id = default_id);

		template <std::ranges::range Iterable>
		cl::Device choose_device(const Iterable& devices);
		static size_t get_device_score(const cl::Device& dev);
		cl::Platform choose_platform();
		cl::Device find_best_device();
		
	private:
		cl::CommandQueue m_cmd_queue;
		cl::Device m_main_device;
		cl::Context m_context;
		bool m_is_initialized;

	};
}

#endif