#include <QApplication>
#include "gui/mainwindow.h"
#include "gui/login_dialog.h"
#include "core/db_manager.h"
#include "utils/logger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Logger::init(); 
    LOG_INFO("Запуск приложения...");

    DBManager db;
    db.connectToDb();

    LoginDialog loginDlg(db);
    if (loginDlg.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow window(nullptr);
    window.show();

    return app.exec();
}
