#include "stdafx.h"
#include "LODScaleOverrideDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIItemOsg>

#include <QPushButton>

#include <ui_LODScaleOverrideDialog.h>

#include <osgEarth/NodeUtils>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

LODScaleOverrideDialog::LODScaleOverrideDialog(QWidget * parent, SGIItemBase * item, ISettingsDialogInfo * info)
    : QDialog(parent)
    , ui(new Ui_LODScaleOverrideDialog)
    , _interface(new SettingsDialogImpl(this))
    , _info(info)
    , _item(item)
    , _original_value(DBL_MAX)
    , _slider_factor(2)
    , _ready(false)
{
    ui->setupUi( this );

    _slider_factor = pow(10.0, ui->doubleSpinBox->decimals());
    double steps = pow(0.1, ui->doubleSpinBox->decimals());
    ui->doubleSpinBox->setSingleStep(steps);

    setDecimals(2);
    setRange(0.01, 10.0);

#ifdef SGI_USE_OSGEARTH_FAST
    osgEarth::LODScaleOverrideNode* object = static_cast<osgEarth::LODScaleOverrideNode*>(static_cast<SGIItemOsg*>(_item.get())->object());
    float value = object->getLODScale();
#else
    float value = 1.0f;
#endif
    setOriginalValue(value);
    setValue(value);

    connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &LODScaleOverrideDialog::close);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &LODScaleOverrideDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &LODScaleOverrideDialog::reset);
    _ready = true;
}

LODScaleOverrideDialog::~LODScaleOverrideDialog()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}

void LODScaleOverrideDialog::setLabel(const QString & message)
{
    ui->label->setText(message);
}

QString LODScaleOverrideDialog::label() const
{
    return ui->label->text();
}

void LODScaleOverrideDialog::apply()
{
    if (!_ready)
        return;

    double value = ui->doubleSpinBox->value();
#ifdef SGI_USE_OSGEARTH_FAST
    osgEarth::LODScaleOverrideNode* object = static_cast<osgEarth::LODScaleOverrideNode*>(static_cast<SGIItemOsg*>(_item.get())->object());
    object->setLODScale((float)value);
#endif
}

void LODScaleOverrideDialog::reset()
{
    if(_original_value != DBL_MAX)
        setValue(_original_value);
}

void LODScaleOverrideDialog::sliderChanged(int pos)
{
    ui->doubleSpinBox->blockSignals(true);
    double value = (double)pos / (double)_slider_factor;
    ui->doubleSpinBox->setValue(value);
    ui->doubleSpinBox->blockSignals(false);
    apply();
}

void LODScaleOverrideDialog::spinBoxChanged(double value)
{
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setValue(ceil(value * _slider_factor));
    ui->horizontalSlider->blockSignals(false);
    apply();
}

void LODScaleOverrideDialog::setOriginalValue(double value)
{
    _original_value = value;
}

double LODScaleOverrideDialog::originalValue() const
{
    return _original_value;
}

void LODScaleOverrideDialog::setValue(double value)
{
    if(_original_value == DBL_MAX)
        setOriginalValue(value);

    ui->doubleSpinBox->blockSignals(true);
    ui->horizontalSlider->blockSignals(true);
    ui->doubleSpinBox->setValue(value);
    ui->horizontalSlider->setValue(ceil(value * _slider_factor));

    ui->doubleSpinBox->blockSignals(false);
    ui->horizontalSlider->blockSignals(false);
}

double LODScaleOverrideDialog::value() const
{
    return ui->doubleSpinBox->value();
}

void LODScaleOverrideDialog::setRange(double minimum, double maximum)
{
    ui->doubleSpinBox->setMinimum(minimum);
    ui->doubleSpinBox->setMaximum(maximum);

    ui->horizontalSlider->setMaximum(ceil(maximum * _slider_factor));
    ui->horizontalSlider->setMinimum(ceil(minimum * _slider_factor));
}

void LODScaleOverrideDialog::setDecimals(int decimals)
{
    ui->doubleSpinBox->setDecimals(decimals);
    ui->doubleSpinBox->setSingleStep(pow(0.1, ui->doubleSpinBox->decimals()));
    _slider_factor = pow(10, ui->doubleSpinBox->decimals());
    double maximum = ui->doubleSpinBox->maximum();
    double minimum = ui->doubleSpinBox->minimum();
    ui->horizontalSlider->setMaximum(ceil(maximum * _slider_factor));
    ui->horizontalSlider->setMinimum(ceil(minimum * _slider_factor));
}

int LODScaleOverrideDialog::decimals() const
{
    return ui->doubleSpinBox->decimals();
}

} // namespace sgi
