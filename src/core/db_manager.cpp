#include "db_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>

//добавляем драйвер PostgreSQL
DbManager::DbManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

// Деструктор: закрываем соединение при выходе
DbManager::~DbManager() {
    if (m_db.isOpen()) m_db.close();
}

// Настройка и открытие соединения
bool DbManager::connect() {
    m_db.setHostName("localhost"); // Хост из docker-compose (или localhost при гибридном запуске)
    m_db.setPort(5432);
    m_db.setDatabaseName("ib_company_db");
    m_db.setUserName("admin");
    m_db.setPassword("secret_password");

    if (!m_db.open()) {
        qCritical() << "DB Connection error:" << m_db.lastError().text();
        return false;
    }
    return true;
}

// Метод инициализации
bool DbManager::initTables() {
    return true;
}

bool DbManager::addCompany(const Company& c) {
    QSqlQuery query;
    // Вызываем SQL-функцию, которая сама найдет ID типа по строке
    query.prepare("SELECT add_company_func(:n, :t, :d, :desc)");
    query.bindValue(":n", c.name);
    query.bindValue(":t", c.type);
    query.bindValue(":d", c.licenseDate);
    query.bindValue(":desc", c.description);
    
    if (!query.exec()) {
        qCritical() << "Add company error:" << query.lastError().text();
        return false;
    }
    return true;
}

//по названию
bool DbManager::removeCompany(const QString& name) {
    QSqlQuery query;
    // Вызываем SQL-функцию удаления
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

// Поиск компаний
std::vector<Company> DbManager::searchCompanies(const QString& name, const QString& type) {
    std::vector<Company> list;
    QSqlQuery query;
    query.prepare("SELECT * FROM search_companies_func(:name, :type)");
    query.bindValue(":name", name);
    query.bindValue(":type", type);

    if (query.exec()) {
        while (query.next()) {
            Company c;
            //работаем только с бизнес-данными
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

// Хеширование пароля
QString DbManager::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

// Заглушка для добавления пользователя
bool DbManager::addUser(const QString& login, const QString& password) {
    return false; 
}

// Обновление компании
bool DbManager::updateCompany(const QString& name, const QDate& newDate, const QString& newDesc) {
    QSqlQuery query;
    query.prepare("SELECT update_company_func(:n, :d, :desc)");
    query.bindValue(":n", name);
    query.bindValue(":d", newDate);
    query.bindValue(":desc", newDesc);
    
    if (!query.exec()) {
        qCritical() << "Update error:" << query.lastError().text();
        return false;
    }
    return true;
}

// Аутентификация администратора
bool DbManager::authenticate(const QString& login, const QString& password) {
    QSqlQuery query;
    // Проверка происходит внутри БД
    query.prepare("SELECT auth_user_func(:login, :hash)");
    query.bindValue(":login", login);
    query.bindValue(":hash", hashPassword(password));
    
    if (query.exec() && query.next()) {
        return query.value(0).toBool();
    }
    return false;
}
