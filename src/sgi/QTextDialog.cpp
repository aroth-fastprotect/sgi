#include "stdafx.h"
#include "QTextDialog.h"
#include "QTextDialog.moc"

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
void QTextDialog::setText(const QString & text, TextMode mode)
{
    switch (mode)
    {
    case TextModePlain:
        ui->textEdit->setAcceptRichText(false);
        ui->textEdit->setPlainText(text);
        break;
    case TextModeHTML:
        ui->textEdit->setAcceptRichText(true);
        ui->textEdit->setHtml(text);
        break;
    }
}

QString QTextDialog::label() const
{
	return ui->label->text();
}

QString QTextDialog::text(TextMode mode) const
{
    switch (mode)
    {
    case TextModePlain:
        return ui->textEdit->toPlainText();
    case TextModeHTML:
        return ui->textEdit->toHtml();
    }
    return QString();
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
