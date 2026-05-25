#pragma once
#include <QSqlDatabase>
#include <QList>
#include "models.h"

class DatabaseManager {
public:
    explicit DatabaseManager(const QString &path = "jupiter.db");
    ~DatabaseManager();

    bool open();
    void close();

    // Core CRUD and Search
    QList<Product> searchProducts(const QString &searchTerm);
    bool updateStock(const QString &productId, int soldQuantity);
    void importCsv(const QString &csvPath);

    // Statistics
    void getInventorySummary(int &totalProducts, double &totalValue);
    QList<Product> getLowStockItems(int threshold = 10);

private:
    QSqlDatabase m_db;
};