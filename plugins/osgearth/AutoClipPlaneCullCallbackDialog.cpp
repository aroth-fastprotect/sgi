#include "stdafx.h"
#include "AutoClipPlaneCullCallbackDialog.h"
#include "AutoClipPlaneCullCallbackDialog.moc"

#include <QPushButton>

#include "ui_AutoClipPlaneCullCallbackDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>

#include <osgEarthUtil/AutoClipPlaneHandler>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace osgearth_plugin {

AutoClipPlaneCullCallbackDialog::AutoClipPlaneCullCallbackDialog(QWidget * parent, osgEarth::Util::AutoClipPlaneCullCallback * callback, ISettingsDialogInfo * info)
	: QDialog(parent)
	, _callback(callback)
    , _interface(new SettingsDialogImpl(this))
    , _info(info)
{
	ui = new Ui_AutoClipPlaneCullCallbackDialog;
	ui->setupUi( this );

	//connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
	connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

	load();
}

AutoClipPlaneCullCallbackDialog::~AutoClipPlaneCullCallbackDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
}

void AutoClipPlaneCullCallbackDialog::showEvent( QShowEvent * event )
{
	load();
}

void AutoClipPlaneCullCallbackDialog::load()
{
	double minNearFarRatio = _callback->getMinNearFarRatio();
    double maxNearFarRatio = _callback->getMaxNearFarRatio();
    double heightThreshold = _callback->getHeightThreshold();
    bool clampFarClipPlane = _callback->getClampFarClipPlane();

    ui->minNearFarRatioSpin->blockSignals(true);
    ui->minNearFarRatio->blockSignals(true);
	ui->minNearFarRatioSpin->setValue(minNearFarRatio);
    ui->minNearFarRatio->setValue(minNearFarRatio * 100000);
    ui->minNearFarRatioSpin->blockSignals(false);
    ui->minNearFarRatio->blockSignals(false);

    ui->maxNearFarRatioSpin->blockSignals(true);
    ui->maxNearFarRatio->blockSignals(true);
    ui->maxNearFarRatioSpin->setValue(maxNearFarRatio);
    ui->maxNearFarRatio->setValue(maxNearFarRatio * 100000);
    ui->maxNearFarRatioSpin->blockSignals(false);
    ui->maxNearFarRatio->blockSignals(false);

    ui->heightThresholdSpin->blockSignals(true);
    ui->heightThreshold->blockSignals(true);
    ui->heightThresholdSpin->setValue(heightThreshold);
    ui->heightThreshold->setValue(heightThreshold * 10);
    ui->heightThresholdSpin->blockSignals(false);
    ui->heightThreshold->blockSignals(false);

    ui->clampFarClipPlane->blockSignals(true);
    ui->clampFarClipPlane->setChecked(clampFarClipPlane);
    ui->clampFarClipPlane->blockSignals(false);
}

void AutoClipPlaneCullCallbackDialog::restoreDefaults()
{
    _callback->setMinNearFarRatio(0.00001);
    _callback->setMaxNearFarRatio(0.00005);
    _callback->setHeightThreshold(250.0);
    _callback->setClampFarClipPlane(true);
	load();
}

void AutoClipPlaneCullCallbackDialog::maxNearFarRatioChanged(int value)
{
    ui->maxNearFarRatioSpin->blockSignals(true);
	ui->maxNearFarRatioSpin->setValue((double)value/100000.0);
    ui->maxNearFarRatioSpin->blockSignals(false);
	apply();
}

void AutoClipPlaneCullCallbackDialog::maxNearFarRatioChanged(double value)
{
    ui->maxNearFarRatio->blockSignals(true);
	ui->maxNearFarRatio->setValue(value * 100000);
    ui->maxNearFarRatio->blockSignals(false);
	apply();
}

void AutoClipPlaneCullCallbackDialog::minNearFarRatioChanged(int value)
{
    ui->minNearFarRatioSpin->blockSignals(true);
    ui->minNearFarRatioSpin->setValue((double)value/100000.0);
    ui->minNearFarRatioSpin->blockSignals(false);
    apply();
}

void AutoClipPlaneCullCallbackDialog::minNearFarRatioChanged(double value)
{
    ui->minNearFarRatio->blockSignals(true);
    ui->minNearFarRatio->setValue(value * 100000);
    ui->minNearFarRatio->blockSignals(false);
    apply();
}

void AutoClipPlaneCullCallbackDialog::heightThresholdChanged(int value)
{
    ui->heightThresholdSpin->blockSignals(true);
    ui->heightThresholdSpin->setValue((double)value/10.0);
    ui->heightThresholdSpin->blockSignals(false);
    apply();
}

void AutoClipPlaneCullCallbackDialog::heightThresholdChanged(double value)
{
    ui->heightThreshold->blockSignals(true);
    ui->heightThreshold->setValue(value * 10);
    ui->heightThreshold->blockSignals(false);
    apply();
}

void AutoClipPlaneCullCallbackDialog::clampFarClipPlaneChanged(bool value)
{
    apply();
}

void AutoClipPlaneCullCallbackDialog::save()
{
	apply(true);
	accept();
}

void AutoClipPlaneCullCallbackDialog::apply()
{
	apply(false);
}

void AutoClipPlaneCullCallbackDialog::apply(bool save)
{
    _callback->setMinNearFarRatio(ui->minNearFarRatioSpin->value());
    _callback->setMaxNearFarRatio(ui->maxNearFarRatioSpin->value());
    _callback->setHeightThreshold(ui->heightThresholdSpin->value());
    _callback->setClampFarClipPlane(ui->clampFarClipPlane->isChecked());
    _info->triggerRepaint();
}

void AutoClipPlaneCullCallbackDialog::reject()
{
	QDialog::reject();
}

} // namespace osgearth_plugin
} // namespace sgi
