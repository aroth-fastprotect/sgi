#pragma once

#include <QMainWindow>
#include <osgViewer/CompositeViewer>

namespace osgQt {
    class GraphicsWindowQt;
}

class QTimer;
class QPaintEvent;

class ViewerWidget : public QMainWindow, public osgViewer::CompositeViewer
{
    Q_OBJECT
public:
    ViewerWidget(osg::ArgumentParser & arguments, QWidget * parent=0);
    virtual ~ViewerWidget();

    osgViewer::View * mainView();

protected slots:
    void onTimer();

protected:
    virtual void paintEvent( QPaintEvent* event );

    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );

protected:
    QTimer * _timer;
    osg::ref_ptr<osgQt::GraphicsWindowQt> _mainGW;
    osg::ref_ptr<osgViewer::View> _mainView;
};
