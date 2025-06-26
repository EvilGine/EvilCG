#include <help/ecg_logger.h>

namespace ecg {
	std::shared_ptr<spdlog::logger> g_ecg_logger = nullptr;
	std::mutex g_ecg_set_logger_mutex;
}