#pragma once
#include <QMainWindow>
#include <QPushButton> // for creating buttons
#include <QLineEdit> // single line
#include <QListWidget> 
#include <QtSql/QSqlDatabase> // sql queries
#include <QListWidgetItem>
#include <QSpinBox>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>

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
    void removeFromCart();
    void clearCart();
    // item quantity function declarations
    void increaseQuantity();
    void decreaseQuantity();
    void quantityChanged(int value);
    // checkout
    void checkout();
    void updateStock();
    void toggleMaximize();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

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
    QPushButton* removeButton;
    QPushButton* maxBtn;
    // checkout
    QPushButton* checkoutButton;
    QPushButton* clearCartButton;
    QLabel* totalLabel;
    // db
    QSqlDatabase db;
    
    QPoint m_dragPosition;

    void openDatabase();
    void importCsvToDatabase(const QString &csvPath);
    void updateCartItemDisplay(QListWidgetItem* item);
};