#pragma once

#include <QDialog>
#include <sgi/plugins/Matrix>

QT_BEGIN_NAMESPACE
class Ui_MatrixInputDialog;
class QComboBox;
class QPlainTextEdit;
QT_END_NAMESPACE

namespace sgi {

class MatrixInputDialog : public QDialog
{
    Q_OBJECT

public:
    MatrixInputDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MatrixInputDialog();

public slots:
    void                    setLabel(const QString & message);
    QString                 label() const;

    void                    setValue(const Matrix & value, MatrixUsage usage);
    const Matrix &          value() const;

    void                    setOriginalValue(const Matrix & value, MatrixUsage usage);
    const Matrix &          originalValue() const;

    void                    setDecimals(int decimals);
    int                     decimals() const;

    void                    rawRepresentationChanged(int index);
    void                    userRepresentationChanged(int index);

    void                    rawTextEditChanged();
    void                    userTextEditChanged();

protected slots:
    void                    reset();

protected:
    void                    fillMatrixUsageType(QComboBox * combo, MatrixUsage usage);

    static void             formatMatrixValue(const Matrix & matrix, QString & text, MatrixUsage usage);
    static bool             parseMatrixValue(Matrix & matrix, const QString & text, MatrixUsage usage, bool * ok=nullptr);

    void                    parseMatrixValue(const QString & text, MatrixUsage usage);
    void                    parseMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage);

    void                    formatMatrixValue(QString & text, MatrixUsage usage);
    void                    formatMatrixValue(QPlainTextEdit * textEdit, MatrixUsage usage);


private:
    Matrix                  _original_value;
    MatrixUsage             _original_usage;
    Matrix                  _value;
    MatrixUsage             _usage;

private:  // for now
    Ui_MatrixInputDialog* ui;
};

} // namespace sgi
