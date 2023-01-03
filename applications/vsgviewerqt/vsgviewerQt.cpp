#include <osg/GL>
#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include "vsgviewerQt.h"
#include <QTimer>
#include <QFileInfo>
#include <QApplication>
#include <QBuffer>
#include <QDir>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QThread>
#include <QWindow>
#include <QWidget>
#include <QResizeEvent>
#include <QDebug>


#include <iostream>
#include <iomanip>

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/helpers/vsg>

#include <vsg/utils/CommandLine.h>
#include <vsg/app/Viewer.h>
#include <vsg/app/CloseHandler.h>
#include <vsg/app/Trackball.h>
#include <vsg/utils/ComputeBounds.h>

#ifdef SGI_USE_VSGQT
#include <vsg/ui/PointerEvent.h>
#include <vsgQt/ViewerWindow.h>
#endif

void setupWidgetAutoCloseTimer(QWidget * widget, int milliseconds)
{
    QTimer * closeTimer = new QTimer();
    QObject::connect(closeTimer, SIGNAL(timeout()), widget, SLOT(close()));
    closeTimer->setSingleShot(true);
    closeTimer->start(milliseconds);
}




ViewerWidget::ViewerWidget(ViewerWidget * parent, bool shared, GLContextImpl impl)
    : QMainWindow(parent)
    , _timer(nullptr)
    , _viewer(parent->_viewer)
    , _helper(parent->_helper)
{

    _viewerWindow = new vsgQt::ViewerWindow();

#if QT_HAS_VULKAN_SUPPORT
    // if required set the QWindow's SurfaceType to QSurface::VulkanSurface.
    _viewerWindow->setSurfaceType(QSurface::VulkanSurface);
#endif


    auto windowTraits = vsg::WindowTraits::create();
    windowTraits->windowTitle = "vsgQt viewer";
    /*
    windowTraits->debugLayer = arguments.read({"--debug", "-d"});
    //windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
    if (arguments.read({"--fullscreen", "--fs"})) windowTraits->fullscreen = true;
    if (arguments.read({"--window", "-w"}, windowTraits->width,windowTraits->height))
    {
        windowTraits->fullscreen = false;
    }
    */
    _viewerWindow->traits = windowTraits;
    auto horizonMountainHeight = 0.0; //arguments.value(0.0, "--hmh");

    vsg::ref_ptr<vsg::Node> vsg_scene(new vsg::Node);

    // provide the calls to set up the vsg::Viewer that will be used to render to the QWindow subclass vsgQt::ViewerWindow
    _viewerWindow->initializeCallback = [&](vsgQt::ViewerWindow& vw, uint32_t width, uint32_t height) {

        auto& window = vw.windowAdapter;
        if (!window) return false;

        auto& viewer = vw.viewer;
        if (!viewer) viewer = vsg::Viewer::create();

        viewer->addWindow(window);

        // compute the bounds of the scene graph to help position camera
        vsg::ComputeBounds computeBounds;
        vsg_scene->accept(computeBounds);
        vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
        double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
        double nearFarRatio = 0.001;

        // set up the camera
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

        vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
        vsg::ref_ptr<vsg::EllipsoidModel> ellipsoidModel(vsg_scene->getObject<vsg::EllipsoidModel>("EllipsoidModel"));
        if (ellipsoidModel)
        {
            perspective = vsg::EllipsoidPerspective::create(
                lookAt, ellipsoidModel, 30.0,
                static_cast<double>(width) /
                    static_cast<double>(height),
                nearFarRatio, horizonMountainHeight);
        }
        else
        {
            perspective = vsg::Perspective::create(
                30.0,
                static_cast<double>(width) /
                    static_cast<double>(height),
                nearFarRatio * radius, radius * 4.5);
        }

        auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

        // add close handler to respond the close window button and pressing escape
        viewer->addEventHandler(vsg::CloseHandler::create(viewer));

        // add trackball to enable mouse driven camera view control.
        viewer->addEventHandler(vsg::Trackball::create(camera, ellipsoidModel));

        auto commandGraph = vsg::createCommandGraphForView(window, camera, vsg_scene);
        viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

        viewer->compile();

        return true;
    };

    // provide the calls to invokve the vsg::Viewer to render a frame.
    _viewerWindow->frameCallback = [](vsgQt::ViewerWindow& vw) {

        if (!vw.viewer || !vw.viewer->advanceToNextFrame())
        {
            return false;
        }

        // pass any events into EventHandlers assigned to the Viewer
        vw.viewer->handleEvents();

        vw.viewer->update();

        vw.viewer->recordAndSubmit();

        vw.viewer->present();

        return true;
    };

}

ViewerWidget::ViewerWidget(vsg::CommandLine & arguments, QWidget * parent)
    : QMainWindow(parent)
    , _timer(nullptr)
    , _viewWidget(nullptr)
    , _helper(arguments)
	, _impl(GLContextImplDefault)
{
    int screenNum = -1;
    while (arguments.read("--screen", screenNum)) {}

    int x = -1, y = -1, width = -1, height = -1;
    while (arguments.read("--window", x, y, width, height)) {}

    bool useMainThread = false;
    if (arguments.read("--use-main-thread"))
        useMainThread = true;

	QRect rc = childrenRect();
	int w = rc.width();
	int h = rc.height();
	if (w == 0)
		w = QMainWindow::width();
	if (h == 0)
		h = QMainWindow::height();

    _mainGW = createGraphicsWindow(0, 0, w, h, nullptr, _impl);
    _viewWidget = getWidgetForGraphicsWindow(_mainGW.get());

    _view = new vsgViewer::View;

    setCentralWidget(_viewWidget);
    if(_viewWidget)
        _viewWidget->setProperty("sgi_skip_object", true);
}

ViewerWidget::~ViewerWidget()
{
    delete _timer;
}


vsgQt::ViewerWindow * ViewerWidget::createGraphicsWindow( int x, int y, int w, int h,
                                                              vsgQt::ViewerWindow * sharedContext,
                                                              GLContextImpl impl)
{
    auto traits = vsg::WindowTraits::create();
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;

#if 0
    switch(_helper.glprofile)
    {
    default:
    case sgi_MapNodeHelper::GLContextProfileNone:
        traits->glContextProfileMask = 0;
        break;
    case sgi_MapNodeHelper::GLContextProfileCore:
        traits->glContextVersion = "3.3";
        traits->glContextProfileMask = GL_CONTEXT_CORE_PROFILE_BIT_ARB;
        traits->sampleBuffers = 1;
        break;
    case sgi_MapNodeHelper::GLContextProfileCompatibility:
        traits->glContextVersion = "3.3";
        traits->glContextProfileMask = GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        traits->sampleBuffers = 1;
        break;
    }
    if(!_helper.glversion.empty())
        traits->glContextVersion = _helper.glversion;
#endif

    vsgQt::ViewerWindow * ret = new vsgQt::ViewerWindow;

#if QT_HAS_VULKAN_SUPPORT
    // if required set the QWindow's SurfaceType to QSurface::VulkanSurface.
    ret->setSurfaceType(QSurface::VulkanSurface);
#endif
    ret->traits = traits;

    if(!_viewer)
        _viewer = vsg::Viewer::create();

    ret->viewer = _viewer;

    return ret;
}

void ViewerWidget::init()
{
}

void ViewerWidget::onTimer()
{
}

bool ViewerWidget::createCamera()
{
    return true;
}

void ViewerWidget::setData(vsg::Node * node)
{

}


void ViewerWidget::showEvent(QShowEvent * event)
{
    QMainWindow::showEvent(event);
    _viewWidget->updateGeometry();
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{
    QMainWindow::paintEvent(event);
    //_viewer->frame();
}

void ViewerWidget::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);

}


int
main(int argc, char** argv)
{
    int ret = 0;
#ifndef _WIN32
    // QApplication constructor shall call X11InitThreads to get OpenGL working with
    // multi-threading.
    QApplication::setAttribute (Qt::AA_X11InitThreads);
#endif // _WIN32

    QApplication app( argc, argv );
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    {
        QString path = QCoreApplication::applicationDirPath();
        path = QDir::cleanPath(path + SGI_QT_PLUGIN_DIR);
        QCoreApplication::addLibraryPath(path);
        QImage load_sgi;
        QBuffer dummyMem;
        load_sgi.load(&dummyMem, "sgi_loader");
    }

    bool allocConsole = false;

    vsg::CommandLine arguments(&argc,argv);
    if ( arguments.read("--console") )
        allocConsole = true;

    if(allocConsole)
    {
#ifdef _WIN32
        ::AllocConsole();
        freopen("CONIN$", "r",stdin);
        freopen("CONOUT$", "w",stdout);
        freopen("CONOUT$", "w",stderr);
#endif // _WIN32
    }

    initializeNotifyLevels(arguments);
    ViewerWidget * myMainWindow = new ViewerWidget(arguments);

    bool addSceneGraphInspector = true;
    bool showSceneGraphInspector = true;
    bool fullscreen = false;
    bool orderIndependantTransparency = false;
    int autoCloseTime = -1;

    if ( arguments.read("--nosgi") )
        addSceneGraphInspector = false;
    if ( arguments.read("--hidesgi") )
        showSceneGraphInspector = false;
    if (arguments.read("--oit"))
        orderIndependantTransparency = true;

    if ( !arguments.read("--autoclose", autoCloseTime) )
        autoCloseTime = -1;


    myMainWindow->createCamera();


    sgi_MapNodeHelper & helper = myMainWindow->helper();
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags
    vsg::Group * node = helper.load( arguments /*, view */);
    if ( node )
    {

        ret = app.exec();
        //viewer->removeView(view);
        delete myMainWindow;
    }
    else
    {
        QString msg;
        msg = QString::fromStdString(helper.errorMessages());

        // delete main window after taking the error message
        delete myMainWindow;

        msg += "\r\nCmdline: ";
        for(int i = 0; i < argc; i++)
        {
            if(i > 0)
                msg += ' ';
            msg += QString::fromLocal8Bit(argv[i]);
        }
        msg += "\r\n";
        msg += QString::fromStdString(helper.usage());
#ifdef _WIN32
        // Only Windows we have to show a message box with the usage information, because
        // on Windoof a GUI application does not have a console output.
        if(allocConsole)
            std::cerr << msg.toStdString() << std::endl;
        QMessageBox::information(NULL, QCoreApplication::applicationFilePath(), msg);
#else
        std::cerr << msg.toStdString() << std::endl;
#endif
        ret = 1;
    }
    if(allocConsole)
    {
#ifdef _WIN32
        FreeConsole();
#endif // _WIN32
    }
    return ret;
}
