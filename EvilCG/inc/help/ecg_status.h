#ifndef ECG_STATUS_H
#define ECG_STATUS_H
#include <ecg_global.h>

namespace ecg {
	/// <summary>
	/// Basic status code variable
	/// </summary>
	typedef int ecg_status;

	/// <summary>
	/// Status codes for different errors
	/// </summary>
	enum status_code {
		SUCCESS = 0,
		UNKNOWN_EXCEPTION,
		EMPTY_VERTEX_ARR,
		EMPTY_INDEX_ARR,
		EMPTY_NORM_ARR,
		OPENCL_ERROR,
		INVALID_ARG,
	};

	/// <summary>
	/// Handler for ecg_status with exception if status not equal SUCCESS
	/// </summary>
	class ecg_status_handler {
	public:
		ecg_status_handler() : m_status(status_code::SUCCESS) {};
		virtual ~ecg_status_handler() = default;

		ecg_status_handler& operator=(const ecg_status_handler& rhs);
		ecg_status_handler& operator=(const ecg_status& rhs);
		bool operator==(const status_code& code);
		ecg_status get_status() const;

	private:
		ecg_status m_status;

	};

	/// <summary>
	/// Custom exception for ecg_status
	/// </summary>
	class ecg_status_ex : public std::exception {
	public:
		ecg_status_ex() : m_message(""), m_status_code(status_code::UNKNOWN_EXCEPTION) {}
		ecg_status_ex(const ecg_status& status) : m_message(""), m_status_code(status) {}
		ecg_status_ex(const std::string& str) : m_message(str), m_status_code(status_code::UNKNOWN_EXCEPTION) {}
		ecg_status_ex(const std::string& str, const ecg_status& status) : m_message(str), m_status_code(status) {}

		const char* what() const noexcept override { return m_message.c_str(); }
		ecg_status get_status() { return m_status_code; }

	private:
		ecg_status m_status_code;
		std::string m_message;

	};
}

#endif