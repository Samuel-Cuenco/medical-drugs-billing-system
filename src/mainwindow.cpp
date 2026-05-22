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
    
    // add to left pane
    leftLayout->addWidget(leftTitle);
    leftLayout->addWidget(searchBox);
    leftLayout->addWidget(searchResults);

    // *right pane of main layout: cart
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QLabel* rightTitle = new QLabel("Customer Cart", central);
    cartList = new QListWidget(central);

    // add to right pane
    rightLayout->addWidget(rightTitle);
    rightLayout->addWidget(cartList);

    
    // *add panes to mainLayout
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);

    // csv path
    QString csvPath = "hospital_medicines.csv";
    if (!QFile::exists(csvPath)) {
        QString exeDir = QCoreApplication::applicationDirPath();
        QString p1 = exeDir + "/hospital_medicines.csv";
        QString p2 = exeDir + "/../hospital_medicines.csv";
        if (QFile::exists(p1)) csvPath = p1;
        else if (QFile::exists(p2)) csvPath = p2;
    }

    openDatabase(); // open sqlite
    importCsvToDatabase(csvPath); // import data from csv to sqlite

    // connect search functions
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::updateResults);
    connect(searchResults, &QListWidget::itemDoubleClicked, this, &MainWindow::addToCart);
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

void MainWindow::addToCart(QListWidgetItem* item) {
    if (!item) return;
    cartList->addItem(item->text());
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
        searchResults->addItem(item);
    }
}