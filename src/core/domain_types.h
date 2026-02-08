#pragma once
#include <QString>
#include <QDate>
#include <QList>

struct License {
    QString activityType;
    QString number;
    QDate date;
};

struct Company {
    int id = -1;
    QString name;
    QString fullName;
    QString inn;
    QString ogrn;
    QString city;
    QString address;
    QString website;
    
    QList<License> licenses;
    QList<QString> services;
};
