#include "db_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>
#include <thread>
#include <chrono>

DbManager::DbManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

DbManager::~DbManager() {
    if (m_db.isOpen()) m_db.close();
}

bool DbManager::connect() {
    m_db.setHostName("ib_registry_db"); 
    m_db.setPort(5432);
    m_db.setDatabaseName("ib_company_db");
    m_db.setUserName("admin");
    m_db.setPassword("secret_password");

    for (int i = 0; i < 5; ++i) {
        qDebug() << "Попытка подключения к БД..." << (i + 1);
        
        if (m_db.open()) {
            qDebug() << "Успешное подключение!";
            return true;
        }
        
        qWarning() << "Ошибка подключения:" << m_db.lastError().text();
        qWarning() << "Ждем 2 секунды...";
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    qCritical() << "Критическая ошибка: БД недоступна.";
    return false;
}

bool DbManager::initTables() {
    return true;
}

bool DbManager::addCompany(const Company& c) {
    QSqlQuery query(m_db);
    query.prepare("SELECT add_company_func(:name, :inn, :ogrn, :addr, :lnum, :type, :date, :desc)");

    query.bindValue(":name", c.name);
    query.bindValue(":inn", c.inn);           
    query.bindValue(":ogrn", c.ogrn);         
    query.bindValue(":addr", c.address);      
    query.bindValue(":lnum", c.licenseNum);   
    query.bindValue(":type", c.type);
    query.bindValue(":date", c.licenseDate);
    query.bindValue(":desc", c.description);

    if (!query.exec()) {
        qCritical() << "Add company error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::removeCompany(const QString& name) {
    QSqlQuery query(m_db);
    query.prepare("SELECT delete_company_func(:name)");
    query.bindValue(":name", name);
    
    if (!query.exec()) {
        qCritical() << "Delete error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Company> DbManager::getAllCompanies() {
    return searchCompanies("", "Все");
}

std::vector<Company> DbManager::searchCompanies(const QString& name, const QString& type) {
    std::vector<Company> list;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM search_companies_func(:name, :type)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);

    if (query.exec()) {
        while (query.next()) {
            Company c;
            c.name = query.value("out_name").toString();
            c.inn = query.value("out_inn").toString();             
            c.ogrn = query.value("out_ogrn").toString();           
            c.address = query.value("out_addr").toString();        
            c.licenseNum = query.value("out_lnum").toString();     
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

QString DbManager::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool DbManager::addUser(const QString& login, const QString& password) {
    return false; 
}

bool DbManager::updateCompany(const QString& name, const QString& address, const QString& licenseNum, const QDate& newDate, const QString& newDesc) {
    QSqlQuery query(m_db);
    query.prepare("SELECT update_company_func(:name, :addr, :lnum, :date, :desc)");
    
    query.bindValue(":name", name);
    query.bindValue(":addr", address);        
    query.bindValue(":lnum", licenseNum);     
    query.bindValue(":date", newDate);
    query.bindValue(":desc", newDesc);
    
    if (!query.exec()) {
        qCritical() << "Update error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::authenticate(const QString& login, const QString& password) {
    QSqlQuery query(m_db);
    query.prepare("SELECT auth_user_func(:login, :hash)");
    query.bindValue(":login", login);
    query.bindValue(":hash", hashPassword(password));
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    return false;
}
