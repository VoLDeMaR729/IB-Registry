#pragma once
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include "domain_types.h"

class DBManager {
public:
    DBManager();
    void connectToDb();

    // Основные методы
    bool addCompany(const Company &company);
    bool updateCompany(const Company &company);
    bool removeCompany(const QString &name);
    
    // Получение полных данных для редактирования
    Company getCompanyFullData(const QString &inn);

    QSqlTableModel* getTableModel(QObject *parent = nullptr);
    bool authenticate(const QString &login, const QString &password);

private:
    QSqlDatabase m_db;
};
