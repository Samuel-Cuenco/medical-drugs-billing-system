#include "logindialog.h"
#include "ui_logindialog.h"
#include <QSqlQuery>
#include <QSqlError>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::accept()
{
    QString user = ui->usernameEdit->text();
    QString pass = ui->passwordEdit->text();

    QSqlQuery query;
    query.prepare("SELECT id, username, role FROM users WHERE username = :u AND password = :p");
    query.bindValue(":u", user);
    query.bindValue(":p", pass);

    if (query.exec() && query.next()) {
        m_userId = query.value(0).toInt();
        m_username = query.value(1).toString();
        m_role = query.value(2).toString();
        QDialog::accept();
    } else {
        ui->errorLabel->setText("Invalid username or password");
        ui->passwordEdit->clear();
        ui->passwordEdit->setFocus();
    }
}