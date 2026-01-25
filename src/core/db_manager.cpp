#include "db_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>

DbManager::DbManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

DbManager::~DbManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DbManager::connect() {
    m_db.setHostName("localhost");
    m_db.setPort(5432);
    m_db.setDatabaseName("ib_company_db");
    m_db.setUserName("admin");
    m_db.setPassword("secret_password");

    if (!m_db.open()) {
        qCritical() << "Connection error:" << m_db.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::initTables() {
    return true;
}

// Добавление компании через Хранимую Процедуру
bool DbManager::addCompany(const Company& c) {
    QSqlQuery query;
    // Вызываем функцию базы данных add_company_func
    query.prepare("SELECT add_company_func(:n, :t, :d, :desc)");
    query.bindValue(":n", c.name);
    query.bindValue(":t", c.type);
    query.bindValue(":d", c.licenseDate);
    query.bindValue(":desc", c.description);
    return query.exec();
}

// Удаление через Хранимую Процедуру
bool DbManager::removeCompany(int id) {
    QSqlQuery query;
    query.prepare("SELECT delete_company_func(:id)");
    query.bindValue(":id", id);
    return query.exec();
}

std::vector<Company> DbManager::getAllCompanies() {
    return searchCompanies("", "Все");
}

// Поиск через Хранимую Процедуру
std::vector<Company> DbManager::searchCompanies(const QString& name, const QString& type) {
    std::vector<Company> list;
    QSqlQuery query;
    
    // Вызываем функцию поиска, которая вернет таблицу
    query.prepare("SELECT * FROM search_companies_func(:name, :type)");
    
    // Передаем параметры (пустые строки, если фильтра нет)
    query.bindValue(":name", name);
    query.bindValue(":type", type);

    if (query.exec()) {
        while (query.next()) {
            Company c;
            c.id = query.value("out_id").toInt();
            c.name = query.value("out_name").toString();
            c.type = query.value("out_type").toString();
            c.licenseDate = query.value("out_date").toDate();
            c.description = query.value("out_desc").toString();
            list.push_back(c);
        }
    } else {
        qCritical() << "Search error:" << query.lastError().text();
    }
    return list;
}

//Безопасность
QString DbManager::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

// Создание пользователя
bool DbManager::addUser(const QString& login, const QString& password) {
    return false; 
}

// Аутентификация через функцию базы
bool DbManager::authenticate(const QString& login, const QString& password) {
    QSqlQuery query;
    // База сама проверит хеш и вернет TRUE/FALSE
    query.prepare("SELECT auth_user_func(:login, :hash)");
    query.bindValue(":login", login);
    query.bindValue(":hash", hashPassword(password));
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    return false;
}
