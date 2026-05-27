#include "paymentdialog.h"
#include "ui_paymentdialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QPushButton> // Required for buttonBox->button()

PaymentDialog::PaymentDialog(double originalTotal, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaymentDialog),
    m_originalTotal(originalTotal),
    m_finalTotal(originalTotal),
    m_discountAmount(0.0),
    m_discountType("None"),
    m_isSeniorPwdApplied(false),
    m_userEditedCashReceived(false) // Initialize the new flag
{
    ui->setupUi(this);
    setWindowTitle("Payment");

    ui->originalTotalLabel->setText(QString("Original Total: ₱%1").arg(m_originalTotal, 0, 'f', 2));
    ui->finalTotalLabel->setText(QString("Final Total: ₱%1").arg(m_finalTotal, 0, 'f', 2));
    ui->discountLabel->setText("Discount: ₱0.00 (None)");
    ui->changeLabel->setText("Change: ₱0.00");

    // Allow only valid double input for cash received
    QDoubleValidator *validator = new QDoubleValidator(0.0, 9999999.99, 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->cashReceivedLineEdit->setValidator(validator);

    updateTotals(); // Initial calculation
    // Set initial focus to the cash received line edit
    ui->cashReceivedLineEdit->setFocus();
    ui->cashReceivedLineEdit->selectAll();
    // Shortcut to toggle the Senior/PWD discount
    ui->seniorPwdCheckBox->setShortcut(QKeySequence("Ctrl+D"));
    connect(ui->cashReceivedLineEdit, &QLineEdit::returnPressed, this, &PaymentDialog::accept);
}

PaymentDialog::~PaymentDialog()
{
    delete ui;
}

double PaymentDialog::getReceivedAmount() const
{
    return ui->cashReceivedLineEdit->text().toDouble();
}

double PaymentDialog::getFinalTotal() const
{
    return m_finalTotal;
}

double PaymentDialog::getDiscountAmount() const
{
    return m_discountAmount;
}

QString PaymentDialog::getDiscountType() const
{
    return m_discountType;
}

void PaymentDialog::updateTotals()
{
    double bulkDiscount = 0.0;
    double seniorDiscount = 0.0;

    // Check for Bulk discount (5%)
    if (m_originalTotal >= 10000.0) {
        bulkDiscount = m_originalTotal * 0.05;
    }

    // Check for Senior/PWD discount (20%)
    if (m_isSeniorPwdApplied) {
        seniorDiscount = m_originalTotal * 0.20;
    }

    m_discountAmount = bulkDiscount + seniorDiscount;

    if (bulkDiscount > 0 && seniorDiscount > 0)
        m_discountType = "Bulk (5%) + Senior/PWD (20%)";
    else if (seniorDiscount > 0)
        m_discountType = "Senior/PWD (20%)";
    else if (bulkDiscount > 0)
        m_discountType = "Bulk (5%)";
    else
        m_discountType = "None";

    m_finalTotal = m_originalTotal - m_discountAmount;

    ui->originalTotalLabel->setText(QString("Original Total: ₱%1").arg(m_originalTotal, 0, 'f', 2));
    ui->finalTotalLabel->setText(QString("Final Total: ₱%1").arg(m_finalTotal, 0, 'f', 2));
    ui->discountLabel->setText(QString("Discount: ₱%1 (%2)").arg(m_discountAmount, 0, 'f', 2).arg(m_discountType));

    // Only update cashReceivedLineEdit if the user hasn't manually edited it
    if (!m_userEditedCashReceived) {
        ui->cashReceivedLineEdit->blockSignals(true);
        ui->cashReceivedLineEdit->setText(QString::number(m_finalTotal, 'f', 2));
        ui->cashReceivedLineEdit->blockSignals(false);
    }

    double received = ui->cashReceivedLineEdit->text().toDouble();
    double change = received - m_finalTotal;
    ui->changeLabel->setText(QString("Change: ₱%1").arg(change, 0, 'f', 2));

    // Enable/disable OK button based on sufficient payment
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(received >= m_finalTotal);
}

void PaymentDialog::on_cashReceivedLineEdit_textChanged(const QString &arg1)
{
    // If the field is cleared, we allow the system to auto-calculate again
    m_userEditedCashReceived = !arg1.isEmpty(); 
    updateTotals();
}

void PaymentDialog::on_seniorPwdCheckBox_toggled(bool checked)
{
    m_isSeniorPwdApplied = checked;
    updateTotals();
    ui->cashReceivedLineEdit->setFocus();
    ui->cashReceivedLineEdit->selectAll();
}