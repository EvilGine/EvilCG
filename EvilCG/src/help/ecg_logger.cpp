#include <help/ecg_logger.h>

namespace ecg {
	std::shared_ptr<spdlog::logger> g_ecg_logger = nullptr;
	const std::string g_unknown_error = "Unknown error";
	std::mutex g_ecg_logger_mutex;

	void info(const std::string& msg) {
		std::scoped_lock lock{ g_ecg_logger_mutex };
		g_ecg_logger->info(msg);
	}

	void error(const std::string& msg) {
		std::scoped_lock lock{ g_ecg_logger_mutex };
		g_ecg_logger->error(msg);
	}

	void warning(const std::string& msg) {
		std::scoped_lock lock{ g_ecg_logger_mutex};
		g_ecg_logger->warn(msg);
	}
}