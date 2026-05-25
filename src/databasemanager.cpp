#include "databasemanager.h"
#include "helpers.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString &path) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    if (!m_db.open()) return false;

    QSqlQuery query;
    return query.exec(R"(
        CREATE TABLE IF NOT EXISTS products (
            item_id TEXT PRIMARY KEY,
            item_name TEXT NOT NULL,
            description TEXT,
            retail_price REAL,
            stock INTEGER
        )
    )");
}

void DatabaseManager::close() {
    m_db.close();
}

QList<Product> DatabaseManager::searchProducts(const QString &searchTerm) {
    QList<Product> results;
    QSqlQuery query;
    
    if (searchTerm.isEmpty()) {
        query.prepare("SELECT item_id, item_name, description, retail_price, stock FROM products LIMIT 999");
    } else {
        query.prepare("SELECT item_id, item_name, description, retail_price, stock FROM products "
                      "WHERE item_name LIKE :t OR description LIKE :t OR item_id LIKE :t LIMIT 999");
        query.bindValue(":t", "%" + searchTerm + "%");
    }

    if (query.exec()) {
        while (query.next()) {
            results.append({
                query.value(0).toString(), query.value(1).toString(),
                query.value(2).toString(), query.value(3).toDouble(),
                query.value(4).toInt()
            });
        }
    }
    return results;
}

bool DatabaseManager::updateStock(const QString &productId, int soldQuantity) {
    QSqlQuery query;
    query.prepare("UPDATE products SET stock = stock - :sold WHERE item_id = :id");
    query.bindValue(":sold", soldQuantity);
    query.bindValue(":id", productId);
    return query.exec();
}

void DatabaseManager::importCsv(const QString &csvPath) {
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    m_db.transaction(); // Start transaction for speed and safety
    
    QSqlQuery query(m_db);
    query.exec("DELETE FROM products");
    
    QTextStream in(&file);
    bool firstLine = true;
    
    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO products VALUES (:id, :name, :desc, :price, :stock)");

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (firstLine) { firstLine = false; continue; }
        
        QStringList fields = Helpers::parseCsvLine(line);
        if (fields.size() < 5) continue;

        insert.bindValue(":id", fields[0]);
        insert.bindValue(":name", fields[1]);
        insert.bindValue(":desc", fields[2]);
        insert.bindValue(":price", fields[3].toDouble());
        insert.bindValue(":stock", fields[4].toInt());
        if (!insert.exec()) {
            qWarning() << "Import error:" << insert.lastError().text();
        }
    }
    m_db.commit();
}

void DatabaseManager::getInventorySummary(int &totalProducts, double &totalValue) {
    QSqlQuery query("SELECT COUNT(*), SUM(retail_price * stock) FROM products");
    if (query.next()) {
        totalProducts = query.value(0).toInt();
        totalValue = query.value(1).toDouble();
    }
}

QList<Product> DatabaseManager::getLowStockItems(int threshold) {
    QList<Product> results;
    QSqlQuery query;
    query.prepare("SELECT item_id, item_name, description, retail_price, stock FROM products WHERE stock < :limit ORDER BY stock ASC");
    query.bindValue(":limit", threshold);
    if (query.exec()) {
        while (query.next()) {
            results.append({query.value(0).toString(), 
                            query.value(1).toString(), 
                            query.value(2).toString(), 
                            query.value(3).toDouble(), 
                            query.value(4).toInt()});
        }
    }
    return results;
}