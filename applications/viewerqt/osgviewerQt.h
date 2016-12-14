#pragma once

#include <QMainWindow>
#include <osgViewer/CompositeViewer>
#include <sstream>

namespace osgQt {
    class GraphicsWindowQt;
}

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
        osgViewer::View*     view,
        osgEarth::Util::Controls::Container* userContainer = 0L);

    void parse(
        osgEarth::MapNode*             mapNode,
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup,
        osgEarth::Util::Controls::LabelControl* userLabel);

    /**
        * Takes an existing map node and processes all the built-in example command
        * line arguments and mapNode externals.
        */
    void parse(
        osgEarth::MapNode*             mapNode,
        osg::ArgumentParser& args,
        osgViewer::View*     view,
        osg::Group*          parentGroup =0L,
        osgEarth::Util::Controls::Container* userContainer=0L);

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
    osgEarth::Util::MapNodeHelper * _mapNodeHelper;
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

protected slots:
    void onTimer();

protected:
    virtual void paintEvent( QPaintEvent* event );

    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, osg::GraphicsContext * sharedContext, const std::string& name=std::string(), bool windowDecoration=false );

private:
    void init();

protected:
    CompositeViewerThread * _thread;
    osg::ref_ptr<osgQt::GraphicsWindowQt> _mainGW;
    osg::ref_ptr<osgViewer::View> _view;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
};
