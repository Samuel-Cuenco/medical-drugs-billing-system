#pragma once
#include <QMainWindow>
#include <QPushButton> // for creating buttons
#include <QLineEdit> // single line
#include <QListWidget> 
#include <QtSql/QSqlDatabase> // sql queries
#include <QListWidgetItem>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    //~MainWindow();

private slots:
    void updateResults(const QString &text); // auto update list
    void addToCart(QListWidgetItem* item);

private:
    QLineEdit* searchBox;
    QListWidget* searchResults;
    QListWidget* cartList;
    QSqlDatabase db;

    void openDatabase();
    void importCsvToDatabase(const QString &csvPath);
};