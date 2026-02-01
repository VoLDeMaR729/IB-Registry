#pragma once

#include <QString>
#include <QDate>

// Структура компании- передачa данных между БД и Интерфейсом
struct Company {
    QString name;
    QString type;
    QDate licenseDate;
    QString description;
};
