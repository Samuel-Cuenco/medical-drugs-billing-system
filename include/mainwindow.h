#pragma once
#include <QMainWindow>
#include "databasemanager.h"
#include <QListWidgetItem>
#include <QPoint>

// *forward-declare the uic-generated class
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // *search box function declarations
    void updateResults(const QString &text);
    // *cart function declarations
    void addToCart(QListWidgetItem* item);
    void addSelectedSearchItem();
    void cartSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void searchSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void updateCartTotals();
    void removeFromCart();
    void clearCart();
    // *item quantity function declarations
    void increaseQuantity();
    void decreaseQuantity();
    void quantityChanged(int value);
    // *checkout
    void checkout();
    void updateStock();
    void toggleMaximize();
    // *navigation
    void showBillingPage();
    void showStatsPage();
    void importNewCsv();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;   // *replaces every individual widget pointer
    DatabaseManager *m_db;
    QPoint m_dragPosition;

    void updateCartItemDisplay(QListWidgetItem* item);
    void refreshStats();

    // Private helper methods for constructor decomposition
    void setupWindowProperties();
    void registerCustomTypes();
    void loadStylesheets();
    void initializeUIStates();
    void connectSignalsAndSlots();
    void initializeDatabase();
};
