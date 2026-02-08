#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "../core/db_manager.h"

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(DBManager& db, QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    void setupUI();

    DBManager& m_db;
    QLineEdit* m_loginEdit;
    QLineEdit* m_passEdit;
    QPushButton* m_loginBtn;
};
