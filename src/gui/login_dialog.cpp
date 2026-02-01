#include "login_dialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

LoginDialog::LoginDialog(DbManager& db, QWidget *parent) 
    : QDialog(parent), m_db(db) 
{
    setWindowTitle("Авторизация");
    setupUI();
}

void LoginDialog::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    resize(300, 150);

    // Логин
    layout->addWidget(new QLabel("Логин:", this));
    m_loginEdit = new QLineEdit(this);
    layout->addWidget(m_loginEdit);

    // Пароль
    layout->addWidget(new QLabel("Пароль:", this));
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password); // Скрывать символы
    layout->addWidget(m_passwordEdit);

    // Кнопка входа
    QPushButton* btnLogin = new QPushButton("Войти", this);
    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    layout->addWidget(btnLogin);
}

void LoginDialog::onLoginClicked() {
    QString login = m_loginEdit->text();
    QString pass = m_passwordEdit->text();

    if (m_db.authenticate(login, pass)) {
        accept(); // Закрыть диалог с результатом Success
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
    }
}
