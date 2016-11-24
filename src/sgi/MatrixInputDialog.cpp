#include "stdafx.h"
#include <sgi/plugins/SGIItemBase.h>
#include "MatrixInputDialog.h"
#include "MatrixInputDialog.moc"

#include <QPushButton>
#include <QTextStream>

#include <ui_MatrixInputDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {


void MatrixInputDialog::formatMatrixValue(const Matrix & matrix, QString & text, MatrixUsage usage)
{
    text.clear();
    QTextStream ts(&text);
    switch(usage)
    {
    case MatrixUsageUnknown:
    default:
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col)
                ts << matrix(row, col) << " ";
            ts << endl;
        }
        break;
    }
    ts.flush();
}

bool MatrixInputDialog::parseMatrixValue(Matrix & matrix, const QString & text, MatrixUsage usage, bool * ok)
{
    bool ret = false;
    QStringList lines = text.split(QChar('\n'));
    switch(usage)
    {
    case MatrixUsageUnknown:
    default:
        ret = lines.size() == 4;
        for(unsigned row = 0; ret && row < lines.size(); ++row)
        {
            const QString & line = lines[row];
            QStringList elems = line.split(' ');
            ret = elems.size() == 4;
            for(unsigned col = 0; ret && col < elems.size(); ++col)
            {
                const QString & elem = elems[col];
                bool v_ok = false;
                double v = elem.toDouble(&v_ok);
                if(v_ok)
                    matrix(row, col) = v;
                else
                    ret = false;
            }
        }
        break;
    }
    if(ok)
        *ok = ret;
    return ret;
}

MatrixInputDialog::MatrixInputDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _original_value()
    , _value()
{
    ui = new Ui_MatrixInputDialog;
    ui->setupUi( this );

    fillMatrixUsageType(ui->representationComboBoxRaw, MatrixUsageUnknown);
    fillMatrixUsageType(ui->representationComboBoxUser, MatrixUsageUnknown);

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

void MatrixInputDialog::fillMatrixUsageType(QComboBox * comboBox, MatrixUsage usage)
{
    comboBox->clear();
    comboBox->addItem(tr("Raw"), MatrixUsageUnknown);
    comboBox->addItem(tr("Model"), MatrixUsageModel);
    comboBox->addItem(tr("View"), MatrixUsageView);
    comboBox->addItem(tr("Projection"), MatrixUsageProjection);
    int index = comboBox->findData(QVariant((int)usage));
    comboBox->setCurrentIndex(index);
}

void MatrixInputDialog::parseMatrixValue(const QString & text, MatrixUsage usage)
{
    Matrix tmp = _value;
    if(parseMatrixValue(tmp, text, usage))
        _value = tmp;
}

void MatrixInputDialog::parseMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage)
{
    parseMatrixValue(textEdit->toPlainText(), usage);
}

void MatrixInputDialog::formatMatrixValue(QString & text, MatrixUsage usage)
{
    formatMatrixValue(_value, text, usage);
}

void MatrixInputDialog::formatMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage)
{
    QString text;
    formatMatrixValue(text, usage);
    textEdit->setPlainText(text);
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

void MatrixInputDialog::rawRepresentationChanged(int index)
{
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    formatMatrixValue(ui->textEditRaw, usage);
}

void MatrixInputDialog::userRepresentationChanged(int index)
{
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    formatMatrixValue(ui->textEditUser, usage);
}

void MatrixInputDialog::rawTextEditChanged()
{
    int index = ui->representationComboBoxRaw->currentIndex();
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    parseMatrixValue(ui->textEditRaw, usage);

    index = ui->representationComboBoxUser->currentIndex();
    usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    formatMatrixValue(ui->textEditUser, usage);
}

void MatrixInputDialog::userTextEditChanged()
{
    int index = ui->representationComboBoxUser->currentIndex();
    MatrixUsage usage = (MatrixUsage)ui->representationComboBoxUser->itemData(index).toInt();
    parseMatrixValue(ui->textEditUser, usage);

    index = ui->representationComboBoxRaw->currentIndex();
    usage = (MatrixUsage)ui->representationComboBoxRaw->itemData(index).toInt();
    formatMatrixValue(ui->textEditRaw, usage);
}

} // namespace sgi
