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
    QTextDialog(QWidget *parent = nullptr, Qt::WindowFlags f = 0);
	~QTextDialog();

    enum TextMode {
        TextModePlain,
        TextModeHTML
    };
    Q_ENUMS(TextMode)

public slots:
	void					setButtons(QDialogButtonBox::StandardButtons buttons);
	void					setLabel(const QString & message);
	void					setText(const QString & text, TextMode mode=TextModePlain);
	void					setReadOnly(bool readOnly=true);

	QString					label() const;
	QString					text(TextMode mode = TextModePlain) const;
	bool					isReadOnly() const;

protected:

private:

private:  // for now
	Ui_QTextDialog* ui;
};

} // namespace sgi
