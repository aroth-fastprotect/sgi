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
#include <sgi/SceneGraphDialog>
#include <sgi/ObjectLoggerDialog>
#include <sgi/ContextMenu>
#include <sgi/AutoLoadOsg>

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

class DefaultSGIProxy : public osg::Referenced
{
public:
    DefaultSGIProxy(osg::Camera * camera)
        : _parent(NULL)
        , _view(NULL)
        , _inspectorInfo(new SceneGraphInspectorHandlerInfo(this))
        , _dialogInfo(new SceneGraphDialogInfo(this))
        , _loggerDialogInfo(new ObjectLoggerDialogInfo(this))
    {
        _view = dynamic_cast<osgViewer::View*>(camera->getView());
        if(_view)
            _view->addEventHandler(new sgi::SceneGraphInspectorHandler(_inspectorInfo));

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
        }
    }
    virtual ~DefaultSGIProxy()
    {
    }
public:
    bool showSceneGraphDialog(osg::Node * node, bool show=true)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog node " << node << std::endl;
        if(!_dialog.valid())
        {
            SGIHostItemOsg item(node);
            _dialog = sgi::showSceneGraphDialogImpl<autoload::Osg>(_parent, &item, _dialogInfo);
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
    bool showSceneGraphDialog(const SGIHostItemBase * item, bool show=true)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog hostitem " << item << std::endl;
        if(!_dialog.valid())
            _dialog = sgi::showSceneGraphDialog<autoload::Osg>(_parent, item, _dialogInfo);
        else
            _dialog->setObject(item);
        if(_dialog.valid() && show)
            _dialog->show();
        return _dialog.valid();
    }
    bool showSceneGraphDialog(SGIItemBase * item)
    {
        OSG_NOTICE << LC << "showSceneGraphDialog item " << item << std::endl;
        bool ret = _dialog.valid();
        if(ret)
        {
            _dialog->setObject(item);
            _dialog->show();
        }
        return ret;
    }
    bool showObjectLoggerDialog(const SGIHostItemBase * item, bool show=true)
    {
        OSG_NOTICE << LC << "showObjectLoggerDialog hostitem " << item << std::endl;
        if(!_loggerDialog.valid())
            _loggerDialog = sgi::showObjectLoggerDialog<autoload::Osg>(_parent, item, _loggerDialogInfo);
        if(_loggerDialog.valid() && show)
            _loggerDialog->show();
        return _loggerDialog.valid();
    }
    bool showObjectLoggerDialog(SGIItemBase * item)
    {
        OSG_NOTICE << LC << "showObjectLoggerDialog item " << item << std::endl;
        bool ret = _loggerDialog.valid();
        if(ret)
            _loggerDialog->show();
        return ret;
    }
    class SceneGraphDialogInfo : public ISceneGraphDialogInfo
    {
    public:
        SceneGraphDialogInfo(DefaultSGIProxy * parent)
            : _parent(parent) {}

        virtual IContextMenu * contextMenu(QWidget * /*parent*/, const SGIItemBase* /*item*/, IContextMenuInfo * /*info*/)
        {
            return NULL;
        }
        virtual SGIHostItemBase * getView()
        {
            //return _parent->_view->view();
            return NULL;
        }
        virtual void            triggerRepaint()
        {
            _parent->_view->requestRedraw();
        }
        virtual bool            showObjectLoggerDialog(SGIItemBase * item)
        {
            return _parent->showObjectLoggerDialog(item);
        }
    private:
        DefaultSGIProxy * _parent;
    };
    class ObjectLoggerDialogInfo : public IObjectLoggerDialogInfo
    {
    public:
        ObjectLoggerDialogInfo(DefaultSGIProxy * parent)
            : _parent(parent) {}

        virtual IContextMenu * contextMenu(QWidget * /*parent*/, const SGIItemBase* /*item*/, IContextMenuInfo * /*info*/)
        {
            return NULL;
        }
        virtual SGIHostItemBase * getView()
        {
            //return _parent->_view->view();
            return NULL;
        }
        virtual void            triggerRepaint()
        {
            _parent->_view->requestRedraw();
        }
        virtual bool            showSceneGraphDialog(SGIItemBase * item)
        {
            return _parent->showSceneGraphDialog(item);
        }
    private:
        DefaultSGIProxy * _parent;
    };
    class SceneGraphInspectorHandlerInfo : public ISceneGraphInspectorHandlerInfo
    {
    public:
        SceneGraphInspectorHandlerInfo(DefaultSGIProxy * parent)
            : _parent(parent) {}

        virtual bool            showSceneGraphDialog(const SGIHostItemBase * item)
        {
            return _parent->showSceneGraphDialog(item);
        }
        virtual bool            showObjectLoggerDialog(const SGIHostItemBase * item)
        {
            return _parent->showObjectLoggerDialog(item);
        }
        virtual ReferencedPickerBase * createPicker(PickerType type, float x, float y)
        {
            ReferencedPickerBase * ret = NULL;
            osg::Node * root = NULL;
            float buffer = 1.0f;
            unsigned traversalMask = ~0u;

            switch(type)
            {
            case ISceneGraphInspectorHandlerInfo::PickerTypeLine:
                ret = new ReferencedLinePicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask);
                break;
            case ISceneGraphInspectorHandlerInfo::PickerTypeDefault:
                ret = new ReferencedPicker(osg::Vec2f(x,y), _parent->_view, root, traversalMask, buffer);
                break;
            }
            return ret;
        }

    private:
        DefaultSGIProxy * _parent;
    };

private:
    QWidget * _parent;
    osgViewer::View * _view;
    sgi::ISceneGraphDialogPtr _dialog;
    sgi::IObjectLoggerDialogPtr _loggerDialog;
    osg::ref_ptr<SceneGraphInspectorHandlerInfo> _inspectorInfo;
    osg::ref_ptr<SceneGraphDialogInfo> _dialogInfo;
    osg::ref_ptr<ObjectLoggerDialogInfo> _loggerDialogInfo;
};
} // namespace sgi

class SGIInstallNode : public osg::Node
{
public:
    SGIInstallNode()
        : osg::Node()
        , _installed(false)
    {
    }
    SGIInstallNode(const SGIInstallNode & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg::Node(rhs, copyop)
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
        sgi::DefaultSGIProxy * proxy = new sgi::DefaultSGIProxy(camera);
    }

private:
    bool _installed;
    OpenThreads::Mutex _mutex;
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

        return ReadResult(new SGIInstallNode);
    }
};

REGISTER_OSGPLUGIN(earth, ReaderWriteSGI)

