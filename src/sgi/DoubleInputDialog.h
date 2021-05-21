#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class Ui_DoubleInputDialog;
QT_END_NAMESPACE

namespace sgi {

class DoubleInputDialog : public QDialog
{
    Q_OBJECT

public:
    DoubleInputDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~DoubleInputDialog();

public slots:
    void                    setLabel(const QString & message);
    QString                 label() const;

    void                    setValue(double value);
    double                  value() const;

    void                    setOriginalValue(double value);
    double                  originalValue() const;

    void                    setRange(double minimum, double maximum);

    void                    setDecimals(int decimals);
    int                     decimals() const;

protected slots:
    void                    reset();
    void                    sliderChanged(int pos);
    void                    spinBoxChanged(double value);

private:
    double                  _original_value;
    int                     _slider_factor;

private:  // for now
    Ui_DoubleInputDialog* ui;
};

} // namespace sgi
