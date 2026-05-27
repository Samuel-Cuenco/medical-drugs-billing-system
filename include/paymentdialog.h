#pragma once

#include <QDialog>
#include <QDialogButtonBox> // Include for QDialogButtonBox

QT_BEGIN_NAMESPACE
namespace Ui { class PaymentDialog; }
QT_END_NAMESPACE

class PaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(double originalTotal, QWidget *parent = nullptr);
    ~PaymentDialog();

    double getReceivedAmount() const;
    double getFinalTotal() const;
    double getDiscountAmount() const;
    QString getDiscountType() const;

private slots:
    void on_cashReceivedLineEdit_textChanged(const QString &arg1);
    void on_seniorPwdCheckBox_toggled(bool checked);

private:
    Ui::PaymentDialog *ui;
    double m_originalTotal;
    double m_finalTotal;
    double m_discountAmount;
    QString m_discountType;
    bool m_isSeniorPwdApplied;
    bool m_userEditedCashReceived; // New member to track if user manually edited cash received

    void updateTotals();
};