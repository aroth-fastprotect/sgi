#include "stdafx.h"
#include "DoubleInputDialog.h"

#include <QPushButton>

#include <ui_DoubleInputDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

DoubleInputDialog::DoubleInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _original_value(DBL_MAX)
    , _slider_factor(2)
{
    ui = new Ui_DoubleInputDialog;
    ui->setupUi( this );

    _slider_factor = pow10(ui->doubleSpinBox->decimals());

    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
}

DoubleInputDialog::~DoubleInputDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

void DoubleInputDialog::setLabel(const QString & message)
{
    ui->label->setText(message);
}

QString DoubleInputDialog::label() const
{
    return ui->label->text();
}

void DoubleInputDialog::reset()
{
    if(_original_value != DBL_MAX)
        setValue(_original_value);
}

void DoubleInputDialog::sliderChanged(int pos)
{
    ui->doubleSpinBox->blockSignals(true);
    double value = (double)pos / (double)_slider_factor;
    ui->doubleSpinBox->setValue(value);
    ui->doubleSpinBox->blockSignals(false);
}

void DoubleInputDialog::spinBoxChanged(double value)
{
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setValue(ceil(value * _slider_factor));
    ui->horizontalSlider->blockSignals(false);
}

void DoubleInputDialog::setOriginalValue(double value)
{
    _original_value = value;
}

double DoubleInputDialog::originalValue() const
{
    return _original_value;
}

void DoubleInputDialog::setValue(double value)
{
    if(_original_value == DBL_MAX)
        setOriginalValue(value);

    ui->doubleSpinBox->blockSignals(true);
    ui->horizontalSlider->blockSignals(true);
    ui->doubleSpinBox->setValue(value);
    ui->horizontalSlider->setValue(ceil(value * _slider_factor));

    ui->doubleSpinBox->blockSignals(false);
    ui->horizontalSlider->blockSignals(false);
}

double DoubleInputDialog::value() const
{
    return ui->doubleSpinBox->value();
}

void DoubleInputDialog::setRange(double minimum, double maximum)
{
    ui->doubleSpinBox->setMinimum(minimum);
    ui->doubleSpinBox->setMaximum(maximum);

    ui->horizontalSlider->setMaximum(ceil(maximum * _slider_factor));
    ui->horizontalSlider->setMinimum(ceil(minimum * _slider_factor));
}

void DoubleInputDialog::setDecimals(int decimals)
{
    ui->doubleSpinBox->setDecimals(decimals);
    _slider_factor = pow10(ui->doubleSpinBox->decimals());
    double maximum = ui->doubleSpinBox->maximum();
    double minimum = ui->doubleSpinBox->minimum();
    ui->horizontalSlider->setMaximum(ceil(maximum * _slider_factor));
    ui->horizontalSlider->setMinimum(ceil(minimum * _slider_factor));
}

int DoubleInputDialog::decimals() const
{
    return ui->doubleSpinBox->decimals();
}

} // namespace sgi
