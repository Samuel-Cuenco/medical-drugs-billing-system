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
    if (!m_db.open()) {
        qCritical() << "Error: Could not open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS products (
            item_id TEXT PRIMARY KEY,
            item_name TEXT NOT NULL,
            description TEXT,
            retail_price REAL,
            stock INTEGER
        )
    )")) {
        qCritical() << "Error creating products table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            total_amount REAL,
            received REAL,
            change_amount REAL
        )
    )")) return false;

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS transaction_items (
            transaction_id INTEGER,
            product_id TEXT,
            quantity INTEGER,
            price REAL
        )
    )")) return false;

    return true;
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

    if (!query.exec()) {
        qWarning() << "Search products query failed:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        results.append({
            query.value(0).toString(), query.value(1).toString(),
            query.value(2).toString(), query.value(3).toDouble(),
            query.value(4).toInt()
        });
    }
    return results;
}

bool DatabaseManager::updateStock(const QString &productId, int soldQuantity) {
    QSqlQuery query;
    query.prepare("UPDATE products SET stock = stock - :sold WHERE item_id = :id");
    query.bindValue(":sold", soldQuantity);
    query.bindValue(":id", productId);
    if (!query.exec()) {
        qWarning() << "Update stock query failed:" << query.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::importCsv(const QString &csvPath) {
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error: Could not open CSV file for import:" << csvPath;
        return;
    }

    m_db.transaction(); // start transaction for speed and safety

    QSqlQuery query(m_db);
    if (!query.exec("DELETE FROM products")) {
        qWarning() << "Error clearing products table before import:" << query.lastError().text();
        m_db.rollback();
        return;
    }

    QTextStream in(&file);
    bool firstLine = true;

    QSqlQuery insert(m_db);
    insert.prepare("INSERT INTO products VALUES (:id, :name, :desc, :price, :stock)");

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (firstLine) { firstLine = false; continue; }

        QStringList fields = Helpers::parseCsvLine(line);
        if (fields.size() < 5) {
            qWarning() << "Skipping malformed CSV line (less than 5 fields):" << line;
            continue;
        }

        // Basic validation for numeric fields
        bool priceOk, stockOk;
        double price = fields[3].toDouble(&priceOk);
        int stock = fields[4].toInt(&stockOk);

        if (!priceOk || !stockOk) {
            qWarning() << "Skipping CSV line due to invalid numeric data (price or stock):" << line;
            continue;
        }

        insert.bindValue(":id", fields[0]);
        insert.bindValue(":name", fields[1]);
        insert.bindValue(":desc", fields[2]);
        insert.bindValue(":price", price);
        insert.bindValue(":stock", stock);
        if (!insert.exec()) {
            qWarning() << "Import error for line:" << line << "Error:" << insert.lastError().text();
            // Continue importing other lines, but log the error.
            // Or, if a single error should fail the whole import, rollback here.
            // For now, let's continue and log.
        }
    }
    m_db.commit();
    file.close(); // Ensure file is closed
}

void DatabaseManager::getInventorySummary(int &totalProducts, double &totalValue) {
    totalProducts = 0;
    totalValue = 0.0;
    QSqlQuery query("SELECT COUNT(*), SUM(retail_price * stock) FROM products");
    if (!query.exec()) {
        qWarning() << "Get inventory summary query failed:" << query.lastError().text();
        return;
    }
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
    if (!query.exec()) {
        qWarning() << "Get low stock items query failed:" << query.lastError().text();
        return results;
    }
    while (query.next()) {
        results.append({query.value(0).toString(),
                        query.value(1).toString(),
                        query.value(2).toString(),
                        query.value(3).toDouble(),
                        query.value(4).toInt()});
    }
    return results;
}

bool DatabaseManager::isDatabaseEmpty() const {
    QSqlQuery query(m_db); // use m_db directly for const method
    if (!query.exec("SELECT COUNT(*) FROM products")) {
        qWarning() << "Failed to check if database is empty:" << query.lastError().text();
        return true; // err on the side of caution, assume empty or unreadable
    }
    query.next();
    return query.value(0).toInt() == 0;
}