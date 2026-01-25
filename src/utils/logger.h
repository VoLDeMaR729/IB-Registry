#pragma once
#include <spdlog/spdlog.h>

// статический класс для логов
class Logger {
public:
    static void init();
};

// макросы для удобства
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
