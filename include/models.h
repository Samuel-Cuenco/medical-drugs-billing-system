#pragma once

#include <QString>
#include <QMetaType> // Required for Q_DECLARE_METATYPE

struct Product {
    QString id;
    QString name;
    QString description;
    double price;
    int stock;
};

Q_DECLARE_METATYPE(Product);