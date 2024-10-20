#ifndef ECG_STATUS_H
#define ECG_STATUS_H
#include <ecg_global.h>

namespace ecg {
	const uint32_t ARG_NULL_PTR = -2;
	const uint32_t UNKNOWN_ERROR = -1;
	const uint32_t SUCCESS = 0;

	const uint32_t EMPTY_MESH = 1;
	const uint32_t EMPTY_INDEXES = 2;
	const uint32_t EMPTY_VERTEXES = 3;

	/// <summary>
	/// Basic status code variable
	/// </summary>
	typedef uint32_t ecg_status;

	/// <summary>
	/// Status codes for different errors
	/// </summary>
	enum status_code {
		INVALID_ARG = 1,
		EMPTY_VERTEX_ARR,
		EMPTY_INDEX_ARR,
		EMPTY_NORM_ARR,
		OPENCL_ERROR,
	};

	/// <summary>
	/// Handler for ecg_status with exception if status not equal SUCCESS
	/// </summary>
	class ecg_status_handler {
	public:
		ecg_status_handler() : m_status(SUCCESS) {};
		virtual ~ecg_status_handler() = default;

		ecg_status_handler& operator=(const ecg_status_handler& rhs);
		ecg_status_handler& operator=(const ecg_status& rhs);
		ecg_status get_status() const;

	private:
		ecg_status m_status;

	};

	/// <summary>
	/// Custom exception for ecg_status
	/// </summary>
	class ecg_status_ex : public std::exception {
	public:
		ecg_status_ex() : m_message(""), m_status_code(UNKNOWN_ERROR) {}
		ecg_status_ex(const ecg_status& status) : m_message(""), m_status_code(status) {}
		ecg_status_ex(const std::string& str) : m_message(str), m_status_code(UNKNOWN_ERROR) {}
		ecg_status_ex(const std::string& str, const ecg_status& status) : m_message(str), m_status_code(status) {}

		const char* what() const noexcept override { return m_message.c_str(); }
		ecg_status get_status() { return m_status_code; }

	private:
		ecg_status m_status_code;
		std::string m_message;

	};
}

#endif