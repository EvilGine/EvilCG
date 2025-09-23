#ifndef ECG_HOST_CTRL_H
#define ECG_HOST_CTRL_H
#include <core/ecg_cl_version.h>
#include <help/ecg_logger.h>
#include <ecg_api_define.h>
#include <ecg_global.h>

namespace ecg {
	const int default_id = -1;

	/// <summary>
	/// Device structure.
	/// </summary>
	struct device_t {
		int id;
		size_t score;
		cl::Device device;
	};

	/// <summary>
	/// Global OpenCL Host Controller.
	/// Thread-Safe - Singleton.
	/// </summary>
	class ECG_API ecg_cl {
	public:
		virtual ~ecg_cl();
		
		bool is_init() const;
		void release_controller(bool log = true) noexcept;
		cl_int default_init(int device_id = default_id);
		
		static std::list<device_t> get_available_devices();
		static ecg_cl& get_instance(int device_id = default_id);

		cl::Device& get_device();
		cl::Context& get_context();
		cl::CommandQueue& get_cmd_queue();
		cl_int get_max_work_group_size() const;

	protected:
		ecg_cl(int device_id = default_id);
		
		static size_t get_vendor_bonus(const std::string& vendor);
		static size_t get_device_score(const cl::Device& dev);

		template <std::ranges::range Iterable>
		cl::Device choose_device(const Iterable& devices);
		cl::Platform choose_platform();
		cl::Device find_best_device();
		
	private:
		cl::CommandQueue m_cmd_queue;
		std::atomic<bool> m_is_initialized;
		cl::Device m_main_device;
		cl::Context m_context;

	};
}

#endif