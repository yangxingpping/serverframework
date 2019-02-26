
#include "LogInit.h"
#include "comm_def.h"
#include "Utils.h"

#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>

bool InitLogMoudle()
{
	bool bret = false;
	static std::shared_ptr<spdlog::logger> gLogger;

	static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> gStdSink;
	static std::shared_ptr<spdlog::sinks::daily_file_sink_mt> gDailyFileSink;
	try
	{
		gStdSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		gStdSink->set_level(spdlog::level::info);

		gDailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(LOG_FILE_NAME, true, 0, 0);
		gDailyFileSink->set_level(spdlog::level::info);
		gLogger = std::shared_ptr<spdlog::logger>(new spdlog::logger(LOGGER_NAME, { gStdSink, gDailyFileSink }));
		spdlog::register_logger(gLogger);
		auto ccc = Utils::SGetLogger();
		gLogger->info("initial logger success");

		bret = true;
	}
	catch (const spdlog::spdlog_ex & ex)
	{
		std::cout << "initial log module failed with  error: " << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "initial log module failed " << std::endl;
	}
	return bret;
}