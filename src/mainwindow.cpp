#include "mainwindow.h" // import file mainwindow.h
#include <QVBoxLayout> // vertical box layout
#include <QHBoxLayout> // horizontal box layout
#include <QLabel> // put text
#include <QPushButton> // create buttons
#include <QSqlDatabase> // handle sql connections
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QListWidgetItem>
#include <QLineEdit> // single line
#include <QMessageBox>
#include <QWidget>
#include <QListWidget>
#include <QTextStream>
#include <QListWidgetItem>
#include <QDir>
#include <QCoreApplication>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Jupiter Drugstore"); // window title

    // *central widget
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    // *main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    central->setLayout(mainLayout); //sf
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(20);

    // *left pane of main layout: search + results
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QLabel* leftTitle = new QLabel("Search Medicines", central);
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

    // *right pane of main layout: cart
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QLabel* rightTitle = new QLabel("Customer Cart", central);
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

    // add to right pane
    rightLayout->addWidget(rightTitle);
    rightLayout->addWidget(cartList);
    rightLayout->addLayout(cartControls);
    
    // *add panes to mainLayout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

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
    openDatabase(); // open sqlite
    importCsvToDatabase(csvPath); // import data from csv to sqlite

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
};

void MainWindow::openDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("jupiter.db");

    if (!db.open()) {
        qWarning() << "Cannot open database:" << db.lastError().text();
        return;
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
}

static QStringList parseCsvLine(const QString &line) {
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

void MainWindow::importCsvToDatabase(const QString &csvPath) {
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

// Search Functions
void MainWindow::searchSelectionChanged(QListWidgetItem* current, QListWidgetItem*) {
    addButton->setEnabled(current != nullptr);
}

void MainWindow::addSelectedSearchItem() {
    QListWidgetItem* current = searchResults->currentItem();
    if (current)
        addToCart(current);
}

// Cart Functions
void MainWindow::updateCartItemDisplay(QListWidgetItem* item) {
    QString id = item->data(Qt::UserRole).toString();
    QString name = item->data(Qt::UserRole + 1).toString();
    double price = item->data(Qt::UserRole + 2).toDouble();
    int quantity = item->data(Qt::UserRole + 4).toInt();

    item->setText(QString("%1 x%2 — ₱%3 each — ID:%4")
        .arg(name)
        .arg(quantity)
        .arg(price, 0, 'f', 2)
        .arg(id));
}

void MainWindow::cartSelectionChanged(QListWidgetItem* current, QListWidgetItem* ) {
    if (!current) {
        quantityBox->setEnabled(false);
        plusButton->setEnabled(false);
        minusButton->setEnabled(false);
        return;
    }

    quantityBox->setEnabled(true);
    plusButton->setEnabled(true);
    minusButton->setEnabled(true);
    quantityBox->setValue(current->data(Qt::UserRole + 4).toInt());
}

void MainWindow::increaseQuantity() {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    int quantity = current->data(Qt::UserRole + 4).toInt() + 1;
    current->setData(Qt::UserRole + 4, quantity);
    updateCartItemDisplay(current);
    quantityBox->setValue(quantity);
}

void MainWindow::decreaseQuantity() {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    int quantity = current->data(Qt::UserRole + 4).toInt() - 1;
    if (quantity <= 0) {
        delete current;
        return;
    }
    current->setData(Qt::UserRole + 4, quantity);
    updateCartItemDisplay(current);
    quantityBox->setValue(quantity);
}

void MainWindow::quantityChanged(int value) {
    QListWidgetItem* current = cartList->currentItem();
    if (!current) return;
    current->setData(Qt::UserRole + 4, value);
    updateCartItemDisplay(current);
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

    if (existing) {
        int quantity = existing->data(Qt::UserRole + 4).toInt() + 1;
        existing->setData(Qt::UserRole + 4, quantity);
        updateCartItemDisplay(existing);
    } else {
        QListWidgetItem* cartItem = new QListWidgetItem;
        cartItem->setData(Qt::UserRole, id);
        cartItem->setData(Qt::UserRole + 1, name);
        cartItem->setData(Qt::UserRole + 2, price);
        cartItem->setData(Qt::UserRole + 4, 1);
        updateCartItemDisplay(cartItem);
        cartList->addItem(cartItem);
    }
    // cartList->addItem(item->text());
}

void MainWindow::updateResults(const QString &text) {
    searchResults->clear();
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) return;

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT item_id, item_name, description, retail_price, stock
        FROM products
        WHERE item_name LIKE :term1 
        OR description LIKE :term2 
        OR item_id LIKE :term3
        LIMIT 200
    )");
    QString match = "%" + trimmed + "%";
    query.bindValue(":term1", match);
    query.bindValue(":term2", match);
    query.bindValue(":term3", match);

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

        searchResults->addItem(item);
    }
}