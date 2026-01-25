#pragma once

#include <QSqlDatabase>
#include <vector>
#include <QString>
#include "domain_types.h"

class DbManager {
public:
    DbManager();
    ~DbManager();

    // Подключение и инициализация
    bool connect();
    bool initTables();

    // Работа с компаниями
    bool addCompany(const Company& company);
    bool removeCompany(int id);
    std::vector<Company> searchCompanies(const QString& name, const QString& type);
    std::vector<Company> getAllCompanies();

    //Безопасность, Проверка логина и пароля
    bool authenticate(const QString& login, const QString& password);
    // Создание пользователя (для админа)
    bool addUser(const QString& login, const QString& password);

private:
    QSqlDatabase m_db;
    // Вспомогательная функция хеширования
    QString hashPassword(const QString& password);
};
