#include <QApplication>
#include "gui/mainwindow.h"
#include "gui/login_dialog.h"
#include "core/db_manager.h"
#include "utils/logger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Logger::init();
    LOG_INFO("App Starting...");

    DbManager db;

    if (!db.connect()) {
        LOG_ERROR("Fatal: DB Connection failed at startup");
        return -1;
    }
    db.initTables();
    LoginDialog auth(db);
    if (auth.exec() != QDialog::Accepted) {
        LOG_INFO("Auth failed or cancelled by user");
        return 0;
    }
    LOG_INFO("Auth successful. Launching Main Window.");
    MainWindow window(db); 
    window.show();
    return app.exec();
}
