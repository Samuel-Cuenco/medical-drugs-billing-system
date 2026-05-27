#pragma once

#include <QDialog>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    int loggedInUserId() const { return m_userId; }
    QString loggedInUsername() const { return m_username; }
    QString loggedInRole() const { return m_role; }

    void accept() override;

private:
    Ui::LoginDialog *ui;
    int m_userId = -1;
    QString m_username;
    QString m_role;
};