#pragma once

#include <QMainWindow>
#include <osgViewer/CompositeViewer>

#include <sgi_viewer_base.h>

class QTimer;
class QShowEvent;
class QPaintEvent;
class QResizeEvent;
class CompositeViewerThread;
class CreateViewHandler;

class CreateViewHandlerProxy : public QObject
{
    Q_OBJECT
public:
    CreateViewHandlerProxy(CreateViewHandler * handler, QObject * parent);

signals:
    void triggerClone(osgViewer::View * source, bool shared);

protected:
    void viewCloneImpl(osgViewer::View * source, bool shared);
};

class CreateViewHandler : public osgGA::GUIEventHandler
{
public:
    CreateViewHandler(QObject * parent);
    ~CreateViewHandler();

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

private:
    CreateViewHandlerProxy * _proxy;
};

class ViewerWidget : public QMainWindow
{
    Q_OBJECT
public:
    ViewerWidget(osg::ArgumentParser & arguments, QWidget * parent=0);
    ViewerWidget(ViewerWidget * parent, bool shared);
    virtual ~ViewerWidget();

    osgViewer::View * view();
    bool createCamera();
    void setData(osg::Node * node);

    sgi_MapNodeHelper & helper() {
        return _helper;
    }

protected:
    void showEvent( QShowEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
    void resizeEvent(QResizeEvent * event) override;

    osgViewer::GraphicsWindow* createGraphicsWindow( int x, int y, int w, int h,
                                                     osg::GraphicsContext * sharedContext,
                                                     const std::string& name=std::string(),
                                                     bool windowDecoration=false,
        bool useQt5=false,
        bool useFlightgear=false);

private:
    void init();
    void onTimer();

protected:
    CompositeViewerThread * _thread;
    QTimer * _timer;
    osg::ref_ptr<osgViewer::GraphicsWindow> _mainGW;
    osg::ref_ptr<osgViewer::View> _view;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
    QWidget * _viewWidget;
    sgi_MapNodeHelper _helper;
};
