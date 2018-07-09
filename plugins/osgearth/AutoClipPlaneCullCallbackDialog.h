#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>
#include <osg/ref_ptr>

QT_BEGIN_NAMESPACE
class Ui_AutoClipPlaneCullCallbackDialog;
QT_END_NAMESPACE

namespace osgEarth {
    namespace Util {
        class AutoClipPlaneCullCallback;
    }
}

namespace sgi {
namespace osgearth_plugin {

class AutoClipPlaneCullCallbackDialog : public QDialog
{
	Q_OBJECT

public:
                        AutoClipPlaneCullCallbackDialog(QWidget * parent, osgEarth::Util::AutoClipPlaneCullCallback * callback, ISettingsDialogInfo * info=nullptr);
	virtual				~AutoClipPlaneCullCallbackDialog();

public:
    ISettingsDialog *   dialogInterface() { return _interface; }

public slots:
	void				save();
	void				apply();
	virtual void		reject();
	void				load();
	void				restoreDefaults();

	void				maxNearFarRatioChanged(int value);
	void				maxNearFarRatioChanged(double value);
    void                minNearFarRatioChanged(int value);
    void                minNearFarRatioChanged(double value);
    void                heightThresholdChanged(int value);
    void                heightThresholdChanged(double value);
    void                clampFarClipPlaneChanged(bool value);

protected:
	void				apply(bool save);

    void                showEvent  ( QShowEvent * event ) override;

private:

private:
	Ui_AutoClipPlaneCullCallbackDialog *	        ui;
    osg::ref_ptr<osgEarth::Util::AutoClipPlaneCullCallback>   _callback;
    ISettingsDialogPtr _interface;
    ISettingsDialogInfoPtr _info;
};

} // namespace osgearth_plugin
} // namespace sgi
