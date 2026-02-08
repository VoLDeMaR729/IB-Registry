#include "db_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DBManager::DBManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

void DBManager::connectToDb() {
    m_db.setHostName("ib_registry_db");
    m_db.setDatabaseName("ib_registry");
    m_db.setUserName("postgres");
    m_db.setPassword("postgres");
    if (!m_db.open()) qDebug() << m_db.lastError().text();
}

bool DBManager::authenticate(const QString &login, const QString &password) {
    return (login == "admin" && password == "admin");
}

bool DBManager::addCompany(const Company &c) {
    if (!m_db.isOpen()) connectToDb();
    m_db.transaction();

    // 1. Город
    QSqlQuery qCity;
    qCity.prepare("SELECT id FROM cities WHERE name = :name");
    qCity.bindValue(":name", c.city);
    int cityId = 1; 
    if(qCity.exec() && qCity.next()) cityId = qCity.value(0).toInt();

    // 2. Компания
    QSqlQuery qComp;
    qComp.prepare("INSERT INTO companies (name, full_name, inn, ogrn, address, website, city_id) "
                  "VALUES (:n, :fn, :inn, :ogrn, :addr, :site, :cid) RETURNING id");
    qComp.bindValue(":n", c.name);
    qComp.bindValue(":fn", c.fullName);
    qComp.bindValue(":inn", c.inn);
    qComp.bindValue(":ogrn", c.ogrn);
    qComp.bindValue(":addr", c.address);
    qComp.bindValue(":site", c.website);
    qComp.bindValue(":cid", cityId);

    if (!qComp.exec()) { m_db.rollback(); return false; }
    
    qComp.next();
    int companyId = qComp.value(0).toInt();

    // 3. Лицензии
    for (const auto &lic : c.licenses) {
        QSqlQuery qLic;
        qLic.prepare("INSERT INTO licenses (company_id, activity_type, license_number, issue_date) "
                     "VALUES (:cid, :type, :num, :date)");
        qLic.bindValue(":cid", companyId);
        qLic.bindValue(":type", lic.activityType);
        qLic.bindValue(":num", lic.number);
        qLic.bindValue(":date", lic.date);
        qLic.exec();
    }

    // 4. Услуги
    for (const auto &servName : c.services) {
        QSqlQuery qServId;
        qServId.prepare("SELECT id FROM services WHERE name = :name");
        qServId.bindValue(":name", servName);
        if(qServId.exec() && qServId.next()) {
            QSqlQuery qLink;
            qLink.prepare("INSERT INTO company_services (company_id, service_id) VALUES (:cid, :sid)");
            qLink.bindValue(":cid", companyId);
            qLink.bindValue(":sid", qServId.value(0).toInt());
            qLink.exec();
        }
    }
    return m_db.commit();
}

bool DBManager::updateCompany(const Company &c) {
    if (!m_db.isOpen()) connectToDb();
    m_db.transaction();

    // 1. Обновляем основные поля
    QSqlQuery qUpd;
    qUpd.prepare("UPDATE companies SET name=:n, full_name=:fn, address=:addr, website=:site, city_id=(SELECT id FROM cities WHERE name=:city) "
                 "WHERE inn=:inn");
    qUpd.bindValue(":n", c.name);
    qUpd.bindValue(":fn", c.fullName);
    qUpd.bindValue(":addr", c.address);
    qUpd.bindValue(":site", c.website);
    qUpd.bindValue(":city", c.city);
    qUpd.bindValue(":inn", c.inn);
    
    if (!qUpd.exec()) { m_db.rollback(); return false; }

    QSqlQuery qId;
    qId.prepare("SELECT id FROM companies WHERE inn=:inn");
    qId.bindValue(":inn", c.inn);
    if (!qId.exec() || !qId.next()) { m_db.rollback(); return false; }
    int id = qId.value(0).toInt();

    // 2. Чистим старое
    QSqlQuery qDelLic;
    qDelLic.prepare("DELETE FROM licenses WHERE company_id = :id");
    qDelLic.bindValue(":id", id);
    qDelLic.exec();

    QSqlQuery qDelServ;
    qDelServ.prepare("DELETE FROM company_services WHERE company_id = :id");
    qDelServ.bindValue(":id", id);
    qDelServ.exec();

    // 3. Лицензии
    for (const auto &lic : c.licenses) {
        QSqlQuery qLic;
        qLic.prepare("INSERT INTO licenses (company_id, activity_type, license_number, issue_date) "
                     "VALUES (:cid, :type, :num, :date)");
        qLic.bindValue(":cid", id);
        qLic.bindValue(":type", lic.activityType);
        qLic.bindValue(":num", lic.number);
        qLic.bindValue(":date", lic.date);
        qLic.exec();
    }

    // 4. Услуги
    for (const auto &servName : c.services) {
        QSqlQuery qServId;
        qServId.prepare("SELECT id FROM services WHERE name = :name");
        qServId.bindValue(":name", servName);
        if(qServId.exec() && qServId.next()) {
            QSqlQuery qLink;
            qLink.prepare("INSERT INTO company_services (company_id, service_id) VALUES (:cid, :sid)");
            qLink.bindValue(":cid", id);
            qLink.bindValue(":sid", qServId.value(0).toInt());
            qLink.exec();
        }
    }

    return m_db.commit();
}

Company DBManager::getCompanyFullData(const QString &inn) {
    Company c;
    if (!m_db.isOpen()) connectToDb();
    
    QSqlQuery q;
    q.prepare("SELECT c.id, c.name, c.full_name, c.inn, c.ogrn, c.address, c.website, ci.name "
              "FROM companies c JOIN cities ci ON c.city_id = ci.id WHERE c.inn = :inn");
    q.bindValue(":inn", inn);
    
    if (q.exec() && q.next()) {
        c.id = q.value(0).toInt();
        c.name = q.value(1).toString();
        c.fullName = q.value(2).toString();
        c.inn = q.value(3).toString();
        c.ogrn = q.value(4).toString();
        c.address = q.value(5).toString();
        c.website = q.value(6).toString();
        c.city = q.value(7).toString();
    }
    QSqlQuery qLic;
    qLic.prepare("SELECT activity_type, license_number, issue_date FROM licenses WHERE company_id = :id");
    qLic.bindValue(":id", c.id);
    qLic.exec();
    while(qLic.next()) {
        c.licenses.append({
            qLic.value(0).toString(),
            qLic.value(1).toString(),
            qLic.value(2).toDate()
        });
    }

    QSqlQuery qServ;
    qServ.prepare("SELECT s.name FROM services s "
                  "JOIN company_services cs ON s.id = cs.service_id "
                  "WHERE cs.company_id = :id");
    qServ.bindValue(":id", c.id);
    qServ.exec();
    while(qServ.next()) {
        c.services.append(qServ.value(0).toString());
    }

    return c;
}

bool DBManager::removeCompany(const QString &name) {
    if (!m_db.isOpen()) connectToDb();
    QSqlQuery query;
    query.prepare("DELETE FROM companies WHERE name = :name");
    query.bindValue(":name", name);
    return query.exec();
}

QSqlTableModel* DBManager::getTableModel(QObject *parent) {
    if (!m_db.isOpen()) connectToDb();
    QSqlTableModel *model = new QSqlTableModel(parent, m_db);
    model->setTable("companies");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    return model;
}
