/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2010 Robert Osfield
 *
 * This application is open source and may be redistributed and/or modified
 * freely and without restriction, both in commercial and non commercial applications,
 * as long as this copyright notice is maintained.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osg/Types>
#include <osgText/Text>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/View>
#include <osgViewer/config/SingleWindow>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgGA/Device>

#ifdef SGI_USE_OSGEARTH
#include <osgEarth/GLUtils>
#endif

#include <iostream>

#include <QApplication>
#include <QTimer>

#include <sgi_viewer_base.h>

/** single camera on a single window.*/
class SingleWindowShared : public osgViewer::ViewConfig
{
public:

    SingleWindowShared() :_x(0), _y(0), _width(-1), _height(-1), _screenNum(0), _windowDecoration(true), _overrideRedirect(false) {}
    SingleWindowShared(int x, int y, int width, int height, unsigned int screenNum = 0) :_x(x), _y(y), _width(width), _height(height), _screenNum(screenNum), _windowDecoration(true), _overrideRedirect(false) {}
    SingleWindowShared(const SingleWindowShared& rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) :ViewConfig(rhs, copyop), _x(rhs._x), _y(rhs._y), _width(rhs._width), _height(rhs._height), _screenNum(rhs._screenNum), _windowDecoration(rhs._windowDecoration), _overrideRedirect(rhs._overrideRedirect) {}

    SingleWindowShared(osgViewer::View * source);

    META_Object(osgviewer, SingleWindowShared);

    virtual void configure(osgViewer::View& view) const;

    void setX(int x) { _x = x; }
    int getX() const { return _x; }

    void setY(int y) { _y = y; }
    int getY() const { return _y; }

    void setWidth(int w) { _width = w; }
    int getWidth() const { return _width; }

    void setHeight(int h) { _height = h; }
    int getHeight() const { return _height; }

    void setScreenNum(unsigned int sn) { _screenNum = sn; }
    unsigned int getScreenNum() const { return _screenNum; }

    void setWindowDecoration(bool wd) { _windowDecoration = wd; }
    bool getWindowDecoration() const { return _windowDecoration; }

    void setOverrideRedirect(bool override) { _overrideRedirect = override; }
    bool getOverrideRedirect() const { return _overrideRedirect; }

protected:

    int _x, _y, _width, _height;
    unsigned int _screenNum;
    bool _windowDecoration;
    bool _overrideRedirect;
    osg::ref_ptr<osg::GraphicsContext> _sharedContext;
};

SingleWindowShared::SingleWindowShared(osgViewer::View * source)
    :_x(0), _y(0), _width(-1), _height(-1), _screenNum(0), _windowDecoration(true), _overrideRedirect(false)
{
    const osgViewer::ViewConfig * lastcfg = source->getLastAppliedViewConfig();
    const osgViewer::SingleWindow* lastcfgwin = dynamic_cast<const osgViewer::SingleWindow*>(lastcfg);
    if (lastcfgwin)
    {
        _x = lastcfgwin->getX();
        _y = lastcfgwin->getY();
        _width = lastcfgwin->getWidth();
        _height = lastcfgwin->getHeight();
        _screenNum = lastcfgwin->getScreenNum();
        _windowDecoration = lastcfgwin->getWindowDecoration();
        _overrideRedirect = lastcfgwin->getOverrideRedirect();
    }
    _sharedContext = source->getCamera()->getGraphicsContext();
}

void SingleWindowShared::configure(osgViewer::View& view) const
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi)
    {
        OSG_NOTICE << "SingleWindow::configure() : Error, no WindowSystemInterface available, cannot create windows." << std::endl;
        return;
    }

    osg::DisplaySettings* ds = getActiveDisplaySetting(view);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

    traits->readDISPLAY();
    if (traits->displayNum < 0) traits->displayNum = 0;

    traits->screenNum = _screenNum;
    traits->x = _x;
    traits->y = _y;
    traits->width = _width;
    traits->height = _height;
    traits->windowDecoration = _windowDecoration;
    traits->overrideRedirect = _overrideRedirect;
    traits->doubleBuffer = true;
    traits->sharedContext = _sharedContext;

    if (traits->width <= 0 || traits->height <= 0)
    {
        osg::GraphicsContext::ScreenIdentifier si;
        si.readDISPLAY();

        // displayNum has not been set so reset it to 0.
        if (si.displayNum < 0) si.displayNum = 0;

        si.screenNum = _screenNum;

        unsigned int width, height;
        wsi->getScreenResolution(si, width, height);
        if (traits->width <= 0) traits->width = width;
        if (traits->height <= 0) traits->height = height;
    }

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    view.getCamera()->setGraphicsContext(gc.get());

    osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
    if (gw)
    {
        OSG_INFO << "View::setUpViewOnSingleScreen - GraphicsWindow has been created successfully." << std::endl;
        gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(traits->x, traits->y, traits->width, traits->height);
    }
    else
    {
        OSG_NOTICE << "  GraphicsWindow has not been created successfully." << std::endl;
        return;
    }

    double fovy, aspectRatio, zNear, zFar;
    view.getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

    double newAspectRatio = double(traits->width) / double(traits->height);
    double aspectRatioChange = newAspectRatio / aspectRatio;
    if (aspectRatioChange != 1.0)
    {
        view.getCamera()->getProjectionMatrix() *= osg::Matrix::scale(1.0 / aspectRatioChange, 1.0, 1.0);
    }

    view.getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;

    view.getCamera()->setDrawBuffer(buffer);
    view.getCamera()->setReadBuffer(buffer);

    if (ds->getKeystoneHint())
    {
        if (ds->getKeystoneHint() && !ds->getKeystoneFileNames().empty())
        {
            osgViewer::Keystone::loadKeystoneFiles(ds);
        }
        if (ds->getKeystones().empty()) ds->getKeystones().push_back(new osgViewer::Keystone);

        view.assignStereoOrKeystoneToCamera(view.getCamera(), ds);
    }
    else if (ds->getStereo() && ds->getUseSceneViewForStereoHint())
    {
        view.assignStereoOrKeystoneToCamera(view.getCamera(), ds);
    }
}

class RunCompositeViewer : public QObject {

    osgViewer::CompositeViewer * _viewer;
public:
    RunCompositeViewer(osgViewer::CompositeViewer * viewer)
        : _viewer(viewer)
    {
        QTimer * timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &RunCompositeViewer::onTimer, Qt::DirectConnection);
        timer->start(20);
    }

    void onTimer()
    {
        _viewer->frame();
        if (_viewer->done())
            qApp->quit();
    }


};

class CreateViewHandler : public osgGA::GUIEventHandler {
public:

    CreateViewHandler() {}
    ~CreateViewHandler() {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        if (!view) return false;
        switch (ea.getEventType())
        {
        case(osgGA::GUIEventAdapter::KEYUP):
            if (ea.getKey() == 'v' && ea.getModKeyMask() == 0)
            {
                viewCloneImpl(view, false);
            }
            else if (ea.getKey() == 'V')
            {
                viewCloneImpl(view, true);
            }
            break;
        default:
            break;
        }
        return false;
    }

    void viewCloneImpl(osgViewer::View * source, bool shared)
    {
        osg::Camera * sourceCamera = source->getCamera();

        osgViewer::View* nextview = new osgViewer::View;

        // configure the near/far so we don't clip things that are up close
        osg::Camera * camera = nextview->getCamera();
        camera->setNearFarRatio(sourceCamera->getNearFarRatio());
        camera->setClearColor(sourceCamera->getClearColor());
        camera->setClearMask(sourceCamera->getClearMask());

        nextview->setSceneData(source->getSceneData());

        nextview->setCameraManipulator(source->getCameraManipulator());
        for (auto evh : source->getEventHandlers())
            nextview->addEventHandler(evh);

        osgViewer::ViewConfig * cfg;
        if (shared)
            cfg = new SingleWindowShared(source);
        else
            cfg = source->getLastAppliedViewConfig();
        if (cfg)
            nextview->apply(cfg);

        osgViewer::CompositeViewer * viewer = dynamic_cast<osgViewer::CompositeViewer *>(source->getViewerBase());
        if (viewer)
            viewer->addView(nextview);
    }

};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the standard OpenSceneGraph example which loads and visualises 3d models.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("--image <filename>","Load an image and render it on a quad");
    arguments.getApplicationUsage()->addCommandLineOption("--dem <filename>","Load an image/DEM and render it on a HeightField");
    arguments.getApplicationUsage()->addCommandLineOption("--login <url> <username> <password>","Provide authentication information for http file access.");
    arguments.getApplicationUsage()->addCommandLineOption("-p <filename>","Play specified camera path animation file, previously saved with 'z' key.");
    arguments.getApplicationUsage()->addCommandLineOption("--speed <factor>","Speed factor for animation playing (1 == normal speed).");
    arguments.getApplicationUsage()->addCommandLineOption("--device <device-name>","add named device to the viewer");
    arguments.getApplicationUsage()->addCommandLineOption("--window <x y w h>", "Set the position (x,y) and size (w,h) of the viewer window.");

    initializeNotifyLevels(arguments);
    // construct the viewer.
    osg::ref_ptr<osgViewer::CompositeViewer> viewer = new osgViewer::CompositeViewer(arguments);

    unsigned int helpType = 0;
    if ((helpType = arguments.readHelpType()))
    {
        arguments.getApplicationUsage()->write(std::cout, helpType);
        return 1;
    }

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    if (arguments.argc()<=1)
    {
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }

    while (arguments.read("-s")) { viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded); }
    while (arguments.read("-g")) { viewer->setThreadingModel(osgViewer::CompositeViewer::CullDrawThreadPerContext); }
    while (arguments.read("-c")) { viewer->setThreadingModel(osgViewer::CompositeViewer::CullThreadPerCameraDrawThreadPerContext); }

    int ret = 0;
    int screenNum = -1;
    while (arguments.read("--screen", screenNum)) {}

    int x = -1, y = -1, width = -1, height = -1;
    while (arguments.read("--window", x, y, width, height)) {}

    std::string url, username, password;
    while(arguments.read("--login",url, username, password))
    {
        if (!osgDB::Registry::instance()->getAuthenticationMap())
        {
            osgDB::Registry::instance()->setAuthenticationMap(new osgDB::AuthenticationMap);
            osgDB::Registry::instance()->getAuthenticationMap()->addAuthenticationDetails(
                url,
                new osgDB::AuthenticationDetails(username, password)
            );
        }
    }

#if defined(OSG_GL3_AVAILABLE) && defined(SGI_USE_OSGEARTH)
    viewer->setRealizeOperation(new osgEarth::GL3RealizeOperation());
#endif

    osgViewer::View* firstview = new osgViewer::View;
    firstview->setName("First view");

#if defined(SGI_USE_OSGEARTH)
    // Sets up global default uniform values needed by osgEarth
    osgEarth::GLUtils::setGlobalDefaults(firstview->getCamera()->getOrCreateStateSet());
#endif

    if (width > 0 && height > 0)
    {
        if (screenNum >= 0) firstview->setUpViewInWindow(x, y, width, height, screenNum);
        else firstview->setUpViewInWindow(x, y, width, height);

    }
    else if (screenNum >= 0)
    {
        firstview->setUpViewOnSingleScreen(screenNum);
    }

    bool addSceneGraphInspector = true;
    bool showSceneGraphInspector = true;
    bool showImagePreviewDialog = false;
    int viewpointNum = -1;
    std::string viewpointName;

    if (arguments.read("--nosgi"))
        addSceneGraphInspector = false;
    if (arguments.read("--hidesgi"))
        showSceneGraphInspector = false;

    firstview->addEventHandler(new CreateViewHandler);

    // load the data
    sgi_MapNodeHelper helper;
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags
    osg::Node* node = helper.load(arguments, firstview);
    if (node)
    {
        bool showImagePreviewDialog = helper.onlyImages() ? true : false;
        osg::ref_ptr<osg::Group> root = new osg::Group;
        root->addChild(node);

        if (addSceneGraphInspector)
        {
            osg::ref_ptr<osgDB::Options> opts = osgDB::Registry::instance()->getOptions();
            if (opts)
                opts = opts->cloneOptions();
            else
                opts = new osgDB::Options;

            createWidgetForGraphicsWindow(firstview->getCamera()->getGraphicsContext(), qApp);
            opts->setPluginStringData("showSceneGraphDialog", showSceneGraphInspector ? "1" : "0");
            opts->setPluginStringData("showImagePreviewDialog", showImagePreviewDialog ? "1" : "0");
            osg::ref_ptr<osg::Node> sgi_loader = osgDB::readRefNodeFile(".sgi_loader", opts);
            if (sgi_loader.valid())
                root->addChild(sgi_loader);
            else
            {
                std::cout << arguments.getApplicationName() << ": Failed to load SGI" << std::endl;
                return 1;
            }
        }

        helper.setupInitialPosition(firstview);

        // pass the model to the MovieEventHandler so it can pick out ImageStream's to manipulate.
        MovieEventHandler* meh = new MovieEventHandler();
        meh->set(root);

        firstview->addEventHandler(meh);

        // optimize the scene graph, remove redundant nodes and state etc.
        osgUtil::Optimizer optimizer;
        optimizer.optimize(root);

        firstview->setSceneData(root);

        viewer->addView(firstview);


        viewer->realize();
        auto run = new RunCompositeViewer(viewer.get());
        ret = app.exec();

        delete run;
    }
    else
    {
        std::cerr << helper.errorMessages() << std::endl;
        ret = 1;
    }
    return ret;
}
