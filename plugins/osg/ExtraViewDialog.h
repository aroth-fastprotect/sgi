#pragma once

#include <sgi/plugins/SGISettingsDialogImpl>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

QT_BEGIN_NAMESPACE
class Ui_ExtraViewDialog;
QT_END_NAMESPACE

namespace osg {
    class Camera;
    class Texture;
    class GraphicsContext;
}
namespace osgViewer {
    class View;
    class CompositeViewer;
    class GraphicsWindow;
}

namespace sgi {
    class SGIPluginHostInterface;

namespace osg_plugin {

class ViewOSG : public QWidget
{
    Q_OBJECT

public:
    ViewOSG(QWidget * parent=nullptr);
    ~ViewOSG() override;

    void setCamera(osgViewer::CompositeViewer * viewer, osg::Camera * camera);
    void setRTTCamera(osgViewer::CompositeViewer * viewer, osg::Texture * texture);

protected:
    osgViewer::GraphicsWindow* createGraphicsWindow(int x, int y, int w, int h, osg::GraphicsContext * sharedContext, const std::string& name=std::string(), bool windowDecoration=false);

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateCamera();

protected:
    osg::ref_ptr<osgViewer::GraphicsWindow> _gfx;
    osg::ref_ptr<osgViewer::View> _view;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
    osg::ref_ptr<osg::Camera> _viewCamera;
    osg::observer_ptr<osg::Camera> _camera;
    QWidget * _widget;
};

class ExtraViewDialog : public SettingsQDialogImpl
{
	Q_OBJECT

public:
                        ExtraViewDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
                        ~ExtraViewDialog() override;

public slots:
	void				save();
	void				apply();
    void                reject() override;
	void				load();
	void				restoreDefaults();

protected:
	void				apply(bool save);

    void                showEvent  ( QShowEvent * event ) override;
    void                hideEvent  ( QHideEvent * event ) override;

private:
    Ui_ExtraViewDialog *	        ui;
    osg::ref_ptr<osg::Camera>   _camera;
    QTimer * _timer;
};

} // namespace osg_plugin
} // namespace sgi
