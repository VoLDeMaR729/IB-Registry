#include <QApplication>
#include "gui/mainwindow.h"
#include "gui/login_dialog.h"
#include "core/db_manager.h"
#include "utils/logger.h"

int main(int argc, char *argv[]) {
    // Инициализация графического движка Qt
    QApplication app(argc, argv);

    // Инициализация системы логирования
    Logger::init();
    LOG_INFO("App Starting...");

    // 1. Создаем менеджер базы данных
    DbManager db;

    // 2. Устанавливаем соединение с PostgreSQL
    if (!db.connect()) {
        LOG_ERROR("Fatal: DB Connection failed at startup");
        return -1; // Если нет базы - закрываемся
    }
    
    // 3. Проверяем и создаем таблицы, если их нет
    db.initTables();

    // 4. Запускаем окно авторизации
    LoginDialog auth(db);
    
    // Ждем, пока пользователь введет пароль или закроет окно
    if (auth.exec() != QDialog::Accepted) {
        LOG_INFO("Auth failed or cancelled by user");
        return 0; // Выход, если авторизация не прошла
    }

    LOG_INFO("Auth successful. Launching Main Window.");
    
    // 5. Если авторизация успешна - запускаем главное окно
    MainWindow window(db); 
    window.show();

    // Запуск основного цикла приложения
    return app.exec();
}
