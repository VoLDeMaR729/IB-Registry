#pragma once

#include <QSqlDatabase>
#include <vector>
#include <QString>
#include "domain_types.h"

class DbManager {
public:
    DbManager();
    ~DbManager();

    bool connect();
    bool initTables();
    bool addCompany(const Company& company);
    bool removeCompany(const QString& name);
    bool updateCompany(const QString& name, const QString& address, const QString& licenseNum, const QDate& newDate, const QString& newDesc);
    std::vector<Company> searchCompanies(const QString& name, const QString& type);
    std::vector<Company> getAllCompanies();

    bool authenticate(const QString& login, const QString& password);
    bool addUser(const QString& login, const QString& password);

private:
    QSqlDatabase m_db;
    QString hashPassword(const QString& password);
};
