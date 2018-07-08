#include <osg/GL>
#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include "osgviewerQt.h"
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
#include <QDebug>

#include <osg/ValueObject>
#include <osg/ImageSequence>
#include <osg/TextureRectangle>
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
#include <osgDB/FileNameUtils>
#include <osgUtil/Optimizer>

#ifdef SGI_USE_OSGEARTH
#include <osgEarth/Notify>
#include <osgEarth/MapNode>
#include <osgEarth/MapFrame>
#include <osgEarth/Registry>
#include <osgEarth/TerrainEngineNode>

#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#endif // SGI_USE_OSGEARTH

#include <iostream>
#include <iomanip>
#include <osg/io_utils>

#include <sgi/helpers/osg_helper_nodes>
#include <sgi/plugins/SGIItemBase.h>
#include <sgi/helpers/osg>

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#endif

#ifdef _WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#elif defined(__linux__)
#include <osgViewer/api/X11/GraphicsWindowX11>
#undef KeyPress
#endif

#include <sgi_viewer_base.h>

#define GL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define GL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002



void setupWidgetAutoCloseTimer(QWidget * widget, int milliseconds)
{
    QTimer * closeTimer = new QTimer();
    QObject::connect(closeTimer, SIGNAL(timeout()), widget, SLOT(close()));
    closeTimer->setSingleShot(true);
    closeTimer->start(milliseconds);
}

class CompositeViewerThread : public QThread
{
public:
    enum { DEFAULT_FRAME_INTERVAL = 25 };

public:
    /// @brief constructor
    /// @param viewer pointer to composite viewer
    CompositeViewerThread(osgViewer::CompositeViewer * viewer, int frameInterval = DEFAULT_FRAME_INTERVAL)
        : QThread()
        , m_viewer(viewer)
        , m_frameInterval(frameInterval)
    {
    }

public:
    void start()
    {
        if (!QThread::isRunning())
            QThread::start();
    }

    void stop(bool wait = false)
    {
        m_viewer->setDone(true);
        quit();
        if (wait)
            QThread::wait();
    }

    virtual void run()
    {
        //OpenThreads::Thread::setCurrentThreadName("CompVwr");
        m_viewer->setDone(false);
        //osgQt::setViewer(m_viewer);
        QTimer * timer = new QTimer;
        connect(timer, &QTimer::timeout, this, &CompositeViewerThread::onTimer, Qt::DirectConnection);
        timer->start(m_frameInterval);
        exec();
        delete timer;
    }

    void onTimer()
    {
        m_viewer->frame();
        if (m_viewer->done())
            quit();
    }

private:
    osgViewer::CompositeViewer *  m_viewer;
    int                 m_frameInterval;
};


ViewerWidget::ViewerWidget(ViewerWidget * parent, bool shared)
    : QMainWindow(parent)
    , _viewer(parent->_viewer)
    , _thread(nullptr)
    , _timer(nullptr)
{
    _mainGW = createGraphicsWindow(0,0,QMainWindow::width(),QMainWindow::height(), (shared)?parent->_mainGW.get():nullptr);

#ifdef SGI_USE_OSGQT
    osgQt::GraphicsWindowQt* gwq = dynamic_cast<osgQt::GraphicsWindowQt*>(_mainGW.get());
    if(gwq)
        setCentralWidget(gwq->getGLWidget());
#endif

    _view = new osgViewer::View;

    osg::Camera* camera = _view->getCamera();
    camera->setGraphicsContext( _mainGW );

    const osg::GraphicsContext::Traits* traits = _mainGW->getTraits();

    camera->setClearColor( osg::Vec4(0.2f, 0.2f, 0.6f, 1.0f) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 10000.0 );

    _viewer->addView(_view);
}

ViewerWidget::ViewerWidget(osg::ArgumentParser & arguments, QWidget * parent)
    : QMainWindow(parent)
    , _viewer(new osgViewer::CompositeViewer(arguments))
    , _thread(nullptr)
    , _timer(nullptr)
    , _viewWidget(nullptr)
{
    int screenNum = -1;
    while (arguments.read("--screen", screenNum)) {}

    int x = -1, y = -1, width = -1, height = -1;
    while (arguments.read("--window", x, y, width, height)) {}

    bool useMainThread = false;
    if (arguments.read("--use-main-thread"))
        useMainThread = true;

    GLContextProfile glprofile = GLContextProfileNone;
    std::string glver;
    arguments.read("--glver", glver);
    if (arguments.read("--core"))
        glprofile = GLContextProfileCore;
    if (arguments.read("--compat"))
        glprofile = GLContextProfileCompatibility;

    _viewer->setThreadingModel(osgViewer::CompositeViewer::ThreadingModel::DrawThreadPerContext);

    // disable the default setting of viewer.done() by pressing Escape.
    _viewer->setKeyEventSetsDone(0);

    _mainGW = createGraphicsWindow(0, 0, QMainWindow::width(), QMainWindow::height(), nullptr, glver, glprofile);

    _viewWidget = getWidgetForGraphicsWindow(_mainGW.get());
    setCentralWidget(_viewWidget);
    if(_viewWidget)
        _viewWidget->setProperty("sgi_skip_object", true);

    _view = new osgViewer::View;

    osg::Camera* camera = _view->getCamera();
    camera->setGraphicsContext(_mainGW);

    const osg::GraphicsContext::Traits* traits = _mainGW ? _mainGW->getTraits() : nullptr;
    int widget_width = traits ? traits->width : 100;
    int widget_height = traits ? traits->height : 100;
    camera->setClearColor(osg::Vec4(0.2f, 0.2f, 0.6f, 1.0f));
    camera->setViewport(new osg::Viewport(0, 0, widget_width, widget_height));
    camera->setProjectionMatrixAsPerspective(30.0, static_cast<double>(widget_width) / static_cast<double>(widget_height), 1.0, 10000.0);
    GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
    camera->setDrawBuffer(buffer);
    camera->setReadBuffer(buffer);


    if (x >= 0 && y >= 0 && width >= 0 && height >= 0)
    {
        setGeometry(x, y, width, height);
    }
    _viewer->addView(_view);
    _viewer->realize();

    if (useMainThread)
    {
        _timer = new QTimer(this);
        connect(_timer, &QTimer::timeout, this, &ViewerWidget::onTimer, Qt::DirectConnection);
        _timer->start(CompositeViewerThread::DEFAULT_FRAME_INTERVAL);
    }
    else
    {
        _thread = new CompositeViewerThread(_viewer);
        _thread->start();
    }
    if(_viewWidget)
        _viewWidget->setFocus();
}

ViewerWidget::~ViewerWidget()
{
    delete _timer;
    if (_thread->isRunning())
    {
        _thread->quit();
        _thread->wait();
    }
    delete _thread;
}

void ViewerWidget::init()
{
}

void ViewerWidget::onTimer()
{
    _viewer->frame();
    if (_viewer->done())
        QApplication::quit();
}

osgViewer::View * ViewerWidget::view()
{
    return _view.get();
}

bool ViewerWidget::createCamera()
{
    return true;
}

void ViewerWidget::setData(osg::Node * node)
{
    _view->setSceneData(node);
}

osgViewer::GraphicsWindow* ViewerWidget::createGraphicsWindow( int x, int y, int w, int h,
                                                               osg::GraphicsContext * sharedContext,
                                                               const std::string& glver,
                                                               GLContextProfile profile,
                                                               const std::string& name,
                                                               bool windowDecoration)
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
    traits->sharedContext = sharedContext;

    switch(profile)
    {
    default:
    case GLContextProfileNone:
        traits->glContextProfileMask = 0;
        break;
    case GLContextProfileCore:
        traits->glContextVersion = "3.3";
        traits->glContextProfileMask = GL_CONTEXT_CORE_PROFILE_BIT_ARB;
        traits->sampleBuffers = 1;
        break;
    case GLContextProfileCompatibility:
        traits->glContextVersion = "3.3";
        traits->glContextProfileMask = GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        traits->sampleBuffers = 1;
        break;
    }
    if(!glver.empty())
        traits->glContextVersion = glver;

    osgViewer::GraphicsWindow* ret = nullptr;
#ifdef SGI_USE_OSGQT
    ret = new osgQt::GraphicsWindowQt(traits.get());
#else
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi)
    {
        OSG_NOTICE << "SingleWindow::configure() : Error, no WindowSystemInterface available, cannot create windows." << std::endl;
        return nullptr;
    }
    osg::GraphicsContext * gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    ret = dynamic_cast<osgViewer::GraphicsWindow*>(gc);
#endif
    if(ret)
    {
        switch(profile)
        {
        default:
        case GLContextProfileNone:
            break;
        case GLContextProfileCore:
        case GLContextProfileCompatibility:

            // for non GL3/GL4 and non GLES2 platforms we need enable the osg_ uniforms that the shaders will use,
            // you don't need thse two lines on GL3/GL4 and GLES2 specific builds as these will be enable by default.
            ret->getState()->setUseModelViewAndProjectionUniforms(true);
            //ret->getState()->setUseVertexAttributeAliasing(true);
            break;
        }
    }
    return ret;
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{
    QMainWindow::paintEvent(event);
    _viewer->frame();
}

CreateViewHandlerProxy::CreateViewHandlerProxy(CreateViewHandler * handler, QObject * parent)
    : QObject(parent)
{
    connect(this, &CreateViewHandlerProxy::triggerClone, this, &CreateViewHandlerProxy::viewCloneImpl);
}

void CreateViewHandlerProxy::viewCloneImpl(osgViewer::View * source, bool shared)
{
    osg::Camera * sourceCamera = source->getCamera();
    ViewerWidget * sourceWidget = nullptr;


#ifdef SGI_USE_OSGQT
    osgQt::GraphicsWindowQt* ctx = dynamic_cast<osgQt::GraphicsWindowQt*>(sourceCamera->getGraphicsContext());
    if (ctx)
    {
        QWidget * w = ctx->getGLWidget();
        if (w)
            sourceWidget = dynamic_cast<ViewerWidget*>(w->parentWidget());
    }
#endif

    ViewerWidget * nextwidget = new ViewerWidget(sourceWidget, shared);
    nextwidget->createCamera();
    osgViewer::View* nextview = nextwidget->view();

    // configure the near/far so we don't clip things that are up close
    osg::Camera * camera = nextview->getCamera();
    camera->setNearFarRatio(sourceCamera->getNearFarRatio());
    camera->setClearColor(sourceCamera->getClearColor());
    camera->setClearMask(sourceCamera->getClearMask());

    nextview->setSceneData(source->getSceneData());

    nextview->setCameraManipulator(source->getCameraManipulator());
    for (auto evh : source->getEventHandlers())
        nextview->addEventHandler(evh);

    if (sourceWidget)
        nextwidget->setGeometry(sourceWidget->geometry());

    nextwidget->show();
}

CreateViewHandler::CreateViewHandler(QObject * parent)
    : _proxy(new CreateViewHandlerProxy(this, parent))
{
}

CreateViewHandler::~CreateViewHandler()
{
}

bool CreateViewHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (!view) return false;
    switch (ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::KEYUP):
        if (ea.getKey() == 'v' && ea.getModKeyMask() == 0)
        {
            emit _proxy->triggerClone(view, false);
        }
        else if (ea.getKey() == 'V' || (ea.getKey() == 'v' && (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)))
        {
            emit _proxy->triggerClone(view, true);
        }
        break;
    default:
        break;
    }
    return false;
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

    osg::ArgumentParser arguments(&argc,argv);
    if ( arguments.read("--nostencil") )
        osg::DisplaySettings::instance()->setMinimumNumStencilBits(0);
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

    osgViewer::View * view = myMainWindow->view();

    //Tell the database pager to not modify the unref settings
    view->getDatabasePager()->setUnrefImageDataAfterApplyPolicy( false, false );

    myMainWindow->createCamera();

    // configure the near/far so we don't clip things that are up close
    view->getCamera()->setNearFarRatio(0.00002);
    //view->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    sgi_MapNodeHelper helper;
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags
    osg::Node* node = helper.load( arguments, view );
    if ( node )
    {
        bool showImagePreviewDialog = helper.onlyImages() ? true : false;
		osg::ref_ptr<osg::Group> root = new osg::Group;
		root->addChild(node);

        if(addSceneGraphInspector)
        {
            osg::ref_ptr<osgDB::Options> opts = osgDB::Registry::instance()->getOptions();
            if(opts)
                opts = opts->cloneOptions();
            else
                opts = new osgDB::Options;
            opts->setPluginStringData("showSceneGraphDialog", showSceneGraphInspector ? "1" : "0");
            opts->setPluginStringData("showImagePreviewDialog", showImagePreviewDialog ? "1" : "0");
            osg::ref_ptr<osg::Node> sgi_loader = osgDB::readRefNodeFile(".sgi_loader", opts);
            if(sgi_loader.valid())
                root->addChild(sgi_loader);
            else
                QMessageBox::information(myMainWindow, QCoreApplication::applicationFilePath(), "Failed to load SGI");
        }

		myMainWindow->setData(root);

        if(fullscreen)
            myMainWindow->showFullScreen();
        else
        {
            myMainWindow->setGeometry(200, 200, 800, 600);
            myMainWindow->show();
        }
        if(autoCloseTime >= 0)
            setupWidgetAutoCloseTimer(myMainWindow, autoCloseTime);
        helper.setupInitialPosition(view);

        view->addEventHandler(new CreateViewHandler(myMainWindow));

        QString filelist;
        for (const auto & f : helper.files())
        {
            if (!filelist.isEmpty())
                filelist += QChar(',');
            filelist += QString::fromStdString(f);
        }

        myMainWindow->setWindowTitle("osgViewerQt - " + filelist);

        ret = app.exec();
        //viewer->removeView(view);
        delete myMainWindow;
    }
    else
    {
        delete myMainWindow;

        QString msg;
        msg = QString::fromStdString(helper.errorMessages());
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
