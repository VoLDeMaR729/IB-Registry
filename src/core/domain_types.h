#pragma once
#include <QString>
#include <QDate>

struct Company {
    QString name;
    QString inn;
    QString ogrn;
    QString address;
    QString licenseNum;
    QString type;
    QDate licenseDate;
    QString description;
};
