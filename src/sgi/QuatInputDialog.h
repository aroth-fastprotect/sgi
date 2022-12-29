#pragma once

#include <QDialog>
#include <sgi/plugins/Quat>

QT_BEGIN_NAMESPACE
class Ui_QuatInputDialog;
QT_END_NAMESPACE

namespace sgi {

class QuatInputDialog : public QDialog
{
    Q_OBJECT

public:
    QuatInputDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QuatInputDialog();

public slots:
    void                    setLabel(const QString & message);
    QString                 label() const;

    void                    setValue(const Quat & value);
    const Quat &            value() const;

    void                    setOriginalValue(const Quat & value);
    const Quat &            originalValue() const;

    void                    setDecimals(int decimals);
    int                     decimals() const;

protected slots:
    void                    reset();

private:
    Quat                    _original_value;
    Quat                    _value;
    int                     _slider_factor;

private:  // for now
    Ui_QuatInputDialog* ui;
};

} // namespace sgi
