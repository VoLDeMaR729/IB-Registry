#include "logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

void Logger::init() {
    // два приемника: консоль и файл
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/app.log", true);

    auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    
    // формат: [время] [уровень] текст
    logger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
    
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
}
