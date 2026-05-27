#include "paymentdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeySequence>

PaymentDialog::PaymentDialog(double total, QWidget *parent) 
    : QDialog(parent), m_originalTotal(total) {
    setWindowTitle("Payment Processing");
    setFixedWidth(350);

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
    
    m_amountSpin = new QDoubleSpinBox(this);
    m_amountSpin->setRange(0, 1000000);
    m_amountSpin->setDecimals(2);
    m_amountSpin->setPrefix("₱");
    m_amountSpin->setFixedHeight(35);
    m_amountSpin->setStyleSheet("font-size: 16px;");

    m_discountCheck = new QCheckBox("Apply Senior/PWD Discount (20%)", this);
    m_discountCheck->setShortcut(QKeySequence("Alt+D")); // Shortcut key
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("Confirm Sale", this);
    QPushButton *cancelBtn = new QPushButton("Cancel", this);
    okBtn->setDefault(true);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);

    layout->addWidget(new QLabel("Transaction Summary:"));
    layout->addWidget(m_infoLabel);
    layout->addWidget(new QLabel("Amount Tendered:"));
    layout->addWidget(m_amountSpin);
    layout->addWidget(m_discountCheck);
    layout->addSpacing(10);
    layout->addLayout(btnLayout);

    connect(m_discountCheck, &QCheckBox::toggled, this, &PaymentDialog::calculateTotals);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    calculateTotals();
    m_amountSpin->setFocus();
    m_amountSpin->selectAll();
}

void PaymentDialog::calculateTotals() {
    if (m_discountCheck->isChecked()) {
        m_discountAmount = m_originalTotal * 0.20;
        m_finalTotal = m_originalTotal - m_discountAmount;
    } else {
        m_discountAmount = 0.0;
        m_finalTotal = m_originalTotal;
    }

    m_infoLabel->setText(QString("Total Due: ₱%1").arg(m_finalTotal, 0, 'f', 2));
    if (m_amountSpin->value() < m_finalTotal) {
        m_amountSpin->setValue(m_finalTotal);
    }
}