// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#include <vsg/io/ReaderWriter.h>

#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <QDebug>

#if defined(_WIN32)
//#include <osgViewer/api/win32/GraphicsWindowWin32>
#elif defined(__APPLE__)
#elif  defined(__linux__)
#include <signal.h>
#include <time.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#include <iostream>

#include <vsg/maths/vec3.h>
#include <vsg/nodes/Group.h>
#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/PointerEvent.h>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>
#include <sgi/ObjectLoggerDialog>
#include <sgi/ImagePreviewDialog>
#include <sgi/ContextMenu>
#include <sgi/AutoLoadQt>
#include <sgi/GenerateItem>
#include <sgi/Shutdown>
#include <sgi/LibraryInfo>
//#include <sgi/ReferencedPicker>
#include <sgi/plugins/SGIHostItemVsg.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>
#include <sgi/helpers/vsg>
#include <sgi/helpers/vsg_helper_nodes>
#include <sgi/helpers/string>

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
        Q_UNUSED(sig);
        //std::cout << "x11_app_timer_signal" << std::endl;
        QCoreApplication::instance()->processEvents();
    }
    unsigned getCurrentThreadId()
    {
        return (unsigned)::syscall(SYS_gettid);
    }
#endif
    static void ensure_QApplication(vsg::Window * ctx)
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
    template<typename T>
    static T * getObjectOption(const vsg::Options * options, const std::string & key, T * defaultValue=nullptr, bool * gotOption=nullptr)
    {
        if (!options)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        T * value = defaultValue;
        if(!options->getValue(key, value))
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        if (gotOption)
            *gotOption = true;
        return value;
    }
    template<typename T>
    static T getOption(const vsg::Options * options, const std::string & key, const T & defaultValue=T(), bool * gotOption = nullptr)
    {
        if (!options)
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        T value = defaultValue;
        if(!options->getValue(key, value))
        {
            if (gotOption)
                *gotOption = false;
            return defaultValue;
        }
        if (gotOption)
            *gotOption = true;
        return value;
    }
    SGIOptions(const std::string & filename_=std::string(), const vsg::Options * options=nullptr);
    SGIHostItemBase * getHostItem() const;

    void clear()
    {
        hostCallback = nullptr;
        showSceneGraphDialog = false;
        showImagePreviewDialog = false;
        qtObject = nullptr;
        parentWidget = nullptr;
        vsgObject = nullptr;
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
    vsg::ref_ptr<vsg::Object> vsgObject;
    std::string filename;
    bool usePickerNodeMask;
    unsigned pickerNodeMask;
    vsg::ref_ptr<vsg::Node> pickerRoot;
};

template<>
bool SGIOptions::getOption<bool>(const vsg::Options * options, const std::string & key, const bool & defaultValue, bool * gotOption)
{
    if (!options)
    {
        if (gotOption)
            *gotOption = false;
        return defaultValue;
    }
    std::string val;
    options->getValue(key, val);
    if(val.empty())
    {
        if (gotOption)
            *gotOption = false;
        return defaultValue;
    }
    if (gotOption)
        *gotOption = true;
    return helpers::string_to_bool(val, nullptr, defaultValue);
}

SGIOptions::SGIOptions(const std::string & filename_, const vsg::Options * options)
        : qtObject(nullptr), filename(filename_), usePickerNodeMask(false), pickerNodeMask(~0u)
{
    hostCallback = getObjectOption<sgi::IHostCallback>(options, "sgi_host_callback");
    if(filename == "sgi-info" || filename == "dialog")
    {
        showSceneGraphDialog = true;
        showImagePreviewDialog = false;
    }
    else
    {
        showSceneGraphDialog = getOption<bool>(options, "showSceneGraphDialog");
        showImagePreviewDialog = getOption<bool>(options, "showImagePreviewDialog");
    }
    qtObject = getObjectOption<QObject>(options, "sgi_qt_object");
    parentWidget = getObjectOption<QWidget>(options, "parentWidget");
    pickerNodeMask = getOption<unsigned>(options, "pickerNodeMask", ~0u, &usePickerNodeMask);
    pickerRoot = getObjectOption<vsg::Node>(options, "pickerRoot");
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
            sgi::details::Referenced * libinfo = sgi::libraryInfoObject<sgi::autoload::Qt>();
            ret = new SGIHostItemInternal(libinfo);
        }
    }
    if(!ret)
    {
        if (qtObject)
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

class SceneGraphInspectorHandler : public vsg::Inherit<vsg::Visitor, SceneGraphInspectorHandler>
{
public:
	SceneGraphInspectorHandler(IHostCallback * callback, const SGIOptions & options);
	virtual ~SceneGraphInspectorHandler();

    //virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void apply(vsg::KeyPressEvent& ev) override;
    void apply(vsg::ButtonPressEvent& ev) override;
    void apply(vsg::ButtonReleaseEvent& ev) override;

	IHostCallback * getHostCallback() { return _hostCallback.get(); }
    void shutdown();

protected:
    bool                showSceneGraphDialog(const SGIHostItemBase * item, QWidget * parent=nullptr);
    bool                showObjectLoggerDialog(const SGIHostItemBase * item, QWidget * parent=nullptr);
    bool                contextMenu(const SGIHostItemBase * item, float x, float y, QWidget * parent=nullptr);

protected:
	IHostCallbackPtr    _hostCallback;
    const SGIOptions &	_options;
    vsg::KeySymbol      _inspectorHitTestKey;
    vsg::KeySymbol      _inspectorEventKey;
    vsg::KeySymbol      _inspectorLoggerKey;
    int                 _inspectorLoggerModMask;
    vsg::KeySymbol      _inspectorInfoKey;
    int                 _inspectorInfoKeyModMask;
    vsg::ButtonMask     _inspectorHitTestMouseButton;
    int                 _inspectorHitTestMouseModMask;
    vsg::ButtonMask     _inspectorContextMenuMouseButton;
    int                 _inspectorContextMenuMouseModMask;
};

SceneGraphInspectorHandler::SceneGraphInspectorHandler(IHostCallback * callback, const SGIOptions & options)
	: _hostCallback(callback)
	, _options(options)
    , _inspectorHitTestKey(vsg::KEY_i)
    , _inspectorEventKey(vsg::KEY_e)
    , _inspectorLoggerKey(vsg::KEY_l)
    , _inspectorLoggerModMask(vsg::MODKEY_Shift)
    , _inspectorInfoKey(vsg::KEY_Q)
    , _inspectorInfoKeyModMask(vsg::MODKEY_Shift)
    , _inspectorHitTestMouseButton(vsg::BUTTON_MASK_2)
    , _inspectorHitTestMouseModMask(vsg::MODKEY_Control | vsg::MODKEY_Shift)
    , _inspectorContextMenuMouseButton(vsg::BUTTON_MASK_3)
    , _inspectorContextMenuMouseModMask(vsg::MODKEY_Control | vsg::MODKEY_Shift)
{
}

SceneGraphInspectorHandler::~SceneGraphInspectorHandler()
{
}

void SceneGraphInspectorHandler::shutdown()
{
    _hostCallback = nullptr;
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


void SceneGraphInspectorHandler::apply(vsg::KeyPressEvent& ev)
{
    if (_inspectorHitTestKey != vsg::KEY_Undefined && ev.keyBase == _inspectorHitTestKey)
    {
        IHostCallback::PickerType pickerType;
        if (ev.keyModifier & (vsg::MODKEY_Shift))
            pickerType = IHostCallback::PickerTypeLine;
        else
            pickerType = IHostCallback::PickerTypeDefault;
        /*
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
*/
    }
    else if (_inspectorInfoKey != vsg::KEY_Undefined && ev.keyBase == _inspectorInfoKey &&
        ev.keyModifier & _inspectorInfoKeyModMask)
    {
        ReferencedInternalInfoData * info = new ReferencedInternalInfoData(InternalInfoData::CommandIdAbout);
        SGIHostItemInternal itemAbout(info);
        showSceneGraphDialog(&itemAbout);
    }
    else if (_inspectorEventKey != vsg::KEY_Undefined && ev.keyBase == _inspectorEventKey)
    {
        SGIHostItemVsg itemEa(&ev);
        showSceneGraphDialog(&itemEa);
    }
    else if (_inspectorLoggerKey != vsg::KEY_Undefined && ev.keyBase == _inspectorLoggerKey &&
        ev.keyModifier & (_inspectorLoggerModMask))
    {
        SGIHostItemVsg itemEa(&ev);
        showObjectLoggerDialog(&itemEa);
    }
}

void SceneGraphInspectorHandler::apply(vsg::ButtonPressEvent & ev)
{
}

void SceneGraphInspectorHandler::apply(vsg::ButtonReleaseEvent & ev)
{
    if (_inspectorHitTestMouseButton != 0 &&
        ev.button == _inspectorHitTestMouseButton &&
        (ev.mask & _inspectorHitTestMouseModMask) == _inspectorHitTestMouseModMask)
    {
        SGIHostItemVsg itemEa(&ev);
        showSceneGraphDialog(&itemEa);
    }
    else if (_inspectorContextMenuMouseButton != 0 &&
            ev.button == _inspectorContextMenuMouseButton &&
             (((ev.mask & _inspectorContextMenuMouseModMask) == _inspectorContextMenuMouseModMask))
        )
    {
        int32_t x = ev.x;
        int32_t y = ev.y;
        vsg::ref_ptr<vsg::Window> window = ev.window.ref_ptr();

        QWidget * parent = nullptr;
        SGIHostItemBasePtr hostItem;
        if (window)
        {
            QObject * qobject = nullptr;
            double sgi_qt_object;
            if (window->getValue("sgi_qt_object", sgi_qt_object) && sgi_qt_object)
                qobject = (QObject*)(void*)(qulonglong)sgi_qt_object;
            if(qobject)
                hostItem = new SGIHostItemQt(qobject);
            else
                hostItem = new SGIHostItemVsg(window);
        }
        else
            hostItem = new SGIHostItemVsg(&ev);
        contextMenu(hostItem.get(), x, y, parent);
    }
}


class DefaultSGIProxy : public vsg::Object
{
public:
    DefaultSGIProxy(vsg::Camera * camera, const SGIOptions & options)
        : _parent(nullptr)
        , _hostCallback(new HostCallbackImpl(this, options.hostCallback.valid()?options.hostCallback.get():sgi::defaultHostCallback<sgi::autoload::Qt>()))
        , _options(options)
        , _inspectorHandler()
    {
        /*
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if (_view)
        {
            _inspectorHandler = new sgi::SceneGraphInspectorHandler(_hostCallback.get(), _options);
            _view->addEventHandler(_inspectorHandler.get());
        }
*/
    }
    ~DefaultSGIProxy() override
    {
        _inspectorHandler.reset();
        _parent = nullptr;
        _options.clear();
        _viewPtr = nullptr;
        _hostCallback = nullptr;
    }
    void runInstallOperations()
    {
#if 0
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
#ifdef OSGQT_ENABLE_QGLWIDGET
				if (osgQt::GraphicsWindowQt* gwqt = dynamic_cast<osgQt::GraphicsWindowQt*>(ctx))
					_parent = gwqt->getGLWidget();
				else
#endif
				if (osgQt::GraphicsWindowQt5* gwqt5 = dynamic_cast<osgQt::GraphicsWindowQt5*>(ctx))
				{
					_parent = gwqt5->getOrCreateGLWidget();
				}
#else // SGI_USE_OSGQT
                if(0)
                {
                    void(0);
                }
#endif // SGI_USE_OSGQT
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
#endif
        runStartup(_options);
    }

    void runStartup(const SGIOptions & options)
    {
        qWarning() << "DefaultSGIProxy runStartup " << _parent << options;
        if(_parent)
        {
#if 0
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
#endif
        }
    }
public:
    SGIItemBase * getView()
    {
#if 0
        if(!_viewPtr.valid() && _view)
        {
            SGIHostItemOsg viewHostItem(_view);
            sgi::generateItem<autoload::Osg>(_viewPtr, &viewHostItem);
        }
#endif
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
        vsg::ref_ptr<SceneGraphInspectorHandler> handler = _inspectorHandler.ref_ptr();
        if (handler)
            handler->shutdown();
        _inspectorHandler.reset();
        _parent = nullptr;
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
            sgi::getHostCallback<autoload::Qt>(_previousInstalled);
            // ... now install this new host callback
            sgi::setHostCallback<autoload::Qt>(this);
		}
        ~HostCallbackImpl() override
        {
            //OSG_WARN << "~HostCallbackImpl: " << this << " org:" << _original.get() << std::endl;
        }
        ReferencedPickerBase * createPicker(PickerType type, float x, float y) override
        {
            if(!_parent)
                return nullptr;

            ReferencedPickerBase * ret = nullptr;
#if 0
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
#endif
            return ret;
        }
        SGIItemBase * getView() override
        {
            if (_parent)
                return _parent.ref_ptr()->getView();
            else
                return nullptr;
        }
        void triggerRepaint() override
        {
            if (_parent) {
                //_parent->_view->requestRedraw();
            }
        }
        QWidget * getFallbackParentWidget() override
        {
            if (_parent)
                return _parent.ref_ptr()->_parent;
            else
                return nullptr;
        }
        void shutdown() override
        {
            if(_parent)
                _parent.ref_ptr()->shutdown();
            HostCallbackBase::shutdown();
        }
        void release()
        {
            IHostCallbackPtr currentHostCallback;
            if (sgi::getHostCallback<autoload::Qt>(currentHostCallback))
            {
                //OSG_WARN << "~HostCallbackImpl::release: " << this << " current:" << currentHostCallback.get() << std::endl;
                if (currentHostCallback.get() == this)
                {
                    //OSG_WARN << "~HostCallbackImpl::release: " << this << " restore:" << _previousInstalled.get() << std::endl;
                    // restore the previously used callback
                    sgi::setHostCallback<autoload::Qt>(_previousInstalled.get());
                    _previousInstalled = nullptr;
                }
            }
        }
    private:
        vsg::observer_ptr<DefaultSGIProxy> _parent;
        IHostCallbackPtr _previousInstalled;
    };

private:
    QWidget * _parent;
    SGIItemBasePtr _viewPtr;
    IHostCallbackPtr _hostCallback;
    sgi::SGIOptions _options;
    vsg::observer_ptr<sgi::SceneGraphInspectorHandler> _inspectorHandler;
};
} // namespace sgi

namespace {
    typedef std::map<std::string, std::string> stringmap;
    typedef std::pair<std::string, std::string> stringpair;
    typedef std::map<std::string, stringpair> stringpairmap;

    using namespace sgi::vsg_helpers;

    static stringpairmap propertiesAliases = {
        std::make_pair("red", std::make_pair("color", "1 0 0 1")),
        std::make_pair("green", std::make_pair("color", "0 1 0 1")),
        std::make_pair("blue", std::make_pair("color", "0 0 1 1")),
        std::make_pair("black", std::make_pair("color", "0 0 0 1")),
        std::make_pair("white", std::make_pair("color", "1 1 1 1")),
        std::make_pair("gray", std::make_pair("color", "0.75 0.75 0.75 1")),
        std::make_pair("mat", std::make_pair("material", "1")),
        std::make_pair("logo", std::make_pair("logo", "1")),
    };
    template<typename T>
    static bool getPropertyAs(const stringmap & props, const std::string & key, T & v)
    {
        auto it = props.find(key);
        bool ret = (it != props.end());
        if (ret)
        {
            std::stringstream ss(it->second);
            ss >> v;
            ret = ss.good() || ss.eof();
        }
        return ret;
    }
    template<>
    bool getPropertyAs<bool>(const stringmap & props, const std::string & key, bool & v)
    {
        auto it = props.find(key);
        bool ret = (it != props.end());
        if (ret)
            v = sgi::helpers::string_to_bool(it->second, &ret, false);
        return ret;
    }
}

class SGIInstallNode : public vsg::Group
{
private:
    static bool parseProperty(const std::string & s, stringpair & out)
    {
        bool ret = false;
        auto it = propertiesAliases.find(s);
        if (it != propertiesAliases.end())
        {
            out.first = it->second.first;
            out.second = it->second.second;
            ret = true;
        }
        return ret;
    }
    static vsg::Node * buildNodeImpl(const std::string & filename, stringmap & props)
    {
        vsg::Node * ret = nullptr;
        std::string ext = vsg::lowerCaseFileExtension(filename);
        if (ext.empty())
        {
            if(!filename.empty())
            {
                std::cout << "buildNodeImpl " << filename << std::endl;

                vsg::vec3 size;
                vsg::vec4 color;
                sgi::vsg_helpers::GeometryParams params;
                if (getPropertyAs(props, "color", color))
                    params.setCustomColor(color);
                if (!getPropertyAs(props, "size", size))
                    size.set(10.0f, 10.0f, 10.0f);
                if (!getPropertyAs(props, "material", params.useMaterial))
                    params.useMaterial = false;
                if (!getPropertyAs(props, "logo", params.useLogo))
                    params.useLogo = false;

                if (filename.compare("box") == 0 || filename.compare("cube") == 0)
                {
                    ret = sgi::vsg_helpers::createBoxGeometry(size, params);
                }
                else if (filename.compare("quad") == 0 || filename.compare("rect") == 0)
                {
                    ret = sgi::vsg_helpers::createQuadGeometry(10.0f, 10.0f, params);
                }
                else if (filename.compare("tri") == 0 || filename.compare("triangle") == 0)
                {
                    ret = sgi::vsg_helpers::createTriangleGeometry(10.0f, params);
                }
                else if (filename.compare("logo") == 0)
                {
                    params.useLogo = true;
                    ret = sgi::vsg_helpers::createBoxGeometry(size, params);
                }
            }
        }
        else
        {
            std::string::size_type i = ext.find('=');
            stringpair v;
            if (i != std::string::npos)
            {
                v.first = ext.substr(0, i);
                v.second = ext.substr(i + 1);
            }
            else
            {
                if (!parseProperty(ext, v))
                {
                    v.first = ext;
                }
            }
            std::cout << "buildNodeImpl ext=" << ext << " " << v.first << "->" << v.second << std::endl;
            props[v.first] = v.second;
            std::string name = vsg::filePath(filename);
            ret = buildNodeImpl(name, props);
        }
        return ret;

    }

    static vsg::Node * buildNode(const std::string & filename)
    {
        if(filename.empty())
            return nullptr;
        stringmap props;
        return buildNodeImpl(filename, props);
    }
public:
    SGIInstallNode(const std::string & filename=std::string(), const vsg::Options * options=nullptr)
        : vsg::Group()
        , _options(filename, options)
        , _installed(false)
    {
        // disable culling initially so this node would not be culled on first render traversal.
        // after the first pass we can re-enable culling for this node.
        //setCullingActive(false);

        vsg::ref_ptr<vsg::Node> child(buildNode(filename));
        if(child)
            addChild(child);
		++numInstances;
    }
	~SGIInstallNode() override
	{
		--numInstances;
        if (_installed)
        {
            std::unique_lock lock(_mutex);
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
            sgi::shutdown<sgi::autoload::Qt>();
	}

    void accept(vsg::Visitor& visitor) override
    {
        //if (nv.validNodeMask(*this))
        {
            //nv.pushOntoNodePath(this);
            //nv.apply(*this);
            //if(nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
            {
                if(!_installed)
                {
                    bool justInstalled = false;
                    {
                        std::unique_lock lock(_mutex);
                        if(!_installed)
                        {
                            uint32_t currentViewID = 0;
                            //vsg::View * currentView = cv->getCurrentCamera()->getView();
                            vsg::View * currentView = nullptr;
                            auto itInst = _instances.find(currentViewID);
                            if(itInst != _instances.end())
                            {
                                _proxy = itInst->second;
                                _installed = true;
                            }
                            else
                            {
                                vsg::Camera * currentCamera = nullptr;
                                std::cout << LC << "install this=" << (void*)this << " filename=" << _options.filename << std::endl;
                                _proxy = new sgi::DefaultSGIProxy(currentCamera, _options);
                                _installed = true;
                                _instances[currentViewID] = _proxy.get();
                                justInstalled = true;
                            }
                            //setCullingActive(true);
                        }
                    }
                    if(justInstalled)
                        _proxy->runInstallOperations();
                    else
                    {
                        std::cout << LC << "post-install this=" << (void*)this << " filename=" << _options.filename << std::endl;
                        _proxy->runStartup(_options);
                    }
                }
            }
        }
    }

private:
    typedef vsg::ref_ptr<sgi::DefaultSGIProxy> DefaultSGIProxyPtr;
    typedef vsg::observer_ptr<vsg::View> ViewPtr;
    std::mutex _mutex;
    sgi::SGIOptions _options;
    DefaultSGIProxyPtr _proxy;
    bool _installed;

    static std::atomic_int numInstances;
    static std::map<uint32_t, DefaultSGIProxyPtr> _instances;
};



std::atomic_int SGIInstallNode::numInstances(0);
// viewId to DefaultSGIProxyPtr map
std::map<uint32_t, SGIInstallNode::DefaultSGIProxyPtr> SGIInstallNode::_instances;

class rw_sgi : public vsg::Inherit<vsg::ReaderWriter, rw_sgi>
{
public:
    rw_sgi()
    {
        //supportsExtension( "sgi_loader", "SGI loader" );
    }
    ~rw_sgi() override
	{
	}

#if 0
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
#endif
};

//REGISTER_OSGPLUGIN(earth, ReaderWriteSGI)

