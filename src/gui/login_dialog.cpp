#include "login_dialog.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

LoginDialog::LoginDialog(DBManager& db, QWidget *parent)
    : QDialog(parent), m_db(db)
{
    setupUI();
}

void LoginDialog::setupUI() {
    setWindowTitle("Авторизация");
    resize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_loginEdit = new QLineEdit(this);
    m_loginEdit->setPlaceholderText("Логин");
    layout->addWidget(new QLabel("Логин:"));
    layout->addWidget(m_loginEdit);

    m_passEdit = new QLineEdit(this);
    m_passEdit->setPlaceholderText("Пароль");
    m_passEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(new QLabel("Пароль:"));
    layout->addWidget(m_passEdit);

    m_loginBtn = new QPushButton("Войти", this);
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    layout->addWidget(m_loginBtn);
}

void LoginDialog::onLoginClicked() {
    QString login = m_loginEdit->text();
    QString pass = m_passEdit->text();

    if (m_db.authenticate(login, pass)) {
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}
