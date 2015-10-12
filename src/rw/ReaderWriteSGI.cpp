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
#include <sgi/InspectorHandler>
#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>
#include <sgi/ObjectLoggerDialog>
#include <sgi/ImagePreviewDialog>
#include <sgi/ContextMenu>
#include <sgi/AutoLoadOsg>
#include <sgi/GenerateItem>

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
    static bool getBoolOption(const osgDB::Options * options, const std::string & key, bool defaultValue=false)
    {
        if(!options)
            return defaultValue;
        std::string val = options->getPluginStringData(key);
        if(val.empty())
            return defaultValue;
        return string_to_bool(val, defaultValue);
    }
    SGIOptions(const osgDB::Options * options=NULL)
    {
        const void * host_callback_plugin_data = options?options->getPluginData("sgi_host_callback"):NULL;
        hostCallback = static_cast<sgi::IHostCallback*>(const_cast<void*>(host_callback_plugin_data));
        showSceneGraphDialog = getBoolOption(options, "showSceneGraphDialog");
    }
    osg::ref_ptr<sgi::IHostCallback> hostCallback;
    bool showSceneGraphDialog;
};

class DefaultSGIProxy : public osg::Referenced
{
public:
    DefaultSGIProxy(osg::Camera * camera, const SGIOptions & options)
        : _parent(NULL)
        , _view(NULL)
        , _hostCallback(new HostCallbackImpl(this, options.hostCallback))
        , _options(options)
    {
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if(_view)
            _view->addEventHandler(new sgi::SceneGraphInspectorHandler(_hostCallback));

        osg::GraphicsContext * ctx = camera->getGraphicsContext();
        if(ctx)
        {
            ensure_QApplication();

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
            OSG_NOTICE << LC << "DefaultSGIProxy parent " << _parent << std::endl;

            if(_parent)
            {
                if(_options.showSceneGraphDialog)
                {
                    SGIHostItemOsg viewItem(_view);
                    showSceneGraphDialog(&viewItem, true);
                }
            }
        }
    }
    virtual ~DefaultSGIProxy()
    {
    }
public:
    IContextMenu * contextMenu(const SGIHostItemBase * item)
    {
        OSG_NOTICE << LC << "contextMenu hostitem " << item << " parent " << _parent << std::endl;
        if(!_contextMenu.valid())
            _contextMenu = sgi::createContextMenu<autoload::Osg>(_parent, item, _hostCallback);
        else
            _contextMenu->setObject(item);
        return _contextMenu.get();
    }
    IContextMenu * contextMenu(const SGIItemBase * item)
    {
        OSG_NOTICE << LC << "contextMenu item " << item << std::endl;
        bool ret = _contextMenu.valid();
        if(ret)
            _contextMenu->setObject(const_cast<SGIItemBase*>(item));
        return _contextMenu.get();
    }
    bool showSceneGraphDialog(osg::Node * node, bool show=true)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog node " << node << std::endl;
        if(!_dialog.valid())
        {
            SGIHostItemOsg item(node);
            _dialog = sgi::showSceneGraphDialogImpl<autoload::Osg>(_parent, &item, _hostCallback);
        }
        else
        {
            SGIHostItemOsg item(node);
            _dialog->setObject(&item);
        }
        if(_dialog.valid() && show)
            _dialog->show();
        return _dialog.valid();
    }
    ISceneGraphDialog * showSceneGraphDialog(const SGIHostItemBase * item, bool show=true)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog hostitem " << item << std::endl;
        if(!_dialog.valid())
            _dialog = sgi::showSceneGraphDialog<autoload::Osg>(_parent, item, _hostCallback);
        else
            _dialog->setObject(item);
        if(_dialog.valid() && show)
            _dialog->show();
        return _dialog.get();
    }
    ISceneGraphDialog * showSceneGraphDialog(SGIItemBase * item)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog item " << item << std::endl;
        bool ret = _dialog.valid();
        if(ret)
        {
            _dialog->setObject(item);
            _dialog->show();
        }
        return _dialog.get();
    }
    IObjectLoggerDialog * showObjectLoggerDialog(const SGIHostItemBase * item, bool show=true)
    {
        OSG_NOTICE << LC << "showObjectLoggerDialog hostitem " << item << std::endl;
        if(!_loggerDialog.valid())
            _loggerDialog = sgi::showObjectLoggerDialog<autoload::Osg>(_parent, item, _hostCallback);
        if(_loggerDialog.valid() && show)
            _loggerDialog->show();
        return _loggerDialog.get();
    }
    IObjectLoggerDialog * showObjectLoggerDialog(SGIItemBase * item)
    {
        OSG_NOTICE << LC << "showObjectLoggerDialog item " << item << std::endl;
        bool ret = _loggerDialog.valid();
        if(ret)
            _loggerDialog->show();
        return _loggerDialog.get();
    }
    IImagePreviewDialog * showImagePreviewDialog(const SGIHostItemBase * item, bool show=true)
    {
        OSG_NOTICE << LC << "showImagePreviewDialog hostitem " << item << std::endl;
        if(!_imagePreviewDialog.valid())
            _imagePreviewDialog = sgi::showImagePreviewDialog<autoload::Osg>(_parent, item, _hostCallback);
        if(_imagePreviewDialog.valid() && show)
            _imagePreviewDialog->show();
        return _imagePreviewDialog.get();
    }
    IImagePreviewDialog * showImagePreviewDialog(SGIItemBase * item)
    {
        OSG_NOTICE << LC << "showImagePreviewDialog item " << item << std::endl;
        bool ret = _imagePreviewDialog.valid();
        if(ret)
        {
            _imagePreviewDialog->setObject(item);
            _imagePreviewDialog->show();
        }
        return _imagePreviewDialog.get();
    }
    SGIItemBase * getView()
    {
        if(!_viewPtr.valid() && _view)
        {
            SGIHostItemOsg viewHostItem(_view);
            sgi::generateItem<autoload::Osg>(_viewPtr, &viewHostItem);
        }
        return _viewPtr.get();
    }

    class HostCallbackImpl : public IHostCallback
    {
    public:
        HostCallbackImpl(DefaultSGIProxy * parent, IHostCallback * callback)
            : _parent(parent) {}
        virtual IContextMenu * contextMenu(QWidget * /*parent*/, const SGIItemBase* item) override
        {
            return _parent->contextMenu(item);
        }
        virtual IContextMenu * contextMenu(QWidget * /*parent*/, const SGIHostItemBase * item) override
        {
            return _parent->contextMenu(item);
        }
        virtual ISceneGraphDialog * showSceneGraphDialog(QWidget * /*parent*/, SGIItemBase * item) override
        {
            return _parent->showSceneGraphDialog(item);
        }
        virtual ISceneGraphDialog * showSceneGraphDialog(QWidget * /*parent*/, const SGIHostItemBase * item) override
        {
            return _parent->showSceneGraphDialog(item);
        }
        virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget * /*parent*/, SGIItemBase * item) override
        {
            return _parent->showObjectLoggerDialog(item);
        }
        virtual IObjectLoggerDialog * showObjectLoggerDialog(QWidget * /*parent*/, const SGIHostItemBase * item) override
        {
            return _parent->showObjectLoggerDialog(item);
        }
        virtual IImagePreviewDialog * showImagePreviewDialog(QWidget * /*parent*/, SGIItemBase * item) override
        {
            return _parent->showImagePreviewDialog(item);
        }
        virtual IImagePreviewDialog * showImagePreviewDialog(QWidget * /*parent*/, const SGIHostItemBase * item) override
        {
            return _parent->showImagePreviewDialog(item);
        }
        virtual ReferencedPickerBase * createPicker(PickerType type, float x, float y)
        {
            ReferencedPickerBase * ret = NULL;
            osg::Node * root = NULL;
            float buffer = 1.0f;
            unsigned traversalMask = ~0u;

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
            _parent->_view->requestRedraw();
        }

    private:
        DefaultSGIProxy * _parent;
    };

private:
    QWidget * _parent;
    osgViewer::View * _view;
    SGIItemBasePtr _viewPtr;
    sgi::IContextMenuPtr _contextMenu;
    sgi::ISceneGraphDialogPtr _dialog;
    sgi::IObjectLoggerDialogPtr _loggerDialog;
    sgi::IImagePreviewDialogPtr _imagePreviewDialog;
    IHostCallbackPtr _hostCallback;
    sgi::SGIOptions _options;
};
} // namespace sgi

class SGIInstallNode : public osg::Node
{
public:
    SGIInstallNode(const osgDB::Options * options=NULL)
        : osg::Node()
        , _options(options)
        , _installed(false)
    {
    }
    SGIInstallNode(const SGIInstallNode & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Node(rhs, copyop)
        , _options(rhs._options)
        , _installed(false)
    {
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
                            installToCamera(cv->getCurrentCamera());
                            _installed = true;
                        }
                    }
                }
            }
            nv.popFromNodePath();
        }
    }
private:
    void installToCamera(osg::Camera * camera)
    {
        sgi::DefaultSGIProxy * proxy = new sgi::DefaultSGIProxy(camera, _options);
    }

private:
    OpenThreads::Mutex _mutex;
    sgi::SGIOptions _options;
    bool _installed;
};


class ReaderWriteSGI : public osgDB::ReaderWriter
{
public:
    ReaderWriteSGI()
    {
        supportsExtension( "sgi_loader", "SGI loader" );
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

        return ReadResult(new SGIInstallNode(options));
    }
};

REGISTER_OSGPLUGIN(earth, ReaderWriteSGI)

