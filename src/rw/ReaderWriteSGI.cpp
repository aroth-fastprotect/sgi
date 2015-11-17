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

#include <QApplication>

#include <osgQt/GraphicsWindowQt>
#if defined(_WIN32)
#include <osgViewer/api/win32/GraphicsWindowWin32>
#elif defined(__APPLE__)
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
#else
#include <osgViewer/api/X11/GraphicsWindowX11>
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
    static void ensure_QApplication()
    {
        QCoreApplication * app = QApplication::instance();
        if(!app)
        {
            int argc = 0;
            char ** argv = NULL;
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
    static T * getObjectOption(const osgDB::Options * options, const std::string & key, T * defaultValue=NULL)
    {
        if(!options)
            return defaultValue;
        const void * data = options->getPluginData(key);
        if(!data)
            return defaultValue;
        return static_cast<T*>(const_cast<void*>(data));
    }
    template<typename T>
    static T getOption(const osgDB::Options * options, const std::string & key, const T & defaultValue=T())
    {
        if(!options)
            return defaultValue;
        const void * data = options->getPluginData(key);
        if(!data)
            return defaultValue;
        return *static_cast<const T*>(data);
    }
    SGIOptions(const std::string & filename_=std::string(), const osgDB::Options * options=NULL);
    SGIHostItemBase * getHostItem() const;

    osg::ref_ptr<sgi::IHostCallback> hostCallback;
    bool showSceneGraphDialog;
	QObject * qtObject;
    QWidget * parentWidget;
	osg::ref_ptr<osg::Referenced> osgReferenced;
    std::string filename;
    unsigned pickerNodeMask;
    osg::ref_ptr<osg::Node> pickerRoot;
};

template<>
bool SGIOptions::getOption<bool>(const osgDB::Options * options, const std::string & key, const bool & defaultValue)
{
    if(!options)
        return defaultValue;
    std::string val = options->getPluginStringData(key);
    if(val.empty())
        return defaultValue;
    return string_to_bool(val, defaultValue);
}

SGIOptions::SGIOptions(const std::string & filename_, const osgDB::Options * options)
        : qtObject(NULL), filename(filename_), pickerNodeMask(~0u)
{
    hostCallback = getObjectOption<sgi::IHostCallback>(options, "sgi_host_callback");
    showSceneGraphDialog = getOption<bool>(options, "showSceneGraphDialog");
    osgReferenced = getObjectOption<osg::Referenced>(options, "sgi_osg_referenced");
    qtObject = getObjectOption<QObject>(options, "sgi_qt_object");
    parentWidget = getObjectOption<QWidget>(options, "parentWidget");
    pickerNodeMask = getOption<unsigned>(options, "pickerNodeMask", ~0u);
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
                osg::Camera * camera = aa.asView()->getCamera();
                if (camera)
                {
                    osg::Viewport * viewport = camera->getViewport();
                    if (viewport)
                        y = viewport->height() - y;
                }

                SGIHostItemBasePtr hostItem = _options.getHostItem();
                if(!hostItem.valid())
                    hostItem = new SGIHostItemOsg(&ea);
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


class DefaultSGIProxy : public osg::Referenced
{
public:
    DefaultSGIProxy(osg::Camera * camera, const SGIOptions & options)
        : _parent(NULL)
        , _view(NULL)
        , _hostCallback(new HostCallbackImpl(this, options.hostCallback.valid()?options.hostCallback.get():sgi::defaultHostCallback<sgi::autoload::Osg>()))
        , _options(options)
    {
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if(_view)
            _view->addEventHandler(new sgi::SceneGraphInspectorHandler(_hostCallback, _options));

        osg::GraphicsContext * ctx = camera->getGraphicsContext();
        if(ctx)
        {
            ensure_QApplication();

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
                else if(osgViewer::GraphicsWindowCarbon * gwcarbon = dynamic_cast<osgViewer::GraphicsWindowCarbon*>(ctx))
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
                if(_options.showSceneGraphDialog)
                {
                    ISceneGraphDialogPtr dialog;
                    if(_view)
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
    virtual ~DefaultSGIProxy()
    {
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

    class HostCallbackImpl : public HostCallbackBase
    {
    public:
        HostCallbackImpl(DefaultSGIProxy * parent, IHostCallback * callback)
            : HostCallbackBase(callback), _parent(parent) 
		{
			sgi::setHostCallback<autoload::Osg>(this);
		}
        virtual ReferencedPickerBase * createPicker(PickerType type, float x, float y)
        {
            if(!_parent->_view)
                return NULL;

            ReferencedPickerBase * ret = NULL;
            osg::Node * root = _parent->_options.pickerRoot.get();
            float buffer = 1.0f;
            unsigned traversalMask = _parent->_options.pickerNodeMask;

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

    private:
        DefaultSGIProxy * _parent;
    };

private:
    QWidget * _parent;
    osgViewer::View * _view;
    SGIItemBasePtr _viewPtr;
    IHostCallbackPtr _hostCallback;
    sgi::SGIOptions _options;
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
        setDataVariance(DYNAMIC);
		++numInstances;
    }
    SGIInstallNode(const SGIInstallNode & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Node(rhs, copyop)
        , _options(rhs._options)
        , _installed(false)
    {
        setDataVariance(DYNAMIC);
		++numInstances;
    }
	~SGIInstallNode()
	{
		--numInstances;
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

