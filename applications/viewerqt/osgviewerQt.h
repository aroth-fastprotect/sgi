#pragma once

#include <QMainWindow>
#include <osgViewer/CompositeViewer>

class QTimer;
class QPaintEvent;
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

protected:
    virtual void paintEvent( QPaintEvent* event );

    enum GLContextProfile {
        GLContextProfileNone,
        GLContextProfileCore,
        GLContextProfileCompatibility,
    };
    osgViewer::GraphicsWindow* createGraphicsWindow( int x, int y, int w, int h,
                                                     osg::GraphicsContext * sharedContext,
                                                     const std::string& glver=std::string(),
                                                     GLContextProfile profile=GLContextProfileNone,
                                                     const std::string& name=std::string(),
                                                     bool windowDecoration=false );

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
};
