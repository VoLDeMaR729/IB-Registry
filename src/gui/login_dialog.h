#pragma once

#include <QDialog>
#include <QLineEdit>
#include "../core/db_manager.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    // Передаем указатель на db, чтобы проверить пароль
    explicit LoginDialog(DbManager& db, QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    void setupUI();

    DbManager& m_db; // Ссылка на менеджер БД
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
};
