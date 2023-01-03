#pragma once

#include <QMainWindow>

#include <sgi_viewer_base_vsg.h>

namespace vsg {
    class CommandLine;
    class Node;
    class Viewer;
    class Window;
}

namespace vsgQt {
    class ViewerWindow ;
}

class QTimer;
class QShowEvent;
class QPaintEvent;
class QResizeEvent;



class ViewerWidget : public QMainWindow
{
    Q_OBJECT
public:

	enum GLContextImpl {
		GLContextImplDefault = 0,
        GLContextImplVulkan,
	};
	
    ViewerWidget(vsg::CommandLine& arguments, QWidget* parent = nullptr);
    ViewerWidget(ViewerWidget * parent, bool shared, GLContextImpl impl);
    virtual ~ViewerWidget();

    bool createCamera();
    void setData(vsg::Node * node);

    sgi_MapNodeHelper & helper() {
        return _helper;
    }

	GLContextImpl glContextImpl() const {
		return _impl;
	}

protected:
    void showEvent( QShowEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
    void resizeEvent(QResizeEvent * event) override;

private:
    void init();
    void onTimer();

    vsgQt::ViewerWindow* createGraphicsWindow( int x, int y, int w, int h,
                                               vsgQt::ViewerWindow * sharedContext,
                                      GLContextImpl impl);


protected:
    QTimer * _timer;
    QWidget * _viewWidget;
    vsg::ref_ptr<vsg::Window> _window;
    vsgQt::ViewerWindow * _viewerWindow;
    vsg::ref_ptr<vsg::Viewer> _viewer;
    sgi_MapNodeHelper _helper;
	GLContextImpl _impl;
};
