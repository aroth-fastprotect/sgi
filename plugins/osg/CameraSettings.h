#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

QT_BEGIN_NAMESPACE
class Ui_CameraSettings;
QT_END_NAMESPACE

namespace osg {
    class Camera;
}

namespace sgi {
namespace osg_plugin {

class CameraSettings : public QDialog
{
	Q_OBJECT

public:
                        CameraSettings(QWidget * parent, osg::Camera * camera);
	virtual				~CameraSettings();

public:
    ISettingsDialog *   dialogInterface() { return _interface; }

public slots:
	void				save();
	void				apply();
	virtual void		reject();
	void				load();
	void				restoreDefaults();

	void				changeLODScale(int value);
	void				changeLODScale(double value);

    void                changeSmallFeature(int value);
    void                changeSmallFeature(double value);

    void                changeNearClipping(int value);
    void                changeNearClipping(double value);

    void                changeFarClipping(int value);
    void                changeFarClipping(double value);

    void                changeNearFarRatio(int value);
    void                changeNearFarRatio(double value);

    void                changeComputeNearFarMode(int value);

	void				farClippingEnable(bool enable);
	void				nearClippingEnable(bool enable);
	void				smallFeatureEnable(bool enable);


protected:
	void				apply(bool save);

	virtual void 		showEvent  ( QShowEvent * event );
    virtual void        hideEvent  ( QHideEvent * event );

private:
    class UpdateCullSettings;

private:
	Ui_CameraSettings *	        ui;
    osg::ref_ptr<osg::Camera>   _camera;
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    ISettingsDialog * _interface;
    QTimer * _timer;
};

} // namespace osg_plugin
} // namespace sgi
