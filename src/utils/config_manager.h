#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct DbConfig {
    std::string host;
    int port;
    std::string db_name;
    std::string user;
    std::string password;
};

class ConfigManager {
public:
    // загрузка конфига бд
    static DbConfig loadDbConfig(const std::string& path);
};
