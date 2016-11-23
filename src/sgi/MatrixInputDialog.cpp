#include "stdafx.h"
#include <sgi/plugins/SGIItemBase.h>
#include "MatrixInputDialog.h"
#include "MatrixInputDialog.moc"

#include <QPushButton>

#include <ui_MatrixInputDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

MatrixInputDialog::MatrixInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _original_value()
    , _value()
{
    ui = new Ui_MatrixInputDialog;
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
}

MatrixInputDialog::~MatrixInputDialog()
{
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
}

void MatrixInputDialog::setLabel(const QString & message)
{
    ui->label->setText(message);
}

QString MatrixInputDialog::label() const
{
    return ui->label->text();
}

void MatrixInputDialog::reset()
{
    setValue(_original_value);
}

void MatrixInputDialog::setOriginalValue(const Matrix & value)
{
    _original_value = value;
}

const Matrix & MatrixInputDialog::originalValue() const
{
    return _original_value;
}

void MatrixInputDialog::setValue(const Matrix & value)
{
    _value = value;
}

const Matrix & MatrixInputDialog::value() const
{
    return _value;
}

void MatrixInputDialog::setDecimals(int decimals)
{
}

int MatrixInputDialog::decimals() const
{
    return 0;
}

} // namespace sgi
