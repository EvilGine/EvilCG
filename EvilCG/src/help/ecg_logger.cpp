#include <help/ecg_logger.h>

namespace ecg {
	std::shared_ptr<spdlog::logger> g_ecg_logger = nullptr;
	const std::string g_unknown_error = "Unknown error";
	std::mutex g_ecg_set_logger_mutex;
}