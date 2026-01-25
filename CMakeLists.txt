cmake_minimum_required(VERSION 3.16)

project(IBRegistry)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Добавляем поддержку обработки MOC, UIC, RCC для Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. ПОИСК БИБЛИОТЕК

find_package(Qt6 COMPONENTS Core Widgets Sql REQUIRED)

# Эти библиотеки тоже нужны
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# PostgreSQL
find_package(PostgreSQL REQUIRED)

# 2. ПОДКЛЮЧЕНИЕ ПАПОК С КОДОМ
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 3. СПИСОК ФАЙЛОВ

# Ядро
set(CORE_SOURCES
    src/core/db_manager.cpp
    src/core/db_manager.h
    src/core/domain_types.h
)

# Утилиты
set(UTILS_SOURCES
    src/utils/logger.cpp
    src/utils/logger.h
    src/utils/config_manager.cpp
    src/utils/config_manager.h
)

# Графический интерфейс
set(GUI_SOURCES
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/add_company_dialog.cpp
    src/gui/add_company_dialog.h
    src/gui/login_dialog.cpp
    src/gui/login_dialog.h
)

# 4. СБОРКА ПРОГРАММЫ
add_executable(ib_registry_app
    src/main.cpp
    ${CORE_SOURCES}
    ${UTILS_SOURCES}
    ${GUI_SOURCES}
)

# 5. ЛИНКОВКА
target_link_libraries(ib_registry_app
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${PostgreSQL_LIBRARIES}
)
