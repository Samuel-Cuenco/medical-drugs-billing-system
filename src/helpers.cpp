#include "helpers.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlDatabase> // handle sql connections
#include <QSqlQuery> // use sql commands
#include <QSqlError>
#include <QDir>

void Helpers::loadUI(QWidget* widget) {
    if (!widget) return;

    QFile file(":/mainwindow.css");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString loadedStyleSheet = stream.readAll();

        widget->setStyleSheet(loadedStyleSheet);

        file.close();
    } else {
        // send error if not found
        qDebug() << "Error: Could not find the CSS file in resources (:/mainwindow.css)";
    }
}

QSqlDatabase Helpers::openDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("jupiter.db");

    if (!db.open()) {
        qWarning() << "Cannot open database:" << db.lastError().text();
        return db;
    }

    QSqlQuery query(db);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS products (
            item_id TEXT PRIMARY KEY,
            item_name TEXT NOT NULL,
            description TEXT,
            retail_price REAL,
            stock INTEGER
        )
    )");

    return db;
}

QStringList Helpers::parseCsvLine(const QString &line) {
    QStringList fields;
    QString field;
    bool inQuotes = false;

    for (QChar c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.append(field.trimmed());
            field.clear();
        } else {
            field += c;
        }
    }

    fields.append(field.trimmed());
    return fields;
}

void Helpers::importCsvToDatabase(const QString &csvPath, QSqlDatabase db) {
    QFile file(csvPath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "CSV not found or cannot open:" << csvPath << " (cwd:)" << QDir::currentPath();
    return;
}
    QSqlQuery countQuery(db);
    if (countQuery.exec("SELECT COUNT(*) FROM products") && countQuery.next()) {
        if (countQuery.value(0).toInt() > 0) {
            qWarning() << "Products in DB after import:" << countQuery.value(0).toInt();
            return;
        }
    }


    QTextStream in(&file);
    bool firstLine = true;
    QSqlQuery insert(db);
    insert.prepare(R"(
        INSERT OR IGNORE INTO products
            (item_id, item_name, description, retail_price, stock)
        VALUES
            (:id, :name, :description, :price, :stock)
    )");

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (firstLine) {
            firstLine = false;
            continue;
        }
        if (line.trimmed().isEmpty()) continue;

        QStringList fields = parseCsvLine(line);
        if (fields.size() < 5) continue;

        insert.bindValue(":id", fields[0]);
        insert.bindValue(":name", fields[1]);
        insert.bindValue(":description", fields[2]);
        insert.bindValue(":price", fields[3].toDouble());
        insert.bindValue(":stock", fields[4].toInt());
        if (!insert.exec()) qWarning() << "Insert failed;" << insert.lastError().text();
    }
}