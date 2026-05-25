#pragma once

#include <QString>
#include <QMetaType> // Required for Q_DECLARE_METATYPE

struct Product {
    QString id;
    QString name;
    QString description;
    double price;
    int stock;

    // Centrally managed display logic
    QString toSearchString() const {
        return QString("%1 | %2 | %3 | ₱%4 | stock:%5")
            .arg(id, name, description)
            .arg(price, 0, 'f', 2).arg(stock);
    }

    QString toCartString(int quantity) const {
        return QString("%1 x%2 — ₱%3 each — stock:%4 — ID:%5")
            .arg(name).arg(quantity).arg(price, 0, 'f', 2).arg(stock).arg(id);
    }
};

Q_DECLARE_METATYPE(Product);