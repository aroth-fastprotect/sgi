// kate: syntax C++;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgUtil/CullVisitor>
#include <osgViewer/View>
#include <osg/ValueObject>

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <QDebug>

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#endif
#if defined(_WIN32)
#include <osgViewer/api/win32/GraphicsWindowWin32>
#elif defined(__APPLE__)
#if !__LP64__
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
#endif // !__LP64__
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
#elif  defined(__linux__)
#include <osgViewer/api/X11/GraphicsWindowX11>
// undefine Status from X11/ICELIB
#undef Status
#include <signal.h>
#include <time.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>
#include <sgi/ObjectLoggerDialog>
#include <sgi/ImagePreviewDialog>
#include <sgi/ContextMenu>
#include <sgi/AutoLoadOsg>
#include <sgi/GenerateItem>
#include <sgi/Shutdown>
#include <sgi/LibraryInfo>
#include <sgi/ReferencedPicker>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>
#include <sgi/helpers/osg_helper_nodes>

#include "../../img/microscope64.c"

#if defined(_DEBUG)
#if defined(_MSC_VER)
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else // defined(_MSC_VER)
#define DEBUG_NEW new
#endif // defined(_MSC_VER)
#endif // defined(_DEBUG)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LC "[ReaderWriteSGI] "

namespace sgi {

namespace {
#ifdef _WIN32
    static void __stdcall win32_app_timer(HWND, UINT, UINT_PTR, DWORD)
    {
        QCoreApplication::instance()->processEvents();
    }
#elif defined(__APPLE__)
#elif defined(__linux__)
    static void x11_app_timer_signal(int sig, siginfo_t*, void*)
    {
        //std::cout << "x11_app_timer_signal" << std::endl;
        QCoreApplication::instance()->processEvents();
    }
    unsigned getCurrentThreadId()
    {
        return (unsigned)::syscall(SYS_gettid);
    }
#endif
    static void ensure_QApplication(osg::GraphicsContext * ctx)
    {
        Q_UNUSED(ctx);
        const int application_timer_interval_in_ms = 50;
        QCoreApplication * app = QApplication::instance();
        if(!app)
        {
#if defined(_WIN32)
            if (osgViewer::GraphicsWindowWin32 * gwwin = dynamic_cast<osgViewer::GraphicsWindowWin32*>(ctx))
            {
                SetTimer(gwwin->getHWND(), /* magic timer id */100232u, application_timer_interval_in_ms, win32_app_timer);
            }
#elif defined(__APPLE__)
#if !__LP64__
            if (osgViewer::GraphicsWindowCarbon * gwcarbon = dynamic_cast<osgViewer::GraphicsWindowCarbon*>(ctx))
            {
            }
            else
#endif // !__LP64__
            if (osgViewer::GraphicsWindowCocoa * gwcocoa = dynamic_cast<osgViewer::GraphicsWindowCocoa*>(ctx))
            {
            }
#elif defined(__linux__)
            int err;
            struct sigaction sa;
            sa.sa_flags = SA_SIGINFO;
            sa.sa_sigaction = x11_app_timer_signal;
            sigemptyset(&sa.sa_mask);
            err = sigaction(SIGRTMIN, &sa, nullptr);

            struct sigevent sevp;
            timer_t timerid;
            memset(&sevp, 0, sizeof(sevp));
            sevp.sigev_notify=SIGEV_THREAD_ID;
            sevp.sigev_signo = SIGRTMIN;
            sevp._sigev_un._tid = getCurrentThreadId();
            err = ::timer_create(CLOCK_REALTIME, &sevp, &timerid);
            if(err == 0)
            {
                struct itimerspec in, out;
                in.it_value.tv_sec = 0;
                in.it_value.tv_nsec = (__syscall_slong_t)application_timer_interval_in_ms * 1000 * 1000; // 40 ms -> us -> ns
                in.it_interval = in.it_value;
                //issue the periodic timer request here.
                err = ::timer_settime(timerid, 0, &in, &out);
            }
#endif

            static int argc = 1;
            static char * argv[] = { (char*)"/dev/null", nullptr };
            new QApplication(argc, argv);
        }
    }

}

struct SGIOptions
{
    static bool string_to_bool(const std::string & s, bool defaultValue=false)
    {
        if(s.compare("1") == 0 || s.compare("on") == 0 || s.compare("true") == 0)
            return true;
        else if(s.compare("0") == 0 || s.compare("off") == 0 || s.compare("false") == 0)
            return false;
        else
            return defaultValue;
    }
    template<typename T>
    static T * getObjectOption(const osgDB::Options * options, const std::string & key, T * defaultValue=nullptr, bool * gotOption=nullptr)
    {
        if (!options)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        const void * data = options->getPluginData(key);
        if (!data)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        if (gotOption)
            *gotOption = true;
        return static_cast<T*>(const_cast<void*>(data));
    }
    template<typename T>
    static T getOption(const osgDB::Options * options, const std::string & key, const T & defaultValue=T(), bool * gotOption = nullptr)
    {
        if (!options)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        const void * data = options->getPluginData(key);
        if (!data)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        if (gotOption)
            *gotOption = true;
        return *static_cast<const T*>(data);
    }
    SGIOptions(const std::string & filename_=std::string(), const osgDB::Options * options=nullptr);
    SGIHostItemBase * getHostItem() const;

    void clear()
    {
        hostCallback = nullptr;
        showSceneGraphDialog = false;
        showImagePreviewDialog = false;
        qtObject = nullptr;
        parentWidget = nullptr;
        osgReferenced = nullptr;
        filename.clear();
        usePickerNodeMask = false;
        pickerNodeMask = 0;
        pickerRoot = nullptr;
    }

    sgi::IHostCallbackPtr hostCallback;
    bool showSceneGraphDialog;
    bool showImagePreviewDialog;
	QObject * qtObject;
    QWidget * parentWidget;
	osg::ref_ptr<osg::Referenced> osgReferenced;
    std::string filename;
    bool usePickerNodeMask;
    unsigned pickerNodeMask;
    osg::ref_ptr<osg::Node> pickerRoot;
};

template<>
bool SGIOptions::getOption<bool>(const osgDB::Options * options, const std::string & key, const bool & defaultValue, bool * gotOption)
{
    if (!options)
    {
        if (gotOption)
            *gotOption = false;
        return defaultValue;
    }
    std::string val = options->getPluginStringData(key);
    if(val.empty())
    {
        if (gotOption)
            *gotOption = false;
        return defaultValue;
    }
    if (gotOption)
        *gotOption = true;
    return string_to_bool(val, defaultValue);
}

SGIOptions::SGIOptions(const std::string & filename_, const osgDB::Options * options)
        : qtObject(nullptr), filename(filename_), usePickerNodeMask(false), pickerNodeMask(~0u)
{
    hostCallback = getObjectOption<sgi::IHostCallback>(options, "sgi_host_callback");
    if(filename == "sgi-info")
    {
        showSceneGraphDialog = true;
        showImagePreviewDialog = false;
    }
    else
    {
        showSceneGraphDialog = getOption<bool>(options, "showSceneGraphDialog");
        showImagePreviewDialog = getOption<bool>(options, "showImagePreviewDialog");
    }
    osgReferenced = getObjectOption<osg::Referenced>(options, "sgi_osg_referenced");
    qtObject = getObjectOption<QObject>(options, "sgi_qt_object");
    parentWidget = getObjectOption<QWidget>(options, "parentWidget");
    pickerNodeMask = getOption<unsigned>(options, "pickerNodeMask", ~0u, &usePickerNodeMask);
    pickerRoot = getObjectOption<osg::Node>(options, "pickerRoot");
}

SGIHostItemBase * SGIOptions::getHostItem() const
{
    SGIHostItemBase * ret = nullptr;
    if(!filename.empty())
    {
        std::cout << __FUNCTION__ << " " << filename << std::endl;
        if(filename == "qapp" || filename == "app")
            ret = new SGIHostItemQt(qApp);
        else if(filename == "sgi-info")
        {
            sgi::details::Referenced * libinfo = sgi::libraryInfoObject<sgi::autoload::Osg>();
            ret = new SGIHostItemInternal(libinfo);
        }
    }
    if(!ret)
    {
        if (osgReferenced.valid())
            ret = new SGIHostItemOsg(osgReferenced.get());
        else if (qtObject)
            ret = new SGIHostItemQt(qtObject);
    }
    return ret;
}


inline QDebug operator<<(QDebug dbg, const sgi::SGIOptions & opts)
{
    dbg << "showSceneGraphDialog" << opts.showSceneGraphDialog
        << "showImagePreviewDialog" << opts.showImagePreviewDialog
           ;
    return dbg;
}

class SceneGraphInspectorHandler : public osgGA::GUIEventHandler
{
public:
	SceneGraphInspectorHandler(IHostCallback * callback, const SGIOptions & options);
	virtual ~SceneGraphInspectorHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	IHostCallback * getHostCallback() { return _hostCallback.get(); }
    void shutdown();

protected:
    bool                showSceneGraphDialog(const SGIHostItemBase * item, QWidget * parent=nullptr);
    bool                showObjectLoggerDialog(const SGIHostItemBase * item, QWidget * parent=nullptr);
    bool                contextMenu(const SGIHostItemBase * item, float x, float y, QWidget * parent=nullptr);

protected:
	IHostCallbackPtr    _hostCallback;
	const SGIOptions &	_options;
	ReferencedPickerBasePtr _picker;
	int                 _inspectorHitTestKey;
	int                 _inspectorEventKey;
	int                 _inspectorLoggerKey;
	int                 _inspectorLoggerModMask;
	int                 _inspectorInfoKey;
	int                 _inspectorInfoKeyModMask;
	int                 _inspectorHitTestMouseButton;
	int                 _inspectorHitTestMouseLeftModMask;
	int                 _inspectorHitTestMouseRightModMask;
	int                 _inspectorContextMenuMouseButton;
	int                 _inspectorContextMenuMouseLeftModMask;
	int                 _inspectorContextMenuMouseRightModMask;
};

SceneGraphInspectorHandler::SceneGraphInspectorHandler(IHostCallback * callback, const SGIOptions & options)
	: _hostCallback(callback)
	, _options(options)
	, _picker()
	, _inspectorHitTestKey('i')
	, _inspectorEventKey('e')
	, _inspectorLoggerKey('l')
	, _inspectorLoggerModMask(osgGA::GUIEventAdapter::MODKEY_SHIFT)
	, _inspectorInfoKey('Q')
	, _inspectorInfoKeyModMask(osgGA::GUIEventAdapter::MODKEY_SHIFT)
	, _inspectorHitTestMouseButton(osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	, _inspectorHitTestMouseLeftModMask(osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL | osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT)
	, _inspectorHitTestMouseRightModMask(osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL | osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT)
	, _inspectorContextMenuMouseButton(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	, _inspectorContextMenuMouseLeftModMask(osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL | osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT)
	, _inspectorContextMenuMouseRightModMask(osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL | osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT)
{
}

SceneGraphInspectorHandler::~SceneGraphInspectorHandler()
{
}

void SceneGraphInspectorHandler::shutdown()
{
    _hostCallback = nullptr;
    _picker = nullptr;
}

bool SceneGraphInspectorHandler::showSceneGraphDialog(const SGIHostItemBase * item, QWidget * parent)
{
    ISceneGraphDialogPtr dialog = _hostCallback->showSceneGraphDialog(parent, item);
    if(dialog.valid())
        dialog->show();
    return dialog.valid();
}

bool SceneGraphInspectorHandler::showObjectLoggerDialog(const SGIHostItemBase * item, QWidget * parent)
{
    return _hostCallback->showObjectLoggerDialog(parent, item) != nullptr;
}

bool SceneGraphInspectorHandler::contextMenu(const SGIHostItemBase * item, float x, float y, QWidget * parent)
{
    IContextMenuPtr contextMenu = _hostCallback->contextMenu(parent, item);
	if (contextMenu.valid())
        contextMenu->popup(!parent ? _hostCallback->getFallbackParentWidget() : parent, x, y);
	return contextMenu.valid();
}


bool SceneGraphInspectorHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (ea.getHandled())
		return false;

    bool ret = false;
    switch(ea.getEventType())
    {
    case osgGA::GUIEventAdapter::KEYDOWN:
        {
            if (_inspectorHitTestKey != 0 && ea.getKey() == _inspectorHitTestKey)
            {
                IHostCallback::PickerType pickerType;
                if (ea.getModKeyMask() & (osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT | osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT))
                    pickerType = IHostCallback::PickerTypeLine;
                else
                    pickerType = IHostCallback::PickerTypeDefault;
                _picker = _hostCallback->createPicker(pickerType, ea.getX(), ea.getY());
                if (_picker.valid() && _picker->result())
                {
                    SGIHostItemOsg itemPicker(_picker);
                    ret = showSceneGraphDialog(&itemPicker);
                }
                else
                {
                    ret = showSceneGraphDialog(nullptr);
                }
            }
            else if (_inspectorInfoKey != 0 && ea.getKey() == _inspectorInfoKey &&
                ea.getModKeyMask() & (_inspectorInfoKeyModMask))
            {
                ReferencedInternalInfoData * info = new ReferencedInternalInfoData(InternalInfoData::CommandIdAbout);
                SGIHostItemInternal itemAbout(info);
                ret = showSceneGraphDialog(&itemAbout);
            }
            else if (_inspectorEventKey != 0 && ea.getKey() == _inspectorEventKey)
            {
                SGIHostItemOsg itemEa(&ea);
                ret = showSceneGraphDialog(&itemEa);
            }
            else if (_inspectorLoggerKey != 0 && ea.getKey() == _inspectorLoggerKey &&
                ea.getModKeyMask() & (_inspectorLoggerModMask))
            {
                SGIHostItemOsg itemEa(&ea);
                ret = showObjectLoggerDialog(&itemEa);
            }
        }
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        {
            if (_inspectorHitTestMouseButton != 0 &&
                ea.getButton() == _inspectorHitTestMouseButton &&
                (((ea.getModKeyMask() & _inspectorHitTestMouseRightModMask) == _inspectorHitTestMouseRightModMask) ||
                    ((ea.getModKeyMask() & _inspectorHitTestMouseLeftModMask) == _inspectorHitTestMouseLeftModMask))
                )
            {
                SGIHostItemOsg itemEa(&ea);
                ret = showSceneGraphDialog(&itemEa);
            }
            else if (_inspectorContextMenuMouseButton != 0 &&
                ea.getButton() == _inspectorContextMenuMouseButton &&
                (((ea.getModKeyMask() & _inspectorContextMenuMouseRightModMask) == _inspectorContextMenuMouseRightModMask) ||
                    ((ea.getModKeyMask() & _inspectorContextMenuMouseLeftModMask) == _inspectorContextMenuMouseLeftModMask))
                )
            {
                float x = ea.getX();
                float y = ea.getY();
                osg::View * view = aa.asView();
                osg::Camera * camera = view ? view->getCamera() : nullptr;
                //OSG_FATAL << "RELEASE " << x << ',' << y << " view=" << view << " cam=" << camera << std::endl;
                if (camera)
                {
                    osg::Viewport * viewport = camera->getViewport();
                    if (viewport)
                        y = viewport->height() - y;
                }

                QWidget * parent = nullptr;
                SGIHostItemBasePtr hostItem;
                if (view)
                {
                    QObject * qobject = nullptr;
                    double sgi_qt_object;
                    if (view->getUserValue("sgi_qt_object", sgi_qt_object) && sgi_qt_object)
                        qobject = (QObject*)(void*)(qulonglong)sgi_qt_object;
                    if(qobject)
                        hostItem = new SGIHostItemQt(qobject);
                    else
                        hostItem = new SGIHostItemOsg(view);
                }
                else
                    hostItem = new SGIHostItemOsg(&ea);
                ret = contextMenu(hostItem.get(), x, y, parent);
            }

        }
        break;
    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::MOVE:
    case osgGA::GUIEventAdapter::DRAG:
        {
            if (_inspectorHitTestMouseButton != 0 &&
                ea.getButton() == _inspectorHitTestMouseButton &&
                (((ea.getModKeyMask() & _inspectorHitTestMouseRightModMask) == _inspectorHitTestMouseRightModMask) ||
                    ((ea.getModKeyMask() & _inspectorHitTestMouseLeftModMask) == _inspectorHitTestMouseLeftModMask))
                )
            {
                ret = true;
            }
            else if (_inspectorContextMenuMouseButton != 0 &&
                ea.getButton() == _inspectorContextMenuMouseButton &&
                (((ea.getModKeyMask() & _inspectorContextMenuMouseRightModMask) == _inspectorContextMenuMouseRightModMask) ||
                    ((ea.getModKeyMask() & _inspectorContextMenuMouseLeftModMask) == _inspectorContextMenuMouseLeftModMask))
                )
            {
                ret = true;
            }
        }
        break;
    }
	return ret;
}

class DefaultSGIProxy : public osg::Referenced
{
public:
    DefaultSGIProxy(osg::Camera * camera, const SGIOptions & options)
        : _parent(nullptr)
        , _view(nullptr)
        , _hostCallback(new HostCallbackImpl(this, options.hostCallback.valid()?options.hostCallback.get():sgi::defaultHostCallback<sgi::autoload::Osg>()))
        , _options(options)
        , _inspectorHandler()
    {
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if (_view)
        {
            _inspectorHandler = new sgi::SceneGraphInspectorHandler(_hostCallback.get(), _options);
            _view->addEventHandler(_inspectorHandler.get());
        }
    }
    ~DefaultSGIProxy() override
    {
        _inspectorHandler = nullptr;
        _parent = nullptr;
        _view = nullptr;
        _options.clear();
        _viewPtr = nullptr;
        _hostCallback = nullptr;
    }
    void runInstallOperations()
    {
        osg::Camera * camera = _view->getCamera();
        osg::GraphicsContext * ctx = camera->getGraphicsContext();
        qWarning() << "DefaultSGIProxy runInstallOperations " << camera << ctx;
        if(ctx)
        {
            ensure_QApplication(ctx);

            if(_options.parentWidget)
                _parent = _options.parentWidget;
            else
            {
#ifdef SGI_USE_OSGQT
                if(osgQt::GraphicsWindowQt * gwqt = dynamic_cast<osgQt::GraphicsWindowQt*>(ctx))
                    _parent = gwqt->getGLWidget();
#else
                if(0)
                {
                    void(0);
                }
#endif
#if defined(_WIN32)
                else if(osgViewer::GraphicsWindowWin32 * gwwin = dynamic_cast<osgViewer::GraphicsWindowWin32*>(ctx))
                {
                    double sgi_ctx_widget;
                    if (gwwin->getUserValue("sgi_ctx_widget", sgi_ctx_widget) && sgi_ctx_widget)
                        _parent = (QWidget*)(void*)(qulonglong)sgi_ctx_widget;
                }
#elif defined(__APPLE__)
#if !__LP64__
                else if (osgViewer::GraphicsWindowCarbon * gwcarbon = dynamic_cast<osgViewer::GraphicsWindowCarbon*>(ctx))
                {
                }
#endif // !__LP64__
                else if (osgViewer::GraphicsWindowCocoa * gwcocoa = dynamic_cast<osgViewer::GraphicsWindowCocoa*>(ctx))
                {
                }
#else
                else if(osgViewer::GraphicsWindowX11 * gwx11 = dynamic_cast<osgViewer::GraphicsWindowX11*>(ctx))
                {
                    double sgi_ctx_widget;
                    if(gwx11->getUserValue("sgi_ctx_widget", sgi_ctx_widget) && sgi_ctx_widget)
                        _parent = (QWidget*)(void*)(qulonglong)sgi_ctx_widget;
                }
#endif
            }
            qWarning() << "DefaultSGIProxy parent " << _parent;
        }
        runStartup(_options);
    }

    void runStartup(const SGIOptions & options)
    {
        qWarning() << "DefaultSGIProxy runStartup " << _parent << options;
        if(_parent)
        {
            osg::Camera * camera = _view->getCamera();
            bool gotImage = false;
            if(options.showImagePreviewDialog)
            {
                IImagePreviewDialogPtr dialog;
                osg_helpers::FindTreeItemNodeVisitor ftinv;
                camera->accept(ftinv);
                for(osg_helpers::FindTreeItemNodeVisitor::NodeList::const_iterator it = ftinv.results().begin(); !gotImage && it != ftinv.results().end(); ++it)
                {
                    const osg_helpers::FindTreeItemNodeVisitor::NodeItem & item = *it;
                    if(item.imageGeode)
                    {
                        // ... and if it is a image geode try to add the image to the tree as well
                        SGIHostItemOsg image(item.getImage());
                        if (!image.hasObject())
                            image = SGIHostItemOsg(item.getImageGeodeTexture());
                        if(image.hasObject())
                        {
                            dialog = _hostCallback->showImagePreviewDialog(_parent, &image);
                            gotImage = true;
                        }
                    }
                    else if (item.heightField)
                    {
                        SGIHostItemOsg image(item.getHeightField());
                        if (image.hasObject())
                        {
                            dialog = _hostCallback->showImagePreviewDialog(_parent, &image);
                            gotImage = true;
                        }
                    }
                }
                if(dialog.valid())
                    dialog->show();
            }
            if(options.showSceneGraphDialog && !gotImage)
            {
                ISceneGraphDialogPtr dialog;
                SGIHostItemBasePtr hostItem = options.getHostItem();
                if(hostItem.valid())
                    dialog = _hostCallback->showSceneGraphDialog(_parent, hostItem.get());
                else
                {
                    SGIItemBase * existingViewItem = getView();
                    if (existingViewItem)
                        dialog = _hostCallback->showSceneGraphDialog(_parent, existingViewItem);
                    else if(_view)
                    {
                        SGIHostItemOsg viewItem(_view);
                        dialog = _hostCallback->showSceneGraphDialog(_parent, &viewItem);
                    }
                    else
                    {
                        SGIHostItemOsg cameraItem(camera);
                        dialog = _hostCallback->showSceneGraphDialog(_parent, &cameraItem);
                    }
                }
                if(dialog.valid())
                    dialog->show();
            }
        }
    }
public:
    SGIItemBase * getView()
    {
        if(!_viewPtr.valid() && _view)
        {
            SGIHostItemOsg viewHostItem(_view);
            sgi::generateItem<autoload::Osg>(_viewPtr, &viewHostItem);
        }
        return _viewPtr.get();
    }

    void release()
    {
        HostCallbackImpl* hostCallbackImpl = dynamic_cast<HostCallbackImpl*>(_hostCallback.get());
        if (hostCallbackImpl)
            hostCallbackImpl->release();
    }

    void shutdown()
    {
        osg::ref_ptr<SceneGraphInspectorHandler> handler;
        if (_inspectorHandler.lock(handler))
            handler->shutdown();
        _inspectorHandler = nullptr;
        _parent = nullptr;
        _view = nullptr;
        _options.clear();
        _viewPtr = nullptr;
        _hostCallback = nullptr;
    }

    class HostCallbackImpl : public HostCallbackBase
    {
    public:
        HostCallbackImpl(DefaultSGIProxy * parent, IHostCallback * callback)
            : HostCallbackBase(callback), _parent(parent) 
		{
            // remember the currently installed host callback, so we could restore it later
            sgi::getHostCallback<autoload::Osg>(_previousInstalled);
            // ... now install this new host callback
			sgi::setHostCallback<autoload::Osg>(this);
		}
        ~HostCallbackImpl() override
        {
            OSG_WARN << "~HostCallbackImpl: " << this << " org:" << _original.get() << std::endl;
        }
        ReferencedPickerBase * createPicker(PickerType type, float x, float y) override
        {
            if(!_parent.get() || !_parent->_view)
                return nullptr;

            ReferencedPickerBase * ret = nullptr;
            osg::Node * root = _parent->_options.pickerRoot.get();
            float buffer = 1.0f;
            unsigned traversalMask = _parent->_options.usePickerNodeMask ? _parent->_options.pickerNodeMask : (unsigned)_parent->_view->getCamera()->getCullMask();

            switch(type)
            {
#ifndef SGI_USE_OSGEARTH
            case PickerTypeDefault:
#endif
            case PickerTypeLine:
                ret = new ReferencedLinePicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask);
                break;
#ifdef SGI_USE_OSGEARTH
            case PickerTypeDefault:
                ret = new ReferencedPicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask, buffer);
                break;
#endif
            }
            return ret;
        }
        SGIItemBase * getView() override
        {
            if (_parent.get())
                return _parent->getView();
            else
                return nullptr;
        }
        void triggerRepaint() override
        {
			if(_parent.get() && _parent->_view)
				_parent->_view->requestRedraw();
        }
        QWidget * getFallbackParentWidget() override
        {
            if (_parent.get())
                return _parent->_parent;
            else
                return nullptr;
        }
        void shutdown() override
        {
            if(_parent.get())
                _parent->shutdown();
            HostCallbackBase::shutdown();
        }
        void release()
        {
            IHostCallbackPtr currentHostCallback;
            if (sgi::getHostCallback<autoload::Osg>(currentHostCallback))
            {
                //OSG_WARN << "~HostCallbackImpl::release: " << this << " current:" << currentHostCallback.get() << std::endl;
                if (currentHostCallback.get() == this)
                {
                    //OSG_WARN << "~HostCallbackImpl::release: " << this << " restore:" << _previousInstalled.get() << std::endl;
                    // restore the previously used callback
                    sgi::setHostCallback<autoload::Osg>(_previousInstalled.get());
                    _previousInstalled = nullptr;
                }
            }
        }
    private:
        osg::observer_ptr<DefaultSGIProxy> _parent;
        IHostCallbackPtr _previousInstalled;
    };

private:
    QWidget * _parent;
    osgViewer::View * _view;
    SGIItemBasePtr _viewPtr;
    IHostCallbackPtr _hostCallback;
    sgi::SGIOptions _options;
    osg::observer_ptr<sgi::SceneGraphInspectorHandler> _inspectorHandler;
};
} // namespace sgi

namespace {
    osg::Image * getSGILogoImage()
    {
        osgDB::ReaderWriter * rw = osgDB::Registry::instance()->getReaderWriterForExtension("png");
        std::string microscope64_png_str;
        microscope64_png_str.assign((const char*)microscope64_png, sizeof(microscope64_png));
        std::stringstream ss(microscope64_png_str);
        osgDB::ReaderWriter::ReadResult result;
        if (rw)
            result = rw->readImage(ss);
        return result.takeImage();
    }


    inline osg::Geometry * createImageBoxGeometryTexEnv(float w, float h, float d, osg::Image* image, bool includeMaterial = false)
    {
        osg::Geometry * ret = sgi::osg_helpers::createBoxGeometry(w, h, d);
        // set up the texture.

        osg::Texture2D* texture = new osg::Texture2D;
        texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
        texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
        texture->setResizeNonPowerOfTwoHint(false);
        float texcoord_x = 1.0f;

        texture->setImage(image);

        osg::StateSet* stateSet = new osg::StateSet;
        stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
        //stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        stateSet->setTextureAttributeAndModes(1, texture, osg::StateAttribute::ON);
        stateSet->setTextureAttributeAndModes(1, new osg::TexEnv(), osg::StateAttribute::ON);
        stateSet->setTextureMode(1, GL_TEXTURE_2D, osg::StateAttribute::ON);
        if (image->isImageTranslucent())
        {
            stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
            stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        }
        if (includeMaterial)
        {
            osg::Material * mat = new osg::Material;
            mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1, 1, 1, 1));
            mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
            mat->setDiffuse(osg::Material::BACK, osg::Vec4(1, 1, 1, 1));
            stateSet->setAttribute(mat, osg::StateAttribute::ON);
        }
        ret->setStateSet(stateSet);
        return ret;
    }
}

class SGIInstallNode : public osg::Group
{
private:
    static osg::Node * buildNode(const std::string & filename)
    {
        if(filename.empty())
            return nullptr;
        osg::Node * ret = nullptr;
        std::string name = osgDB::getStrippedName(filename);
        if(name.compare("box") == 0 || name.compare("cube") == 0)
        {
            ret = sgi::osg_helpers::createBoxGeometry(10.0f, 10.0f, 10.0f);
        }
        else if(name.compare("quad") == 0 || name.compare("rect") == 0)
        {
            ret = sgi::osg_helpers::createQuadGeometry(10.0f, 10.0f);
        }
        else if(name.compare("tri") == 0 || name.compare("triangle") == 0)
        {
            ret = sgi::osg_helpers::createTriangleGeometry(10.0f);
        }
        else if (name.compare("logo") == 0)
        {
            osg::ref_ptr<osg::Image> img = getSGILogoImage();
            ret = sgi::osg_helpers::createImageBoxGeometry(10.0f, 10.0f, 10.0f, img.get());
        }
        else if (name.compare("logo_mat") == 0)
        {
            osg::ref_ptr<osg::Image> img = getSGILogoImage();
            ret = createImageBoxGeometryTexEnv(10.0f, 10.0f, 10.0f, img.get(), true);
        }
        return ret;
    }
public:
    SGIInstallNode(const std::string & filename=std::string(), const osgDB::Options * options=nullptr)
        : osg::Group()
        , _options(filename, options)
        , _installed(false)
    {
        setDataVariance(osg::Object::STATIC);
        // disable culling initially so this node would not be culled on first render traversal.
        // after the first pass we can re-enable culling for this node.
        setCullingActive(false);

        osg::Node * child = buildNode(filename);
        if(child)
            addChild(child);
		++numInstances;
    }
    SGIInstallNode(const SGIInstallNode & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Group(rhs, copyop)
        , _options(rhs._options)
        , _installed(false)
    {
        setDataVariance(osg::Object::STATIC);
		++numInstances;
    }
	~SGIInstallNode() override
	{
		--numInstances;
        if (_installed)
        {
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
            if (_installed)
            {
                if (_proxy.valid())
                {
                    _proxy->release();
                    _proxy = nullptr;
                }
            }
        }
		if(numInstances == 0)
			sgi::shutdown<sgi::autoload::Osg>();
	}

    osg::Object* cloneType() const override { return new SGIInstallNode (); }
    osg::Object* clone(const osg::CopyOp& copyop) const override { return new SGIInstallNode (*this,copyop); }
    bool isSameKindAs(const osg::Object* obj) const override { return dynamic_cast<const SGIInstallNode *>(obj)!=nullptr; }
    const char* className() const override { return "SGIInstallNode"; }
    const char* libraryName() const override { return "osgdb_sgi_loader"; }
    void accept(osg::NodeVisitor& nv) override
    {
        if (nv.validNodeMask(*this))
        {
            nv.pushOntoNodePath(this);
            nv.apply(*this);
            if(nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
            {
                if(!_installed)
                {
                    bool justInstalled = false;
                    {
                        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
                        if(!_installed)
                        {
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
                            osgUtil::CullVisitor * cv = nv.asCullVisitor();
#else
                            osgUtil::CullVisitor * cv = dynamic_cast<osgUtil::CullVisitor *>(&nv);
#endif
                            osg::View * currentView = cv->getCurrentCamera()->getView();
                            auto itInst = _instances.find(currentView);
                            if(itInst != _instances.end())
                            {
                                _proxy = itInst->second;
                                _installed = true;
                            }
                            else
                            {
                                OSG_NOTICE << LC << "install this=" << (void*)this << " filename=" << _options.filename << std::endl;
                                _proxy = new sgi::DefaultSGIProxy(cv->getCurrentCamera(), _options);
                                _installed = true;
                                _instances[currentView] = _proxy.get();
                                justInstalled = true;
                            }
                            setCullingActive(true);
                        }
                    }
                    if(justInstalled)
                        _proxy->runInstallOperations();
                    else
                    {
                        OSG_NOTICE << LC << "post-install this=" << (void*)this << " filename=" << _options.filename << std::endl;
                        _proxy->runStartup(_options);
                    }
                }
            }
            nv.popFromNodePath();
        }
    }

private:
    typedef osg::ref_ptr<sgi::DefaultSGIProxy> DefaultSGIProxyPtr;
    typedef osg::observer_ptr<osg::View> ViewPtr;
    OpenThreads::Mutex _mutex;
    sgi::SGIOptions _options;
    DefaultSGIProxyPtr _proxy;
    bool _installed;

    static std::atomic_int numInstances;
    static std::map<ViewPtr, DefaultSGIProxyPtr> _instances;
};



std::atomic_int SGIInstallNode::numInstances(0);
std::map<SGIInstallNode::ViewPtr, SGIInstallNode::DefaultSGIProxyPtr> SGIInstallNode::_instances;

class ReaderWriteSGI : public osgDB::ReaderWriter
{
public:
    ReaderWriteSGI()
    {
        supportsExtension( "sgi_loader", "SGI loader" );
    }
    ~ReaderWriteSGI() override
	{
	}

    const char* className() const override
    {
        return "SGI ReaderWriter";
    }

    bool acceptsExtension(const std::string& extension) const override
    {
        return osgDB::equalCaseInsensitive( extension, "sgi_loader" );
    }

    ReadResult readObject(const std::string& fileName, const osgDB::Options* options) const override
    {
        return readNode( fileName, options );
    }
    ReadResult readNode(const std::string& fileName, const osgDB::Options* options) const override
    {
        std::string ext = osgDB::getFileExtension( fileName );
        if ( !acceptsExtension( ext ) )
            return ReadResult::FILE_NOT_HANDLED;

        OSG_NOTICE << LC << "readNode " << fileName << std::endl;

        return ReadResult(new SGIInstallNode(osgDB::getNameLessExtension(fileName), options));
    }
};

REGISTER_OSGPLUGIN(earth, ReaderWriteSGI)

