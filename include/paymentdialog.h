#pragma once

#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

class PaymentDialog : public QDialog {
    Q_OBJECT
public:
    explicit PaymentDialog(double total, QWidget *parent = nullptr);

    double getReceivedAmount() const { return m_amountSpin->value(); }
    bool isDiscountApplied() const { return m_discountCheck->isChecked(); }
    double getFinalTotal() const { return m_finalTotal; }
    double getDiscountAmount() const { return m_discountAmount; }

private slots:
    void calculateTotals();

private:
    double m_originalTotal;
    double m_finalTotal;
    double m_discountAmount;
    QLabel *m_infoLabel;
    QDoubleSpinBox *m_amountSpin;
    QCheckBox *m_discountCheck;
};