/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2014 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgUtil/CullVisitor>
#include <osgViewer/View>
#include <osg/ValueObject>

#include <QApplication>

#include <osgQt/GraphicsWindowQt>
#if defined(_WIN32)
#include <osgViewer/api/win32/GraphicsWindowWin32>
#elif defined(__APPLE__)
#if !__LP64__
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
#endif // !__LP64__
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
#elif  defined(__linux__)
#include <osgViewer/api/X11/GraphicsWindowX11>
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
#include <sgi/ReferencedPicker>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>

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

#if QT_VERSION < 0x050000
class NativeWidget : public QWidget
{
public:
#ifdef _WIN32
    typedef HWND WinHandle;
#elif defined(__APPLE__)
    typedef Window WinHandle;
#else
    typedef Window WinHandle;
#endif
    NativeWidget(WinHandle handle)
        : QWidget()
    {
        QWidget::create(handle, false, false);
    }
};
#endif // QT_VERSION < 0x050000

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
        const int application_timer_interval_in_ms = 50;
        QCoreApplication * app = QApplication::instance();
        if(!app)
        {
#if defined(_WIN32)
            if (osgViewer::GraphicsWindowWin32 * gwwin = dynamic_cast<osgViewer::GraphicsWindowWin32*>(ctx))
            {
                SetTimer(gwwin->getHWND(), 100232u, application_timer_interval_in_ms, win32_app_timer);
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
            err = sigaction(SIGRTMIN, &sa, NULL);

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
            static char * argv[] = { (char*)"/dev/null", NULL };
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
    static T * getObjectOption(const osgDB::Options * options, const std::string & key, T * defaultValue=NULL, bool * gotOption=NULL)
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
    static T getOption(const osgDB::Options * options, const std::string & key, const T & defaultValue=T(), bool * gotOption = NULL)
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
    SGIOptions(const std::string & filename_=std::string(), const osgDB::Options * options=NULL);
    SGIHostItemBase * getHostItem() const;

    void clear()
    {
        hostCallback = NULL;
        showSceneGraphDialog = false;
        showImagePreviewDialog = false;
        qtObject = NULL;
        parentWidget = NULL;
        osgReferenced = NULL;
        filename.clear();
        usePicketNodeMask = false;
        pickerNodeMask = 0;
        pickerRoot = NULL;
    }

    osg::ref_ptr<sgi::IHostCallback> hostCallback;
    bool showSceneGraphDialog;
    bool showImagePreviewDialog;
	QObject * qtObject;
    QWidget * parentWidget;
	osg::ref_ptr<osg::Referenced> osgReferenced;
    std::string filename;
    bool usePicketNodeMask;
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
        : qtObject(NULL), filename(filename_), usePicketNodeMask(false), pickerNodeMask(~0u)
{
    hostCallback = getObjectOption<sgi::IHostCallback>(options, "sgi_host_callback");
    showSceneGraphDialog = getOption<bool>(options, "showSceneGraphDialog");
    showImagePreviewDialog = getOption<bool>(options, "showImagePreviewDialog");
    osgReferenced = getObjectOption<osg::Referenced>(options, "sgi_osg_referenced");
    qtObject = getObjectOption<QObject>(options, "sgi_qt_object");
    parentWidget = getObjectOption<QWidget>(options, "parentWidget");
    pickerNodeMask = getOption<unsigned>(options, "pickerNodeMask", ~0u, &usePicketNodeMask);
    pickerRoot = getObjectOption<osg::Node>(options, "pickerRoot");
}

SGIHostItemBase * SGIOptions::getHostItem() const
{
    SGIHostItemBase * ret = NULL;
    if(!filename.empty())
    {
        if(filename == "qapp")
            ret = new SGIHostItemQt(qApp);
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


class SceneGraphInspectorHandler : public osgGA::GUIEventHandler
{
public:
	SceneGraphInspectorHandler(IHostCallback * callback, const SGIOptions & options);
	virtual ~SceneGraphInspectorHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	IHostCallback * getHostCallback() { return _hostCallback.get(); }
    void shutdown();

protected:
	bool                showSceneGraphDialog(const SGIHostItemBase * item);
	bool                showObjectLoggerDialog(const SGIHostItemBase * item);
	bool                contextMenu(const SGIHostItemBase * item, float x, float y);

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
    _hostCallback = NULL;
    _picker = NULL;
}

bool SceneGraphInspectorHandler::showSceneGraphDialog(const SGIHostItemBase * item)
{
    ISceneGraphDialogPtr dialog = _hostCallback->showSceneGraphDialog(NULL, item);
    if(dialog.valid())
        dialog->show();
    return dialog.valid();
}

bool SceneGraphInspectorHandler::showObjectLoggerDialog(const SGIHostItemBase * item)
{
	return _hostCallback->showObjectLoggerDialog(NULL, item) != NULL;
}

bool SceneGraphInspectorHandler::contextMenu(const SGIHostItemBase * item, float x, float y)
{
	IContextMenuPtr contextMenu = _hostCallback->contextMenu(NULL, item);
	if (contextMenu.valid())
		contextMenu->popup(_hostCallback->getFallbackParentWidget(), x, y);
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
                    ret = showSceneGraphDialog(NULL);
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
                osg::Camera * camera = view ? view->getCamera() : NULL;
                OSG_FATAL << "RELEASE " << x << ',' << y << " view=" << view << " cam=" << camera << std::endl;
                if (camera)
                {
                    osg::Viewport * viewport = camera->getViewport();
                    if (viewport)
                        y = viewport->height() - y;
                }

                SGIHostItemBasePtr hostItem = _options.getHostItem();
                if (!hostItem.valid())
                {
                    if (aa.asView())
                        hostItem = new SGIHostItemOsg(aa.asView());
                    else
                        hostItem = new SGIHostItemOsg(&ea);
                }
                ret = contextMenu(hostItem, x, y);
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


class FindTreeItemNodeVisitor : public osg::NodeVisitor
{
public:
    FindTreeItemNodeVisitor(TraversalMode tm=TRAVERSE_ALL_CHILDREN)
        : osg::NodeVisitor(tm) {}
    struct NodeItem
    {
        osg::ref_ptr<osg::Node> node;
        std::string name;
        bool imageGeode;
    };
    typedef std::vector<NodeItem> NodeList;
    const NodeList &   results() const
        { return _nodes; }

    virtual void apply(osg::Node& node)
    {
        bool sgi_tree_item = false;
        if(node.getUserValue<bool>("sgi_tree_item", sgi_tree_item))
        {
            if (sgi_tree_item)
            {
                NodeItem item;
                item.node = &node;
                item.imageGeode = false;
                node.getUserValue<std::string>("sgi_tree_itemname", item.name);
                if(!node.getUserValue<bool>("sgi_tree_imagegeode", item.imageGeode))
                    item.imageGeode = false;
                _nodes.push_back(item);
            }
        }
        traverse(node);
    }
protected:
    NodeList _nodes;
};

class DefaultSGIProxy : public osg::Referenced
{
public:
    DefaultSGIProxy(osg::Camera * camera, const SGIOptions & options)
        : _parent(NULL)
        , _view(NULL)
        , _hostCallback(new HostCallbackImpl(this, options.hostCallback.valid()?options.hostCallback.get():sgi::defaultHostCallback<sgi::autoload::Osg>()))
        , _options(options)
        , _inspectorHandler()
    {
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if (_view)
        {
            _inspectorHandler = new sgi::SceneGraphInspectorHandler(_hostCallback, _options);
            _view->addEventHandler(_inspectorHandler.get());
        }

        osg::GraphicsContext * ctx = camera->getGraphicsContext();
        if(ctx)
        {
            ensure_QApplication(ctx);

            if(options.parentWidget)
                _parent = options.parentWidget;
            else
            {
                if(osgQt::GraphicsWindowQt * gwqt = dynamic_cast<osgQt::GraphicsWindowQt*>(ctx))
                    _parent = gwqt->getGLWidget();
#if defined(_WIN32)
                else if(osgViewer::GraphicsWindowWin32 * gwwin = dynamic_cast<osgViewer::GraphicsWindowWin32*>(ctx))
                {
#if QT_VERSION < 0x050000
                    _parent = QWidget::find(gwwin->getHWND());
#endif // QT_VERSION < 0x050000
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
                    //_parent = QWidget::find(gwx11->getWindow());
                }
#endif
            }
            OSG_NOTICE << LC << "DefaultSGIProxy parent " << _parent << std::endl;

            if(_parent)
            {
                bool gotImage = false;
                if(_options.showImagePreviewDialog)
                {
                    IImagePreviewDialogPtr dialog;
                    FindTreeItemNodeVisitor ftinv;
                    camera->accept(ftinv);
                    for(FindTreeItemNodeVisitor::NodeList::const_iterator it = ftinv.results().begin(); !gotImage && it != ftinv.results().end(); ++it)
                    {
                        const FindTreeItemNodeVisitor::NodeItem & item = *it;
                        if(item.imageGeode)
                        {
                            // ... and if it is a image geode try to add the image to the tree as well
                            osg::StateSet* stateSet = item.node->getStateSet();
                            if(stateSet)
                            {
                                osg::StateAttribute * sa = stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
                                osg::Texture * texture = sa ? sa->asTexture() : NULL;
                                if(texture)
                                {
                                    SGIHostItemOsg image(texture->getImage(0));
                                    if(image.hasObject())
                                    {
                                        dialog = _hostCallback->showImagePreviewDialog(_parent, &image);
                                        gotImage = true;
                                    }
                                }
                            }
                        }
                    }
                    if(dialog.valid())
                        dialog->show();
                }
                if(_options.showSceneGraphDialog && !gotImage)
                {
                    ISceneGraphDialogPtr dialog;
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
                    if(dialog.valid())
                        dialog->show();
                }
            }
        }
    }
    virtual ~DefaultSGIProxy() override
    {
        _inspectorHandler = NULL;
        _parent = NULL;
        _view = NULL;
        _options.clear();
        _viewPtr = NULL;
        _hostCallback = NULL;
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
        _inspectorHandler = NULL;
        _parent = NULL;
        _view = NULL;
        _options.clear();
        _viewPtr = NULL;
        _hostCallback = NULL;
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
        virtual ReferencedPickerBase * createPicker(PickerType type, float x, float y)
        {
            if(!_parent->_view)
                return NULL;

            ReferencedPickerBase * ret = NULL;
            osg::Node * root = _parent->_options.pickerRoot.get();
            float buffer = 1.0f;
            unsigned traversalMask = _parent->_options.usePicketNodeMask ? _parent->_options.pickerNodeMask : (unsigned)_parent->_view->getCamera()->getCullMask();

            switch(type)
            {
            case PickerTypeLine:
                ret = new ReferencedLinePicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask);
                break;
            case PickerTypeDefault:
                ret = new ReferencedPicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask, buffer);
                break;
            }
            return ret;
        }
        virtual SGIItemBase * getView() override
        {
            return _parent->getView();
        }
        virtual void triggerRepaint() override
        {
			if(_parent->_view)
				_parent->_view->requestRedraw();
        }
        virtual QWidget * getFallbackParentWidget() override
        {
            return _parent->_parent;
        }
        virtual void shutdown() override
        {
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
                    _previousInstalled = NULL;
                }
            }
        }
    private:
        DefaultSGIProxy * _parent;
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

class SGIInstallNode : public osg::Node
{
public:
	static unsigned numInstances;
    SGIInstallNode(const std::string & filename=std::string(), const osgDB::Options * options=NULL)
        : osg::Node()
        , _options(filename, options)
        , _installed(false)
    {
        setDataVariance(osg::Object::STATIC);
        // disable culling initially so this node would not be culled on first render traversal.
        // after the first pass we can re-enable culling for this node.
        setCullingActive(false);
		++numInstances;
    }
    SGIInstallNode(const SGIInstallNode & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Node(rhs, copyop)
        , _options(rhs._options)
        , _installed(false)
    {
        setDataVariance(osg::Object::STATIC);
		++numInstances;
    }
	~SGIInstallNode()
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
                    _proxy = NULL;
                }
            }
        }
		if(numInstances == 0)
			sgi::shutdown<sgi::autoload::Osg>();
	}

    virtual osg::Object* cloneType() const { return new SGIInstallNode (); }
    virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new SGIInstallNode (*this,copyop); }
    virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const SGIInstallNode *>(obj)!=NULL; }
    virtual const char* className() const { return "SGIInstallNode"; }
    virtual const char* libraryName() const { return "osgdb_sgi"; }
    virtual void accept(osg::NodeVisitor& nv)
    {
        if (nv.validNodeMask(*this))
        {
            nv.pushOntoNodePath(this);
            nv.apply(*this);
            if(nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
            {
                if(!_installed)
                {
                    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
                    if(!_installed)
                    {
                        setCullingActive(true);
                        OSG_NOTICE << LC << "install." << std::endl;

                        osgUtil::CullVisitor * cv = dynamic_cast<osgUtil::CullVisitor *>(&nv);
                        if(cv)
                        {
                            _proxy = new sgi::DefaultSGIProxy(cv->getCurrentCamera(), _options);
                            _installed = true;
                        }
                    }
                }
            }
            nv.popFromNodePath();
        }
    }

private:
    OpenThreads::Mutex _mutex;
    sgi::SGIOptions _options;
    osg::ref_ptr<sgi::DefaultSGIProxy> _proxy;
    bool _installed;
};

unsigned SGIInstallNode::numInstances = 0;

class ReaderWriteSGI : public osgDB::ReaderWriter
{
public:
    ReaderWriteSGI()
    {
        supportsExtension( "sgi_loader", "SGI loader" );
    }
	~ReaderWriteSGI()
	{
	}

    virtual const char* className()
    {
        return "SGI ReaderWriter";
    }

    virtual bool acceptsExtension(const std::string& extension) const
    {
        return osgDB::equalCaseInsensitive( extension, "sgi_loader" );
    }

    virtual ReadResult readObject(const std::string& fileName, const osgDB::Options* options) const
    {
        return readNode( fileName, options );
    }
    virtual ReadResult readNode(const std::string& fileName, const osgDB::Options* options) const
    {
        std::string ext = osgDB::getFileExtension( fileName );
        if ( !acceptsExtension( ext ) )
            return ReadResult::FILE_NOT_HANDLED;

        OSG_NOTICE << LC << "readNode " << fileName << std::endl;

        return ReadResult(new SGIInstallNode(osgDB::getNameLessExtension(fileName), options));
    }
};

REGISTER_OSGPLUGIN(earth, ReaderWriteSGI)

