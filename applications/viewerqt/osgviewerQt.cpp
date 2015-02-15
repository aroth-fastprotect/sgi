#include "osgviewerQt.h"
#include <QTimer>
#include <QApplication>
#include <QHBoxLayout>

#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgGA/Device>

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

#include <osgQt/GraphicsWindowQt>

#include <iostream>

ViewerWidget::ViewerWidget(osg::ArgumentParser & arguments, QWidget * parent)
    : QMainWindow(parent)
    , osgViewer::CompositeViewer(arguments)
    , _timer(new QTimer(this))
{
    int screenNum = -1;
    while (arguments.read("--screen",screenNum)) {}

    int x = -1, y = -1, width = -1, height = -1;
    while (arguments.read("--window",x,y,width,height)) {}

#if QT_VERSION >= 0x050000
    // Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
#endif

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    _mainGW = createGraphicsWindow(0,0,QMainWindow::width(),QMainWindow::height());
    setCentralWidget(_mainGW->getGLWidget());

    _mainView = new osgViewer::View;
    addView( _mainView );

    osg::Camera* camera = _mainView->getCamera();
    camera->setGraphicsContext( _mainGW );

    const osg::GraphicsContext::Traits* traits = _mainGW->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    _mainView->addEventHandler( new osgViewer::StatsHandler );
    _mainView->setCameraManipulator( new osgGA::TrackballManipulator );

    if(x >= 0 && y >= 0 && width >= 0 && height >= 0)
    {
        setGeometry(x, y, width, height);
    }

    connect( _timer, SIGNAL(timeout()), this, SLOT(onTimer()) );
    _timer->start( 10 );
}

ViewerWidget::~ViewerWidget()
{
}

osgViewer::View * ViewerWidget::mainView()
{
    return _mainView.get();
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    return new osgQt::GraphicsWindowQt(traits.get());
}

void ViewerWidget::onTimer()
{
    frame();
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{
    QMainWindow::paintEvent(event);
    frame();
}

int main( int argc, char** argv )
{
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
    arguments.getApplicationUsage()->addCommandLineOption("--window <x y w h>","Set the position (x,y) and size (w,h) of the viewer window.");

    QApplication app(argc, argv);
    ViewerWidget * mainWidget = new ViewerWidget(arguments);

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

    osgViewer::View * mainView = mainWidget->mainView();

    std::string device;
    while(arguments.read("--device", device))
    {
        osg::ref_ptr<osgGA::Device> dev = osgDB::readFile<osgGA::Device>(device);
        if (dev.valid())
        {
            mainView->addDevice(dev.get());
        }
    }

    // set up the camera manipulators.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );
        keyswitchManipulator->addMatrixManipulator( '5', "Orbit", new osgGA::OrbitManipulator() );
        keyswitchManipulator->addMatrixManipulator( '6', "FirstPerson", new osgGA::FirstPersonManipulator() );
        keyswitchManipulator->addMatrixManipulator( '7', "Spherical", new osgGA::SphericalManipulator() );

        std::string pathfile;
        double animationSpeed = 1.0;
        while(arguments.read("--speed",animationSpeed) ) {}
        char keyForAnimationPath = '8';
        while (arguments.read("-p",pathfile))
        {
            osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
            if (apm || !apm->valid())
            {
                apm->setTimeScale(animationSpeed);

                unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
                keyswitchManipulator->selectMatrixManipulator(num);
                ++keyForAnimationPath;
            }
        }

        mainView->setCameraManipulator( keyswitchManipulator.get() );
    }

    bool doArbSync = arguments.read("--sync");

    // add the state manipulator
    mainView->addEventHandler( new osgGA::StateSetManipulator(mainView->getCamera()->getOrCreateStateSet()) );

    // add the thread model handler
    mainView->addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    mainView->addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    mainView->addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    mainView->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    // add the record camera path handler
    mainView->addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    mainView->addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    mainView->addEventHandler(new osgViewer::ScreenCaptureHandler);

    // load the data
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
        std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
        return 1;
    }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }


    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    mainView->setSceneData( loadedModel.get() );

    mainWidget->show();
    return app.exec();
}
