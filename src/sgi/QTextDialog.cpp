#include "stdafx.h"
#include "QTextDialog.h"

#include <ui_QTextDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

QTextDialog::QTextDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
	, ui(NULL)
{
	ui = new Ui_QTextDialog;
    ui->setupUi( this );
}

QTextDialog::~QTextDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
}

void QTextDialog::setButtons(QDialogButtonBox::StandardButtons buttons)
{
	ui->buttonBox->setStandardButtons(buttons);
}

void QTextDialog::setLabel(const QString & message)
{
	ui->label->setText(message);
}
void QTextDialog::setText(const QString & text)
{
	ui->textEdit->setText(text);
}

QString QTextDialog::label() const
{
	return ui->label->text();
}

QString QTextDialog::text() const
{
	return ui->textEdit->toPlainText();
}

void QTextDialog::setReadOnly(bool readOnly)
{
	ui->textEdit->setReadOnly(readOnly);
}

bool QTextDialog::isReadOnly() const
{
	return ui->textEdit->isReadOnly();
}

} // namespace sgi
