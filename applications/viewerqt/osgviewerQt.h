#pragma once

#include <QMainWindow>
#include <osgViewer/CompositeViewer>
#include <sstream>

#ifdef SGI_USE_OSGQT
namespace osgQt {
    class GraphicsWindowQt;
}
#endif // WITH_OSGQT

#ifdef SGI_USE_OSGEARTH
namespace osgEarth {
    class MapNode;
    namespace Util {
        class MapNodeHelper;
        namespace Controls {
            class Control;
            class Container;
            class LabelControl;
        }
    }
}
#endif // WITH_OSGEARTH

class QTimer;
class QPaintEvent;

/**
    * Parses a set of built-in example arguments. Any Controls created by parsing
    * command-line parameters will appear in the lower-left corner of the display.
    */
class osgViewerQtMapNodeHelper
{
public:
    osgViewerQtMapNodeHelper();
    ~osgViewerQtMapNodeHelper();

    /**
        * Loads a map file and processes all the built-in example command line
        * arguments and XML externals.
        */
    osg::Group* load(
        osg::ArgumentParser& args,
        osgViewer::View*     view
#ifdef SGI_USE_OSGEARTH
        , osgEarth::Util::Controls::Container* userContainer = 0L
#endif
    );

    void parse(
#ifdef SGI_USE_OSGEARTH
        osgEarth::MapNode*   mapNode,
#endif // WITH_OSGEARTH
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup
#ifdef SGI_USE_OSGEARTH
        , osgEarth::Util::Controls::LabelControl* userLabel
#endif
    );

#ifdef SGI_USE_OSGEARTH
    /**
        * Takes an existing map node and processes all the built-in example command
        * line arguments and mapNode externals.
        */
    void parse(
        osgEarth::MapNode*   mapNode,
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup =0L,
        osgEarth::Util::Controls::Container* userContainer=0L
    );
#endif // WITH_OSGEARTH

    /**
        * Configures a view with a stock set of event handlers that are useful
        * for demos, and performs some other common view configuration for osgEarth.
        */
    void configureView( osgViewer::View* view ) const;

    void setupInitialPosition( osgViewer::View* view, int viewpointNum=-1, const std::string & viewpointName=std::string() ) const;

    osg::Group * setupRootGroup(osg::Group * root);

    /**
        * Returns a usage string
        */
    std::string usage() const;

    std::string errorMessages() const;

    const std::vector<std::string> & files() const {
        return m_files;
    }
    bool onlyImages() const { return _onlyImages; }

private:
    std::stringstream m_errorMessages;
    std::vector<std::string> m_files;
#ifdef SGI_USE_OSGEARTH
    osgEarth::Util::MapNodeHelper * _mapNodeHelper;
#endif
    bool _onlyImages;
};

class CompositeViewerThread;

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

    osgViewer::GraphicsWindow* createGraphicsWindow( int x, int y, int w, int h, osg::GraphicsContext * sharedContext, const std::string& name=std::string(), bool windowDecoration=false );

private:
    void init();
    void onTimer();

protected:
    CompositeViewerThread * _thread;
    QTimer * _timer;
    osg::ref_ptr<osgViewer::GraphicsWindow> _mainGW;
    osg::ref_ptr<osgViewer::View> _view;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
};
