#include "stdafx.h"
#include <sgi/plugins/Quat>
#include "QuatInputDialog.h"

#include <QPushButton>

#include <ui_QuatInputDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

QuatInputDialog::QuatInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _original_value()
    , _value()
    , _slider_factor(2)
{
    ui = new Ui_QuatInputDialog;
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
}

QuatInputDialog::~QuatInputDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

void QuatInputDialog::setLabel(const QString & message)
{
    ui->label->setText(message);
}

QString QuatInputDialog::label() const
{
    return ui->label->text();
}

void QuatInputDialog::reset()
{
    setValue(_original_value);
}

void QuatInputDialog::setOriginalValue(const Quat & value)
{
    _original_value = value;
}

const Quat & QuatInputDialog::originalValue() const
{
    return _original_value;
}

void QuatInputDialog::setValue(const Quat & value)
{
    _value = value;
}

const Quat & QuatInputDialog::value() const
{
    return _value;
}

void QuatInputDialog::setDecimals(int decimals)
{
}

int QuatInputDialog::decimals() const
{
    return 0;
}

} // namespace sgi
