#include "mainwindow.h" // import file mainwindow.h
#include "helpers.h" // helper functions in here
#include <QVBoxLayout> // vertical box layout
#include <QHBoxLayout> // horizontal box layout
#include <QCoreApplication>
#include <QWidget> // can hold other widgets
#include <QLabel> // put text
#include <QPushButton> // create buttons
#include <QSqlDatabase> // handle sql connections
#include <QSqlQuery> // use sql commands
#include <QSqlError> // catches sql errors
#include <QFile> // reading and writing files - better compatibility for qt afaik compared to cpp file stream
#include <QTextStream> // format and read human text
#include <QListWidget> // create container for items
#include <QListWidgetItem> // put items into listwidget
#include <QLineEdit> // single line entry, easier to understand that it takes single line of text
#include <QMessageBox> // send a pop up dialog box
#include <QDir> // navigate file system
#include <QSpinBox> // an input box for numbers specifically
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Jupiter Drugstore"); // window title
    this->setWindowFlags(Qt::FramelessWindowHint); // remove title bar

    // *load ui
    Helpers::loadUI(this);

    // *central widget
    QWidget* central = new QWidget(this);
    central->setObjectName("central"); // this must match the .central selector in your css file
    
    // *outer vertical layout to hold title bar and content
    QVBoxLayout* outerLayout = new QVBoxLayout(central);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // *custom title bar
    QWidget* titleBar = new QWidget(central);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(40);

    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(15, 0, 10, 0);
    
    QLabel* titleLabel = new QLabel("Jupiter Drugstore", titleBar);
    titleLabel->setObjectName("titleLabel");
    
    QPushButton* minBtn = new QPushButton("—", titleBar);
    minBtn->setObjectName("minBtn");
    minBtn->setFixedSize(30, 30);
    
    maxBtn = new QPushButton("▢", titleBar);
    maxBtn->setObjectName("maxBtn");
    maxBtn->setFixedSize(30, 30);

    QPushButton* closeBtn = new QPushButton("✕", titleBar);
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(30, 30);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minBtn);
    titleLayout->addWidget(maxBtn);
    titleLayout->addWidget(closeBtn);
    
    outerLayout->addWidget(titleBar);

    // * navigation menu
    QWidget* navBar = new QWidget(central);
    navBar->setObjectName("navBar");
    navBar->setFixedHeight(50);
    QHBoxLayout* navLayout = new QHBoxLayout(navBar);
    navLayout->setContentsMargins(10, 0, 10, 0);
    navLayout->setSpacing(5);

    QPushButton* billingNavBtn = new QPushButton("Billing", navBar);
    billingNavBtn->setObjectName("navBtn");
    QPushButton* statsNavBtn = new QPushButton("Statistics", navBar);
    statsNavBtn->setObjectName("navBtn");
    importBtn = new QPushButton("Import Stock", navBar);
    importBtn->setObjectName("navBtn");

    navLayout->addWidget(billingNavBtn);
    navLayout->addWidget(statsNavBtn);
    navLayout->addStretch();
    navLayout->addWidget(importBtn);
    outerLayout->addWidget(navBar);

    // * Stacked Widget for Pages
    stackedWidget = new QStackedWidget(central);
    billingPage = new QWidget(stackedWidget);
    statsPage = new QWidget(stackedWidget);
    stackedWidget->addWidget(billingPage);
    stackedWidget->addWidget(statsPage);
    outerLayout->addWidget(stackedWidget);

    // * Setup Stats Page UI
    QVBoxLayout* statsLayout = new QVBoxLayout(statsPage);
    statsLayout->setContentsMargins(30, 30, 30, 30);
    statsLayout->setSpacing(20);

    QLabel* statsHeader = new QLabel("Inventory Dashboard", statsPage);
    statsHeader->setStyleSheet("font-size: 22px; font-weight: bold; color: #2c3e50;");
    
    // Summary Cards (Horizontal)
    QHBoxLayout* summaryCards = new QHBoxLayout();
    auto createCard = [&](const QString& title, QLabel*& valueLabel) {
        QFrame* card = new QFrame(statsPage);
        card->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
        card->setStyleSheet("QFrame { background: white; border-radius: 10px; border: 1px solid #ddd; padding: 15px; }");
        QVBoxLayout* l = new QVBoxLayout(card);
        QLabel* t = new QLabel(title, card);
        t->setStyleSheet("font-size: 14px; color: #7f8c8d; border: none;");
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2e7d32; border: none;");
        l->addWidget(t);
        l->addWidget(valueLabel);
        return card;
    };

    summaryCards->addWidget(createCard("Unique Products", totalProductsLabel));
    summaryCards->addWidget(createCard("Total Inventory Value", totalValueLabel));

    QLabel* lowStockTitle = new QLabel("Low Stock Alerts (Items < 10)", statsPage);
    lowStockTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #c0392b; margin-top: 20px;");

    lowStockList = new QListWidget(statsPage);
    lowStockList->setObjectName("lowStockList");

    statsLayout->addWidget(statsHeader);
    statsLayout->addLayout(summaryCards);
    statsLayout->addWidget(lowStockTitle);
    statsLayout->addWidget(lowStockList);

    // central widget
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(20);

    // *left pane of main layout containing search and results
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QLabel* leftTitle = new QLabel("Search Medicines", central);
    leftTitle->setObjectName("leftTitle");
    searchBox = new QLineEdit(central);
    searchBox->setPlaceholderText("Search Medicines...");
    searchResults = new QListWidget(central);
    addButton = new QPushButton("Add to Cart", central);
    addButton->setEnabled(false);
    
    // add to left pane
    leftLayout->addWidget(leftTitle);
    leftLayout->addWidget(searchBox);
    leftLayout->addWidget(searchResults);
    leftLayout->addWidget(addButton);

    // right pane of main layout: cart
    // header
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QLabel* rightTitle = new QLabel("Customer Cart", central);
    rightTitle->setObjectName("rightTitle");
    
    // cart layout
    cartList = new QListWidget(central);
    plusButton = new QPushButton("+", central);
    minusButton = new QPushButton("-", central);
    quantityBox = new QSpinBox(central);
    quantityBox->setRange(1, 999);
    quantityBox->setValue(1);
    QHBoxLayout* cartControls = new QHBoxLayout;
    cartControls->addWidget(minusButton);
    cartControls->addWidget(quantityBox);
    cartControls->addWidget(plusButton);

    // total pane
    totalLabel = new QLabel("Total: P0.00", central);
    removeButton = new QPushButton("Remove", central);
    removeButton->setEnabled(false);
    checkoutButton = new QPushButton("Checkout", central);
    clearCartButton = new QPushButton("Clear Cart", central);
    QHBoxLayout* checkoutLayout = new QHBoxLayout;
    checkoutLayout->addWidget(totalLabel);
    checkoutLayout->addWidget(clearCartButton);
    checkoutLayout->addWidget(removeButton);
    checkoutLayout->addWidget(checkoutButton);

    // add to right pane
    rightLayout->addWidget(rightTitle);
    rightLayout->addWidget(cartList);
    rightLayout->addLayout(cartControls);
    rightLayout->addLayout(checkoutLayout);
    
    // *add panes to mainLayout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);
    billingPage->setLayout(mainLayout);

    // * set csv path
    QString csvPath = "hospital_medicines.csv";
    if (!QFile::exists(csvPath)) {
        QString exeDir = QCoreApplication::applicationDirPath();
        QString p1 = exeDir + "/hospital_medicines.csv";
        QString p2 = exeDir + "/../hospital_medicines.csv";
        if (QFile::exists(p1)) csvPath = p1;
        else if (QFile::exists(p2)) csvPath = p2;
    }

    // * prepare database
    db = Helpers::openDatabase(); // Initialize the MainWindow member 'db'
    Helpers::importCsvToDatabase(csvPath, db); // import data from csv to sqlite

    // *connections
    // connect search functions
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::updateResults);
    connect(searchResults, &QListWidget::itemDoubleClicked, this, &MainWindow::addToCart);
    connect(searchResults, &QListWidget::currentItemChanged, this, &MainWindow::searchSelectionChanged);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addSelectedSearchItem);

    // connect cart functions
    connect(cartList, &QListWidget::currentItemChanged, this, &MainWindow::cartSelectionChanged);
    connect(plusButton, &QPushButton::clicked, this, &MainWindow::increaseQuantity);
    connect(minusButton, &QPushButton::clicked, this, &MainWindow::decreaseQuantity);
    connect(quantityBox, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::quantityChanged);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeFromCart);
    connect(clearCartButton, &QPushButton::clicked, this, &MainWindow::clearCart);

    // checkout
    connect(checkoutButton, &QPushButton::clicked, this, &MainWindow::checkout);

    // nav connections
    connect(billingNavBtn, &QPushButton::clicked, this, &MainWindow::showBillingPage);
    connect(statsNavBtn, &QPushButton::clicked, this, &MainWindow::showStatsPage);
    connect(importBtn, &QPushButton::clicked, this, &MainWindow::importNewCsv);

    // title bar connections
    connect(minBtn, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(maxBtn, &QPushButton::clicked, this, &MainWindow::toggleMaximize);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::close);

    // load initial data immediately
    updateResults("");
};

void MainWindow::mousePressEvent(QMouseEvent *event) {
    // only allow dragging from the title bar area (height 40)
    if (event->button() == Qt::LeftButton && event->position().y() < 40) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (isMaximized()) {
            // if dragging while maximized, restore first
            toggleMaximize();
            return;
        }
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    // maximize/Restore when double clicking the title bar
    if (event->button() == Qt::LeftButton && event->position().y() < 40) {
        toggleMaximize();
    }
}

void MainWindow::toggleMaximize() {
    if (isMaximized()) {
        showNormal();
        maxBtn->setText("▢");
        // add border back when not maximized
        centralWidget()->setStyleSheet("#central { border: 1px solid #dcdcdc; }");
    } else {
        showMaximized();
        maxBtn->setText("❐");
        // remove border when maximized for a "fit to window" look
        centralWidget()->setStyleSheet("#central { border: none; }");
    }
}

// search functions definitions
void MainWindow::searchSelectionChanged(QListWidgetItem* current, QListWidgetItem*) {
    addButton->setEnabled(current != nullptr);
}

void MainWindow::addSelectedSearchItem() {
    QListWidgetItem* current = searchResults->currentItem();
    if (current)
        addToCart(current);
}

void MainWindow::addToCart(QListWidgetItem* item) {
    if (!item) return;

    QString id = item->data(Qt::UserRole).toString();
    QString name = item->data(Qt::UserRole + 1).toString();
    double price = item->data(Qt::UserRole + 2).toDouble();

    QListWidgetItem* existing = nullptr;
    for (int i=0; i < cartList->count(); ++i) {
        QListWidgetItem* it = cartList->item(i);
        if (it->data(Qt::UserRole).toString() == id) {
            existing = it;
            break;
        }
    }

    int stock = item->data(Qt::UserRole + 4).toInt();
    // prevent adding items that are out of stock
    if (stock <= 0) {
        QMessageBox::warning(this, "Out of stock", "This item is out of stock and cannot be added to the cart.");
        return;
    }

    if (existing) {
        int quantity = existing->data(Qt::UserRole + 5).toInt() + 1;
        if (quantity > stock) {
            QMessageBox::warning(this, "Stock limit", QString("Only %1 unit(s) available.").arg(stock));
            return;
        }
        existing->setData(Qt::UserRole + 5, quantity);
        updateCartItemDisplay(existing);
    } else {
        QListWidgetItem* cartItem = new QListWidgetItem;
        cartItem->setData(Qt::UserRole, id);
        cartItem->setData(Qt::UserRole + 1, name);
        cartItem->setData(Qt::UserRole + 2, price);
        cartItem->setData(Qt::UserRole + 4, stock);
        cartItem->setData(Qt::UserRole + 5, 1);
        updateCartItemDisplay(cartItem);
        cartList->addItem(cartItem);
    }
    updateCartTotals();
    // cartList->addItem(item->text());
}

void MainWindow::updateResults(const QString &text) {
    searchResults->clear();
    QString trimmed = text.trimmed();

    QSqlQuery query(db);
    if (trimmed.isEmpty()) {
        // if search is empty, show all items (limited to 200)
        query.prepare(R"(
            SELECT item_id, item_name, description, retail_price, stock
            FROM products
            LIMIT 999
        )");
    } else {
        // if user typed something, filter the results
        query.prepare(R"(
            SELECT item_id, item_name, description, retail_price, stock
            FROM products
            WHERE item_name LIKE :term1 
            OR description LIKE :term2 
            OR item_id LIKE :term3
            LIMIT 999
        )");
        QString match = "%" + trimmed + "%";
        query.bindValue(":term1", match);
        query.bindValue(":term2", match);
        query.bindValue(":term3", match);
    }

    if (!query.exec()) {
        qWarning() << "Search query failed:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString desc = query.value(2).toString();
        double price = query.value(3).toDouble();
        int stock = query.value(4).toInt();

        QString itemText = QString("%1 | %2 | %3 | P%4 | stock:%5")
            .arg(id, name, desc)
            .arg(price, 0, 'f', 2)
            .arg(stock);
        
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole + 1, name);
        item->setData(Qt::UserRole + 2, price);
        item->setData(Qt::UserRole + 3, desc);
        item->setData(Qt::UserRole + 4, stock);

        searchResults->addItem(item);
    }
}

// cart functions definitions
void MainWindow::updateCartItemDisplay(QListWidgetItem* item) {
    QString id = item->data(Qt::UserRole).toString();
    QString name = item->data(Qt::UserRole + 1).toString();
    double price = item->data(Qt::UserRole + 2).toDouble();
    int quantity = item->data(Qt::UserRole + 5).toInt();
    int stock = item->data(Qt::UserRole + 4).toInt();

    item->setText(QString("%1 x%2 — ₱%3 each — stock:%4 — ID:%5")
        .arg(name)
        .arg(quantity)
        .arg(price, 0, 'f', 2)
        .arg(stock)
        .arg(id));
}

void MainWindow::cartSelectionChanged(QListWidgetItem* current, QListWidgetItem* ) {
    if (!current) {
        quantityBox->setEnabled(false);
        plusButton->setEnabled(false);
        minusButton->setEnabled(false);
        removeButton->setEnabled(false);
        return;
    }

    quantityBox->setEnabled(true);
    plusButton->setEnabled(true);
    minusButton->setEnabled(true);
    removeButton->setEnabled(true);
    quantityBox->setValue(current->data(Qt::UserRole + 5).toInt());
}

void MainWindow::increaseQuantity() {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    int stock = current->data(Qt::UserRole + 4).toInt();
    int quantity = current->data(Qt::UserRole + 5).toInt() + 1;
    if (quantity > stock) {
        QMessageBox::warning(this, "Stock limit", QString("Only %1 unit(s) available.").arg(stock));
        return;
    }
    current->setData(Qt::UserRole + 5, quantity);
    updateCartItemDisplay(current);
    quantityBox->setValue(quantity);
    updateCartTotals();
}

void MainWindow::decreaseQuantity() {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    int quantity = current->data(Qt::UserRole + 5).toInt() - 1;
    if (quantity <= 0) {
        delete current;
        updateCartTotals();
        return;
    }
    current->setData(Qt::UserRole + 5, quantity);
    updateCartItemDisplay(current);
    quantityBox->setValue(quantity);
    updateCartTotals();
}

void MainWindow::quantityChanged(int value) {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    int stock = current->data(Qt::UserRole + 4).toInt();
    if (value > stock) {
        QMessageBox::warning(this, "Stock limit", QString("Only %1 unit(s) available.").arg(stock));
        quantityBox->setValue(stock);
        value = stock;
    }
    current->setData(Qt::UserRole + 5, value);
    updateCartItemDisplay(current);
    updateCartTotals();
}

void MainWindow::updateCartTotals() {
    double total = 0.0;
    for (int i = 0; i < cartList->count(); ++i) {
        QListWidgetItem* item = cartList->item(i);
        double price = item->data(Qt::UserRole + 2).toDouble();
        int quantity = item->data(Qt::UserRole + 5).toInt();
        total += price * quantity;
    }
    totalLabel->setText(QString("Total: ₱%1").arg(total, 0, 'f', 2));
}

void MainWindow::removeFromCart() {
    QListWidgetItem* current = cartList->currentItem();
    if (current) {
        delete current;
        updateCartTotals();
    }
}

void MainWindow::clearCart() {
    cartList->clear();
    updateCartTotals();
    quantityBox->setEnabled(false);
    plusButton->setEnabled(false);
    minusButton->setEnabled(false);
    removeButton->setEnabled(false);
}

// checkout function definitions
void MainWindow::checkout() {
    if (cartList->count() == 0) {
        QMessageBox::information(this, "Checkout", "Cart is empty.");
        return;
    }

    double total = 0.0;
    for (int i = 0; i < cartList->count(); ++i) {
        QListWidgetItem* item = cartList->item(i);
        total += item->data(Qt::UserRole + 2).toDouble() *
                 item->data(Qt::UserRole + 5).toInt();
    }

    QMessageBox::information(this, "Checkout",
        QString("Total due: ₱%1\nSale complete.")
            .arg(total, 0, 'f', 2));

    updateStock();
    cartList->clear();
    updateCartTotals();
    quantityBox->setEnabled(false);
    plusButton->setEnabled(false);
    minusButton->setEnabled(false);
    removeButton->setEnabled(false);
    updateResults(searchBox->text());
}

void MainWindow::updateStock() {
    QSqlQuery update(db);
    update.prepare("UPDATE products SET stock = stock - :sold WHERE item_id = :id");

    for (int i = 0; i < cartList->count(); ++i) {
        QListWidgetItem* item = cartList->item(i);
        QString id = item->data(Qt::UserRole).toString();
        int quantity = item->data(Qt::UserRole + 5).toInt();
        update.bindValue(":sold", quantity);
        update.bindValue(":id", id);
        if (!update.exec()) {
            qWarning() << "Failed to update stock:" << update.lastError().text();
        }
    }
}

void MainWindow::showBillingPage() {
    stackedWidget->setCurrentWidget(billingPage);
}

void MainWindow::showStatsPage() {
    refreshStats();
    stackedWidget->setCurrentWidget(statsPage);
}

void MainWindow::refreshStats() {
    QSqlQuery query(db);
    
    // 1. Get totals
    if (query.exec("SELECT COUNT(*), SUM(retail_price * stock) FROM products")) {
        if (query.next()) {
            totalProductsLabel->setText(query.value(0).toString());
            double totalVal = query.value(1).toDouble();
            totalValueLabel->setText(QString("₱%1").arg(totalVal, 0, 'f', 2));
        }
    }

    // 2. Get low stock items
    lowStockList->clear();
    query.prepare("SELECT item_name, stock, item_id FROM products WHERE stock < 10 ORDER BY stock ASC");
    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();
            int stock = query.value(1).toInt();
            QString id = query.value(2).toString();
            
            QListWidgetItem* item = new QListWidgetItem(
                QString("⚠️ %1 (ID: %2) - Only %3 left").arg(name, id).arg(stock)
            );
            if (stock == 0) item->setForeground(Qt::red);
            lowStockList->addItem(item);
        }
    }
}

void MainWindow::importNewCsv() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Medicines", "", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        Helpers::importCsvToDatabase(fileName, db);
        updateResults(searchBox->text());
        
        // Refresh stats if we are currently viewing the statistics page
        if (stackedWidget->currentWidget() == statsPage) {
            refreshStats();
        }

        QMessageBox::information(this, "Success", "Database updated successfully.");
    }
}
