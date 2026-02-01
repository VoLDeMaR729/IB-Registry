#pragma once

#include <QSqlDatabase>
#include <vector>
#include <QString>
#include "domain_types.h"

class DbManager {
public:
    DbManager();
    ~DbManager();

    // Открытие соединения с БД
    bool connect();
    // Создание таблиц
    bool initTables();

    bool addCompany(const Company& company);
    bool removeCompany(const QString& name);
    bool updateCompany(const QString& name, const QDate& newDate, const QString& newDesc);

    // Поиск компаний с фильтрацией
    std::vector<Company> searchCompanies(const QString& name, const QString& type);
    
    // Получение списка всех компаний
    std::vector<Company> getAllCompanies();

    // Аутентификация пользователя 
    bool authenticate(const QString& login, const QString& password);
    
    // Добавление нового пользователя (функционал для будущего расширения)
    bool addUser(const QString& login, const QString& password);

private:
    QSqlDatabase m_db;
    QString hashPassword(const QString& password);
};
