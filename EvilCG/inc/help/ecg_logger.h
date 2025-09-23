#ifndef ECG_LOGGER_H
#define ECG_LOGGER_H
#include <ecg_global.h>

#define SPDLOG_ENABLE_SOURCE_LOCATION
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace ecg {
	extern std::shared_ptr<spdlog::logger> g_ecg_logger;
	extern const std::string g_unknown_error;
	extern std::mutex g_ecg_logger_mutex;

	void info(const std::string& msg);
	void error(const std::string& msg);
	void warning(const std::string& msg);
}

#endif