#pragma once

#include <QDialog>
#include <QDialogButtonBox>

QT_BEGIN_NAMESPACE
class Ui_QTextDialog;
QT_END_NAMESPACE

namespace sgi {

class QTextDialog : public QDialog
{
	Q_OBJECT

public:
	QTextDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
	~QTextDialog();

public slots:
	void					setButtons(QDialogButtonBox::StandardButtons buttons);
	void					setLabel(const QString & message);
	void					setText(const QString & text);
	void					setReadOnly(bool readOnly=true);

	QString					label() const;
	QString					text() const;
	bool					isReadOnly() const;

protected:

private:

private:  // for now
	Ui_QTextDialog* ui;
};

} // namespace sgi
