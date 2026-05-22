#pragma once

#include <QWidget>
#include <QSqlDatabase>

class Helpers {
public:
    static void loadUI(QWidget* widget);
    static QSqlDatabase openDatabase();
    static QStringList parseCsvLine(const QString &line);
    static void importCsvToDatabase(const QString &csvPath, QSqlDatabase db);
};