#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class Ui_MatrixInputDialog;
QT_END_NAMESPACE

namespace sgi {

class MatrixInputDialog : public QDialog
{
    Q_OBJECT

public:
    MatrixInputDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~MatrixInputDialog();

public slots:
    void                    setLabel(const QString & message);
    QString                 label() const;

    void                    setValue(const Matrix & value);
    const Matrix &          value() const;

    void                    setOriginalValue(const Matrix & value);
    const Matrix &          originalValue() const;

    void                    setDecimals(int decimals);
    int                     decimals() const;

protected slots:
    void                    reset();

private:
    Matrix                  _original_value;
    Matrix                  _value;

private:  // for now
    Ui_MatrixInputDialog* ui;
};

} // namespace sgi
