#pragma once

#include <QString>
#include <QDate>

// структура компании
struct Company {
    int id = -1;
    QString name;// название
    QString type;// тип (вендор, интегратор)
    QDate licenseDate;// дата лицензии
    QString description;//описание
};
