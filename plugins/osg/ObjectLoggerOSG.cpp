#include "stdafx.h"
#include "ObjectLoggerOSG.h"
#include "SGIItemOsg"

#include <osg/ProxyNode>
#include <osg/ClipNode>
#include <osg/CameraView>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/TexGenNode>
#include <osg/Sequence>
#include <osg/OcclusionQueryNode>

#include <osgViewer/ViewerBase>
#include <osgViewer/Renderer>
#include <osgViewer/View>

#include <osgUtil/CullVisitor>
#include <osgUtil/UpdateVisitor>
#include <osgGA/EventVisitor>

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/helpers/osg>

#include <cassert>

namespace sgi {


typedef SGIDataFieldT<osg::NodeVisitor::VisitorType> SGIDataFieldVisitorType;

template<>
inline std::string SGIDataFieldT<osg::NodeVisitor::VisitorType>::toString(SGIPluginHostInterface * hostInterface) const
{
    switch(_value)
    {
    case osg::NodeVisitor::NODE_VISITOR: return "node";
    case osg::NodeVisitor::UPDATE_VISITOR: return "update";
    case osg::NodeVisitor::EVENT_VISITOR: return "event";
    case osg::NodeVisitor::COLLECT_OCCLUDER_VISITOR: return "collect";
    case osg::NodeVisitor::CULL_VISITOR: return "cull";
    default: break;
    }
    return "unknown";
}

namespace osg_plugin {


template<typename OPERATION, class BASE>
class NodeVisitorHook : public BASE
{
public:
    NodeVisitorHook(BASE * original)
        : _original(original), _operation()
    {
    }

    void release()
    {
        _original = NULL;
    }
protected:
    virtual void copyStateToOriginal() = 0;
    virtual void copyStateFromOriginal() = 0;

public:
#define NodeVisitorHook_apply() \
    assert(_original.valid()); \
	_operation.preApply(this, node); \
	copyStateToOriginal(); \
	_original->apply(node); \
	copyStateFromOriginal(); \
	_operation.postApply(this, node);

    virtual void apply(osg::Node& node) override
    {
		NodeVisitorHook_apply();
    }
    virtual void apply(osg::Geode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Billboard& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Group& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::ProxyNode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Projection& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::CoordinateSystemNode& node) override
    {
		NodeVisitorHook_apply();
	}


    virtual void apply(osg::ClipNode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::TexGenNode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::LightSource& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Transform& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Camera& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::CameraView& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::MatrixTransform& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::PositionAttitudeTransform& node) override
    {
		NodeVisitorHook_apply();
	}
    virtual void apply(osg::Switch& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::Sequence& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::LOD& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::PagedLOD& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::ClearNode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::OccluderNode& node) override
    {
		NodeVisitorHook_apply();
	}

    virtual void apply(osg::OcclusionQueryNode& node) override
    {
		NodeVisitorHook_apply();
	}

protected:
    osg::ref_ptr<BASE> _original;
    OPERATION _operation;
};

template<typename OPERATION>
class CullVisitorHook : public NodeVisitorHook<OPERATION, osgUtil::CullVisitor>
{
public:
    typedef NodeVisitorHook<OPERATION, osgUtil::CullVisitor> _Base;

    CullVisitorHook(osg::Camera * camera)
        : _Base(NULL), _camera(camera)
    {
        osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(_camera->getRenderer());
        osgUtil::SceneView * sceneView = renderer?renderer->getSceneView(0):NULL;
        if(sceneView)
        {
            _Base::_original = sceneView->getCullVisitor();
            copyStateFromOriginal();
            sceneView->setCullVisitor(this);
        }
    }
    virtual ~CullVisitorHook()
    {
        release();
    }
    void release()
    {
        if(_Base::_original.valid())
        {
            copyStateToOriginal();
            osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(_camera->getRenderer());
            osgUtil::SceneView * sceneView = renderer?renderer->getSceneView(0):NULL;
            if(sceneView)
                sceneView->setCullVisitor(_Base::_original.get());
            _Base::_original = NULL;
        }
        _camera = NULL;
        _Base::release();
    }

    virtual osg::Vec3 getEyePoint() const { return _Base::_original->getEyePoint(); }
    virtual osg::Vec3 getViewPoint() const { return _Base::_original->getViewPoint(); }

    virtual float getDistanceToEyePoint(const osg::Vec3& pos, bool withLODScale) const
        { return _Base::_original->getDistanceToEyePoint(pos, withLODScale); }
    virtual float getDistanceFromEyePoint(const osg::Vec3& pos, bool withLODScale) const
        { return _Base::_original->getDistanceFromEyePoint(pos, withLODScale); }
    virtual float getDistanceToViewPoint(const osg::Vec3& pos, bool withLODScale) const
        { return _Base::_original->getDistanceToViewPoint(pos, withLODScale); }

    virtual void popProjectionMatrix() { _Base::_original->popProjectionMatrix(); }
    virtual bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
        { return _Base::_original->clampProjectionMatrixImplementation(projection, znear, zfar); }
    virtual bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
        { return _Base::_original->clampProjectionMatrixImplementation(projection, znear, zfar); }

protected:
    class CopyCullVisitorState : public osgUtil::CullVisitor
    {
    public:
        void copyStateFrom(const CopyCullVisitorState & original)
        {
            _rootStateGraph = original._rootStateGraph;
            _currentStateGraph = original._currentStateGraph;

            _rootRenderStage = original._rootRenderStage;
            _currentRenderBin = original._currentRenderBin;
            _renderBinStack = original._renderBinStack;

            _traversalNumber = original._traversalNumber;

            _computed_znear = original._computed_znear;
            _computed_zfar = original._computed_zfar;

            _reuseRenderLeafList = original._reuseRenderLeafList;
            _currentReuseRenderLeafIndex = original._currentReuseRenderLeafIndex;

            _numberOfEncloseOverrideRenderBinDetails = original._numberOfEncloseOverrideRenderBinDetails;

            _renderInfo = original._renderInfo;

            _nearPlaneCandidateMap = original._nearPlaneCandidateMap;
            _farPlaneCandidateMap = original._farPlaneCandidateMap;

            _identifier = original._identifier;

            // properties from osg::CullStack
            _occluderList = original._occluderList;

            _projectionStack = original._projectionStack;

            _modelviewStack = original._modelviewStack;
            _MVPW_Stack = original._MVPW_Stack;

            _viewportStack = original._viewportStack;

            _referenceViewPoints = original._referenceViewPoints;
            _eyePointStack = original._eyePointStack;
            _viewPointStack = original._viewPointStack;

            _clipspaceCullingStack = original._clipspaceCullingStack;
            _projectionCullingStack = original._projectionCullingStack;

            _modelviewCullingStack = original._modelviewCullingStack;
            _index_modelviewCullingStack = original._index_modelviewCullingStack;
            _back_modelviewCullingStack = original._back_modelviewCullingStack;

            _frustumVolume = original._frustumVolume;

            _bbCornerNear = original._bbCornerNear;
            _bbCornerFar = original._bbCornerFar;

            _identity = original._identity;

            _reuseMatrixList = original._reuseMatrixList;
            _currentReuseMatrixIndex = original._currentReuseMatrixIndex;

        }
        void copyStateTo(CopyCullVisitorState & original) const
        {
            original._rootStateGraph = _rootStateGraph;
            original._currentStateGraph = _currentStateGraph;

            original._rootRenderStage = _rootRenderStage;
            original._currentRenderBin = _currentRenderBin;
            original._renderBinStack = _renderBinStack;

            original._traversalNumber = _traversalNumber;

            original._computed_znear = _computed_znear;
            original._computed_zfar = _computed_zfar;

            original._reuseRenderLeafList = _reuseRenderLeafList;
            original._currentReuseRenderLeafIndex = _currentReuseRenderLeafIndex;

            original._numberOfEncloseOverrideRenderBinDetails = _numberOfEncloseOverrideRenderBinDetails;

            original._renderInfo = _renderInfo;

            original._nearPlaneCandidateMap = _nearPlaneCandidateMap;
            original._farPlaneCandidateMap = _farPlaneCandidateMap;

            original._identifier = _identifier;

            // properties from osg::CullStack
            original._occluderList = _occluderList;

            original._projectionStack = _projectionStack;

            original._modelviewStack = _modelviewStack;
            original._MVPW_Stack = _MVPW_Stack;

            original._viewportStack = _viewportStack;

            original._referenceViewPoints = _referenceViewPoints;
            original._eyePointStack = _eyePointStack;
            original._viewPointStack = _viewPointStack;

            original._clipspaceCullingStack = _clipspaceCullingStack;
            original._projectionCullingStack = _projectionCullingStack;

            original._modelviewCullingStack = _modelviewCullingStack;
            original._index_modelviewCullingStack = _index_modelviewCullingStack;
            original._back_modelviewCullingStack = _back_modelviewCullingStack;

            original._frustumVolume = _frustumVolume;

            original._bbCornerNear = _bbCornerNear;
            original._bbCornerFar = _bbCornerFar;

            original._identity = _identity;

            original._reuseMatrixList = _reuseMatrixList;
            original._currentReuseMatrixIndex = _currentReuseMatrixIndex;

        }
    };
    virtual void copyStateToOriginal() override
    {
        // properties from osgUtil::CullVisitor
        reinterpret_cast<CopyCullVisitorState*>(this)->copyStateTo(* reinterpret_cast<CopyCullVisitorState*>(_Base::_original.get()));
    }
    virtual void copyStateFromOriginal() override
    {
        // properties from osgUtil::CullVisitor
        reinterpret_cast<CopyCullVisitorState*>(this)->copyStateFrom(* reinterpret_cast<CopyCullVisitorState*>(_Base::_original.get()));
    }

protected:
    osg::ref_ptr<osg::Camera> _camera;
};

template<typename OPERATION>
class UpdateVisitorHook : public NodeVisitorHook<OPERATION, osgUtil::UpdateVisitor>
{
public:
    typedef NodeVisitorHook<OPERATION, osgUtil::UpdateVisitor> _Base;
    UpdateVisitorHook(osg::Camera * camera)
        : _Base(NULL), _camera(camera)
    {
        osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
        osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():NULL;
        if(viewer)
        {
            _Base::_original = viewer->getUpdateVisitor();
            copyStateFromOriginal();
            viewer->setUpdateVisitor(this);
        }
    }
    virtual ~UpdateVisitorHook()
    {
        release();
    }

    void release()
    {
        if(_Base::_original.valid())
        {
            copyStateToOriginal();
            osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
            osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():NULL;
            if(viewer)
                viewer->setUpdateVisitor(_Base::_original.get());
            _Base::_original = NULL;
        }
        _camera = NULL;
        _Base::release();
    }
protected:
    virtual void copyStateToOriginal() override
    {
    }
    virtual void copyStateFromOriginal() override
    {
    }
protected:
    osg::ref_ptr<osg::Camera> _camera;
};

template<typename OPERATION>
class EventVisitorHook : public NodeVisitorHook<OPERATION, osgGA::EventVisitor>
{
public:
    typedef NodeVisitorHook<OPERATION, osgGA::EventVisitor> _Base;
    EventVisitorHook(osg::Camera * camera)
        : _Base(NULL), _camera(camera)
    {
        osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
        osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():NULL;
        if(viewer)
        {
            _Base::_original = viewer->getEventVisitor();
            copyStateFromOriginal();
            viewer->setEventVisitor(this);
        }
    }
    virtual ~EventVisitorHook()
    {
        release();
    }

    void release()
    {
        if(_Base::_original.valid())
        {
            copyStateToOriginal();
            osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
            osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():NULL;
            if(viewer)
                viewer->setEventVisitor(_Base::_original.get());
            _Base::_original = NULL;
        }
        _camera = NULL;
        _Base::release();
    }
protected:
    virtual void copyStateToOriginal() override
    {
    }
    virtual void copyStateFromOriginal() override
    {
    }
protected:
    osg::ref_ptr<osg::Camera> _camera;
};

class ObjectLogger::Inserter
{
public:
    template<osg::NodeVisitor::VisitorType VISITORTYPE>
    class LogEntry
    {
    public:
        LogEntry()
            : _logger(NULL)
            {
            }

        void setInspector(ObjectLogger * inspector)
        {
            _logger = inspector;
        }
        const ObjectLogger * getLogger() const
        {
            return _logger;
        }
        ObjectLogger * getLogger()
        {
            return _logger;
        }
        void preApply(osg::NodeVisitor * nv, osg::Node & node)
        {
            if(_logger->isNodeActive(&node))
                _logger->addEntry(nv, true, node);
        }
        void postApply(osg::NodeVisitor * nv, osg::Node & node)
        {
            if(_logger->isNodeActive(&node))
                _logger->addEntry(nv, false, node);
        }

    private:
        ObjectLogger * _logger;
    };
};

class ObjectLogger::CullVisitor : public CullVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::CULL_VISITOR> >
{
public:
    typedef CullVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::CULL_VISITOR> > _Base;
    CullVisitor(ObjectLogger * inspector, osg::Camera * camera)
        : _Base(camera)
    {
        _operation.setInspector(inspector);
    }

    const ObjectLogger * getLogger() const { return _operation.getLogger(); }
    ObjectLogger * getLogger() { return _operation.getLogger(); }
};

class ObjectLogger::UpdateVisitor : public UpdateVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::UPDATE_VISITOR> >
{
public:
    typedef UpdateVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::UPDATE_VISITOR> > _Base;
    UpdateVisitor(ObjectLogger * inspector, osg::Camera * camera)
        : _Base(camera)
    {
        _operation.setInspector(inspector);
    }

    const ObjectLogger * getLogger() const { return _operation.getLogger(); }
    ObjectLogger * getLogger() { return _operation.getLogger(); }
};

class ObjectLogger::EventVisitor : public EventVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::EVENT_VISITOR> >
{
public:
    typedef EventVisitorHook<ObjectLogger::Inserter::LogEntry<osg::NodeVisitor::EVENT_VISITOR> > _Base;
    EventVisitor(ObjectLogger * inspector, osg::Camera * camera)
        : _Base(camera)
    {
        _operation.setInspector(inspector);
    }

    const ObjectLogger * getLogger() const { return _operation.getLogger(); }
    ObjectLogger * getLogger() { return _operation.getLogger(); }
};

//------------------------------------------------------------

ObjectLogger::ObjectLogger(osg::Camera * camera, SGIPluginHostInterface * hostInterface)
    : ObjectLoggerBase(hostInterface)
{
    _fields = new SGIDataItemBase;
    _fields->addField(new SGIDataFieldString("Framestamp"));
    _fields->addField(new SGIDataFieldString("Type"));
    _fields->addField(new SGIDataFieldString("Pre/Post"));
    _fields->addField(new SGIDataFieldString("Node"));

    setup(camera);
}

ObjectLogger::ObjectLogger(const ObjectLogger & rhs,const osg::CopyOp& copyop)
    : ObjectLoggerBase(rhs, copyop)
{
}

ObjectLogger::~ObjectLogger()
{
    setup(NULL);
}

void ObjectLogger::setup(osg::Camera * camera)
{
    if(_activeCamera.valid())
    {
        // restore old camera
        _updateVisitor->release();
        _cullVisitor->release();
        _eventVisitor->release();
    }

    if(camera)
    {
        _updateVisitor = new UpdateVisitor(this, camera);
        _cullVisitor = new CullVisitor(this, camera);
        _eventVisitor = new EventVisitor(this, camera);
    }
    else
    {
        _updateVisitor = NULL;
        _cullVisitor = NULL;
        _eventVisitor = NULL;
    }
    _activeCamera = camera;
}

bool ObjectLogger::addItem(SGIItemBase * item, bool alsoChilds)
{
    bool ret = ObjectLoggerBase::addItem(item, alsoChilds);
    if(ret)
    {
        osg::Node* object = static_cast<osg::Node*>(static_cast<const SGIItemOsg*>(item)->object());
        _activeNodes.insert(object);
    }
    return ret;
}

bool ObjectLogger::removeItem(SGIItemBase * item)
{
    bool ret = ObjectLoggerBase::removeItem(item);
    if(ret)
    {
        osg::Node* object = static_cast<osg::Node*>(static_cast<const SGIItemOsg*>(item)->object());
        _activeNodes.erase(object);
    }
    return ret;
}

bool ObjectLogger::isNodeActive(osg::Node * node) const
{
    return _activeNodes.find(node) != _activeNodes.end();
}

void ObjectLogger::addEntry(const osg::NodeVisitor * nv, bool preApply, osg::Node & node)
{
    SGIDataItemBase * entry = new SGIDataItemBase;

    const osg::FrameStamp * fs = nv->getFrameStamp();
    std::stringstream ss;
    ss << fs->getFrameNumber() << "/" << fs->getReferenceTime() << "@" << fs->getSimulationTime();

    SGIItemBasePtr itemNode;
    SGIHostItemOsg hostItemNode(&node);
    _hostInterface->generateItem(itemNode, &hostItemNode);

    entry->addField(new SGIDataFieldString(ss.str()));
    entry->addField(new SGIDataFieldVisitorType(nv->getVisitorType()));
    entry->addField(new SGIDataFieldBool(preApply));
    entry->addField(new SGIDataFieldItemBasePtr(itemNode));
    addLogItem(entry);
}

ObjectLogger * ObjectLogger::getLoggerFromCamera(osg::Camera * camera)
{
    ObjectLogger * ret = NULL;
    osgViewer::View * view = camera?dynamic_cast<osgViewer::View*>(camera->getView()):NULL;
    osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():NULL;
    if(viewer)
    {
        osgUtil::UpdateVisitor * updateVisitorBase = viewer->getUpdateVisitor();
        UpdateVisitor * updateVisitor = dynamic_cast<UpdateVisitor*>(updateVisitorBase);
        if(updateVisitor)
            ret = updateVisitor->getLogger();
    }
    return ret;
}

ObjectLogger * ObjectLogger::getLogger(SGIItemBase * item)
{
    osg::Node* object = static_cast<osg::Node*>(static_cast<const SGIItemOsg*>(item)->object());
    osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
    if(!camera)
        camera = osg_helpers::findFirstParentOfType<osg::Camera>(object);

    ObjectLogger * logger = getLoggerFromCamera(camera);
    return logger;
}

ObjectLogger * ObjectLogger::getOrCreateLogger(SGIItemBase * item, SGIPluginHostInterface * hostInterface)
{
    osg::Node* object = static_cast<osg::Node*>(static_cast<const SGIItemOsg*>(item)->object());
    
    osg::Camera * camera = dynamic_cast<osg::Camera*>(object);
    if(!camera)
        camera = osg_helpers::findFirstParentOfType<osg::Camera>(object);

    ObjectLogger * logger = getLoggerFromCamera(camera);
    if(!logger)
    {
        logger = new ObjectLogger(camera, hostInterface);
        logger->addItem(item, false);
    }
    return logger;
}

IObjectLoggerDialog * ObjectLogger::getOrCreateDialog(QWidget *parent, IHostCallback * callback)
{
    if(!_dialog.valid())
    {
        if(callback)
            _dialog = callback->showObjectLoggerDialog(parent, this);
        else
            _dialog = _hostInterface->showObjectLoggerDialog(parent, this, callback);
    }
    return _dialog.get();
}

GET_OR_CREATE_OBJECTLOGGER_IMPL_REGISTER(osg::Node)

bool getOrCreateObjectLoggerImpl<osg::Node>::execute(IObjectLoggerPtr & result)
{
    if(_createIfNotExists)
        result = ObjectLogger::getOrCreateLogger(_item, _hostInterface);
    else
        result = ObjectLogger::getLogger(_item);
    return result.valid();
}

} // namespace osg_plugin
} // namespace sgi
