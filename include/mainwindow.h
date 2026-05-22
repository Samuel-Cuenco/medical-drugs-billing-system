#pragma once
#include <QMainWindow>
#include <QPushButton> // for creating buttons
#include <QLineEdit> // single line
#include <QListWidget> 
#include <QtSql/QSqlDatabase> // sql queries
#include <QListWidgetItem>
#include <QSpinBox>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    //~MainWindow();

private slots:
    // search box function declarations
    void updateResults(const QString &text); // auto update list
    // cart function declarations
    void addToCart(QListWidgetItem* item);
    void addSelectedSearchItem();
    void cartSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void searchSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void updateCartTotals();
    void clearCart();
    // item quantity function declarations
    void increaseQuantity();
    void decreaseQuantity();
    void quantityChanged(int value);
    // checkout
    void checkout();
    void updateStock();

private:
    // search
    QLineEdit* searchBox;
    QListWidget* searchResults;
    // cart
    QListWidget* cartList;
    QPushButton* addButton;
    QPushButton* plusButton;
    QPushButton* minusButton;
    QSpinBox* quantityBox;
    // checkout
    QPushButton* checkoutButton;
    QPushButton* clearCartButton;
    QLabel* totalLabel;
    // db
    QSqlDatabase db;
    
    void openDatabase();
    void importCsvToDatabase(const QString &csvPath);
    void updateCartItemDisplay(QListWidgetItem* item);
};