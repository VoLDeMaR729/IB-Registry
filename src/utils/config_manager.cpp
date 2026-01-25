#include "config_manager.h"
#include "logger.h"
#include <fstream>

DbConfig ConfigManager::loadDbConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("config file not found: {}", path);
        // возвращаем дефолт если файла нет
        return {"localhost", 5432, "ib_company_db", "admin", "secret_password"};
    }

    json j;
    file >> j;

    DbConfig cfg;
    cfg.host = j.value("host", "localhost");
    cfg.port = j.value("port", 5432);
    cfg.db_name = j.value("db_name", "ib_company_db");
    cfg.user = j.value("user", "admin");
    cfg.password = j.value("password", "secret_password");

    LOG_INFO("config loaded successfully");
    return cfg;
}
