#pragma once

#include <QDialog>
#include <sgi/plugins/SGIPluginInterface.h>

QT_BEGIN_NAMESPACE
class Ui_ExtraViewDialog;
QT_END_NAMESPACE

namespace osg {
    class Camera;
}
namespace osgQt {
    class GraphicsWindowQt;
}
namespace osgViewer {
    class View;
    class CompositeViewer;
}

namespace sgi {
namespace osg_plugin {

class ViewOSG : public QWidget
{
    Q_OBJECT

public:
    ViewOSG(QWidget * parent=0);
    virtual	~ViewOSG();

    void setCamera(osgViewer::CompositeViewer * viewer, osg::Camera * camera);

protected:
    osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, osg::GraphicsContext * sharedContext, const std::string& name=std::string(), bool windowDecoration=false);

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateCamera();

protected:
    osg::ref_ptr<osgQt::GraphicsWindowQt> _gfx;
    osg::ref_ptr<osgViewer::View> _view;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
    osg::ref_ptr<osg::Camera> _viewCamera;
    osg::observer_ptr<osg::Camera> _camera;
    QWidget * _widget;
};

class ExtraViewDialog : public QDialog
{
	Q_OBJECT

public:
                        ExtraViewDialog(QWidget * parent, osg::Camera * camera);
	virtual				~ExtraViewDialog();

public:
    ISettingsDialog *   dialogInterface() { return _interface; }

public slots:
	void				save();
	void				apply();
	virtual void		reject();
	void				load();
	void				restoreDefaults();

protected:
	void				apply(bool save);

	virtual void 		showEvent  ( QShowEvent * event );
    virtual void        hideEvent  ( QHideEvent * event );

private:
    Ui_ExtraViewDialog *	        ui;
    osg::ref_ptr<osg::Camera>   _camera;
    ISettingsDialogPtr _interface;
    QTimer * _timer;
};

} // namespace osg_plugin
} // namespace sgi
