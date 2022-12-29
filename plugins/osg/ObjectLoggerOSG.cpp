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
#include <osg/Polytope>

#include <osgViewer/ViewerBase>
#include <osgViewer/Renderer>
#include <osgViewer/View>

#include <osgUtil/CullVisitor>
#include <osgUtil/UpdateVisitor>
#include <osgGA/EventVisitor>

#include <OpenThreads/ScopedLock>

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/helpers/osg>

#include <cassert>

#include "osg_accessor.h"

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
        _original = nullptr;
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
        : _Base(nullptr), _camera(camera)
    {
        osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(_camera->getRenderer());
        osgUtil::SceneView * sceneView = renderer?renderer->getSceneView(0):nullptr;
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
            osgUtil::SceneView * sceneView = renderer?renderer->getSceneView(0):nullptr;
            if(sceneView)
                sceneView->setCullVisitor(_Base::_original.get());
            _Base::_original = nullptr;
        }
        _camera = nullptr;
        _Base::release();
    }

    osg::Vec3 getEyePoint() const override { return _Base::_original->getEyePoint(); }
    osg::Vec3 getViewPoint() const override { return _Base::_original->getViewPoint(); }

    float getDistanceToEyePoint(const osg::Vec3& pos, bool withLODScale) const override
        { return _Base::_original->getDistanceToEyePoint(pos, withLODScale); }
    float getDistanceFromEyePoint(const osg::Vec3& pos, bool withLODScale) const override
        { return _Base::_original->getDistanceFromEyePoint(pos, withLODScale); }
    float getDistanceToViewPoint(const osg::Vec3& pos, bool withLODScale) const override
        { return _Base::_original->getDistanceToViewPoint(pos, withLODScale); }

    void popProjectionMatrix() override { _Base::_original->popProjectionMatrix(); }
    bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const override
        { return _Base::_original->clampProjectionMatrixImplementation(projection, znear, zfar); }
    bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const override
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
        : _Base(nullptr), _camera(camera)
    {
        osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
        osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():nullptr;
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
            osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():nullptr;
            if(viewer)
                viewer->setUpdateVisitor(_Base::_original.get());
            _Base::_original = nullptr;
        }
        _camera = nullptr;
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
        : _Base(nullptr), _camera(camera)
    {
        osgViewer::View * view = dynamic_cast<osgViewer::View*>(_camera->getView());
        osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():nullptr;
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
            osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():nullptr;
            if(viewer)
                viewer->setEventVisitor(_Base::_original.get());
            _Base::_original = nullptr;
        }
        _camera = nullptr;
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
            : _logger(nullptr)
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

ObjectLogger::ObjectLogger(const ObjectLogger & rhs)
    : ObjectLoggerBase(rhs)
{
}

ObjectLogger::~ObjectLogger()
{
    setup(nullptr);
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
        _updateVisitor = nullptr;
        _cullVisitor = nullptr;
        _eventVisitor = nullptr;
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
    ObjectLogger * ret = nullptr;
    osgViewer::View * view = camera?dynamic_cast<osgViewer::View*>(camera->getView()):nullptr;
    osgViewer::ViewerBase * viewer = (view)?view->getViewerBase():nullptr;
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



class CullingInfoForCamera::CullCallbackHandler
{
public:
    CullingInfoForCamera * inspector()
    {
        return _logger;
    }
    void setInspector(CullingInfoForCamera * inspector)
    {
        _logger = inspector;
    }
    void preApply(osg::NodeVisitor * nv, osg::Node & node);
    void postApply(osg::NodeVisitor * nv, osg::Node & node);

protected:
    CullingNodeInfo * getCullingNodeInfo(osg::Node & node);

private:
    CullingInfoForCamera * _logger;
    osg::NodePathList::const_iterator _currentItem;
};

class CullingInfoForCamera::CullVisitor : public CullVisitorHook<CullingInfoForCamera::CullCallbackHandler>
{
public:
    typedef CullVisitorHook<CullingInfoForCamera::CullCallbackHandler> _Base;
    CullVisitor(CullingInfoForCamera * inspector, osg::Camera * camera)
        : _Base(camera)
    {
        _operation.setInspector(inspector);
    }
	CullingInfoForCamera* inspector()
	{
		return _operation.inspector();
	}
};

CullingNodeInfo * CullingInfoForCamera::CullCallbackHandler::getCullingNodeInfo(osg::Node & node)
{
    CullingNodeInfo * ret = nullptr;
	const osg::Node* pnode = &node;
	for (CullingInfoPtr& cullinfo : _logger->_activeNodes)
    {
        for (const CullingNodeInfoPath & path : cullinfo->pathlist())
        {
            auto pathit = std::find_if(path.begin(), path.end(), [pnode](const CullingNodeInfoPtr & other) { return other->node == pnode; });
            if (pathit != path.end())
                ret = const_cast<CullingNodeInfo *>(pathit->get());
        }
    }
    return ret;
}

namespace {
    class CullingSetAccess : public osg::CullingSet
    {
    public:
        const osg::Vec4 & pixelSizeVector() const {
            return _pixelSizeVector;
        }
        const OccluderList & occluderList() const {
            return _occluderList;
        }
    };
}

unsigned CullingNodeInfo::getCullingMask(osgUtil::CullVisitor * cv, osg::BoundingSphere & bs)
{
    unsigned ret = osg::CullingSet::NO_CULLING;
    const CullingSetAccess & cs = static_cast<const CullingSetAccess&>(cv->getCurrentCullingSet());
    unsigned mask = cs.getCullingMask();

    if (mask&osg::CullingSet::VIEW_FRUSTUM_CULLING)
    {
        osg::Polytope & frustum = const_cast<osg::Polytope &>(cs.getFrustum());
        // is it outside the view frustum...
        if (!frustum.contains(bs)) 
            ret |= osg::CullingSet::VIEW_FRUSTUM_CULLING;
    }

    if (mask&osg::CullingSet::SMALL_FEATURE_CULLING)
    {
        float smallFeatureCullingPixelSize = cs.getSmallFeatureCullingPixelSize();
        const osg::Vec4 & pixelSizeVector = cs.pixelSizeVector();
        if (((bs.center()*pixelSizeVector)*smallFeatureCullingPixelSize) > bs.radius()) 
            ret |= osg::CullingSet::SMALL_FEATURE_CULLING;
    }
#ifdef COMPILE_WITH_SHADOW_OCCLUSION_CULLING
    if (mask&osg::CullingSet::SHADOW_OCCLUSION_CULLING)
    {
        const osg::CullingSet::OccluderList & occluderList = cs.occluderList();
        // is it in one of the shadow occluder volumes.
        if (!occluderList.empty())
        {
            for (osg::CullingSet::OccluderList::const_iterator itr = occluderList.begin();
                itr != occluderList.end();
                ++itr)
            {
                if (const_cast<osg::ShadowVolumeOccluder&>(*itr).contains(bs))
                {
                    ret |= osg::CullingSet::SHADOW_OCCLUSION_CULLING;
                    break;
                }
            }
        }
    }
#endif
    return ret;
}

unsigned CullingNodeInfo::getCullingMask(osgUtil::CullVisitor * cv, osg::Node & node)
{
    unsigned ret = osg::CullingSet::NO_CULLING;
    if (node.isCullingActive())
    {
        osg::BoundingSphere bs = static_cast<NodeAccess&>(node).getBoundNoCompute();
        ret = getCullingMask(cv, bs);
    }
    return ret;
}

void CullingInfoForCamera::CullCallbackHandler::preApply(osg::NodeVisitor * nv, osg::Node & node)
{
    CullVisitor* cv = dynamic_cast<CullVisitor*>(nv->asCullVisitor());
    CullingNodeInfo * info = getCullingNodeInfo(node);
    if (info)
		info->before.from(cv, node);
}

void CullingInfoForCamera::CullCallbackHandler::postApply(osg::NodeVisitor * nv, osg::Node & node)
{
    CullVisitor* cv = dynamic_cast<CullVisitor*>(nv->asCullVisitor());
    CullingNodeInfo * info = getCullingNodeInfo(node);
    if (info)
		info->after.from(cv, node);
}


CullingInfoForCamera::CullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface)
    : osg::Object(hostInterface)
{
    setup(camera);
}

CullingInfoForCamera::CullingInfoForCamera(const CullingInfoForCamera & rhs, const osg::CopyOp& copyop)
    : osg::Object(rhs, copyop)
{
}

CullingInfoForCamera::~CullingInfoForCamera()
{
    setup(nullptr);
}

void CullingInfoForCamera::setup(osg::Camera * camera)
{
    if (_activeCamera.valid())
    {
        // restore old camera
        _cullVisitor->release();
    }

    if (camera)
    {
        _cullVisitor = new CullVisitor(this, camera);
        CullingInfoRegistry::instance()->add(camera, this);
    }
    else
    {
        _cullVisitor = nullptr;
        CullingInfoRegistry::instance()->remove(_activeCamera.get(), this);
    }
    _activeCamera = camera;
}

CullingInfoForCamera * CullingInfoForCamera::getCullingInfoForCamera(osg::Camera * camera)
{
    CullingInfoForCamera * ret = nullptr;
	osgViewer::Renderer* r = camera ? dynamic_cast<osgViewer::Renderer*>(camera->getRenderer()) : nullptr;
	osgUtil::SceneView* s = r ? r->getSceneView(0) : nullptr;
	osgUtil::CullVisitor * cvbase = s ? s->getCullVisitor() : nullptr;
	if(cvbase)
	{
		CullVisitor* cv = dynamic_cast<CullVisitor*>(cvbase);
        if (cv)
            ret = cv->inspector();
    }
    return ret;
}

CullingInfoForCamera * CullingInfoForCamera::getOrCreateCullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface)
{
	CullingInfoForCamera* ret = nullptr;
	osgViewer::Renderer* r = camera ? dynamic_cast<osgViewer::Renderer*>(camera->getRenderer()) : nullptr;
	osgUtil::SceneView* s = r ? r->getSceneView(0) : nullptr;
	osgUtil::CullVisitor* cvbase = s ? s->getCullVisitor() : nullptr;
	if (cvbase)
	{
		CullVisitor* cv = dynamic_cast<CullVisitor*>(cvbase);
		if (cv)
			ret = cv->inspector();
        else
            ret = new CullingInfoForCamera(camera, hostInterface);
    }
    return ret;
}

bool CullingInfoForCamera::isNodeActive(osg::Node * node) const
{
    bool ret = false;
    for (CullingInfoPtr ptr : _activeNodes)
    {
        if (ptr->node() == node)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool CullingInfoForCamera::enableNode(osg::Node * node, bool enable)
{
    bool ret = false;
    bool found = false;

    for(CullingInfoPtrList::iterator it = _activeNodes.begin(); it != _activeNodes.end(); ++it)
    {
        CullingInfoPtr & ptr = *it;
        if (ptr->node() == node)
        {
            if (!enable)
            {
                _activeNodes.erase(it);
                ret = true;
            }
            found = true;
            break;
        }
    }
    if (!found && enable)
    {
        _activeNodes.push_back(new CullingInfo(node, _activeCamera));
        ret = true;
    }
    return ret;
}

namespace {
	class CullStackCopy : public osg::CullStack {
	public:
		CullStackCopy& operator=(const CullStackCopy& rhs)
		{
			_projectionStack = rhs._projectionStack;

			_modelviewStack = rhs._modelviewStack;
			_MVPW_Stack = rhs._MVPW_Stack;

			_viewportStack = rhs._viewportStack;
			_referenceViewPoints = rhs._referenceViewPoints;
			_eyePointStack = rhs._eyePointStack;
			_viewPointStack = rhs._viewPointStack;

			_clipspaceCullingStack = rhs._clipspaceCullingStack;
			_projectionCullingStack = rhs._projectionCullingStack;

			_modelviewCullingStack = rhs._modelviewCullingStack;
			if (rhs._back_modelviewCullingStack != nullptr)
			{
				_index_modelviewCullingStack = _modelviewCullingStack.size() - 1;
				_back_modelviewCullingStack = &_modelviewCullingStack.back();
			}
			return *this;
		}
	};
}


void CullingNodeInfo::Info::from(osgUtil::CullVisitor* cv, osg::Node & node_)
{
	NodeAccess& node = static_cast<NodeAccess&>(node_);
	static_cast<CullStackCopy&>(cullStack) = static_cast<const CullStackCopy&>(static_cast<const osg::CullStack&>(*cv));
	boundingSphereComputed = node.isBoundingSphereComputed();
	boundingSphere = node.getBoundNoCompute();
	cullingMask = getCullingMask(cv, node);
}

CullingInfo::CullingInfo(osg::Node * node, osg::Camera * camera)
    : osg::Object()
    , _node(node)
{
    osg::NodePathList paths = node->getParentalNodePaths(camera);
    _pathlist.resize(paths.size());
    for (unsigned i = 0; i < paths.size(); ++i)
    {
        osg::NodePath & path = paths[i];
        CullingNodeInfoPath & outpath = _pathlist[i];
        for (auto & pathnode : path)
            outpath.push_back(new CullingNodeInfo(pathnode));
    }
}

CullingInfo::CullingInfo(const CullingInfo & rhs, const osg::CopyOp& copyop)
    : osg::Object(rhs, copyop)
    , _node(rhs._node)
    , _pathlist(rhs._pathlist)
{
}

bool CullingInfo::isPresent(osg::Node * node)
{
    bool ret = false;
    osg::Camera * camera = osg_helpers::findCamera(node);
    if (camera)
    {
        CullingInfoForCamera * perCamera = CullingInfoForCamera::getCullingInfoForCamera(camera);
        if(perCamera)
            ret = perCamera->isNodeActive(node);
    }
    return ret;
}

bool CullingInfo::enable(osg::Node * node, bool enable, SGIPluginHostInterface * hostInterface)
{
    bool ret = false;
    osg::Camera * camera = osg_helpers::findCamera(node);
    if (camera)
    {
        CullingInfoForCamera * perCamera = CullingInfoForCamera::getOrCreateCullingInfoForCamera(camera, hostInterface);
        if (perCamera)
            ret = perCamera->enableNode(node, enable);
    }
    return ret;
}

CullingInfoRegistry::CullingInfoRegistry()
	: _mutex(OpenThreads::Mutex::MUTEX_RECURSIVE)
{
}

CullingInfoRegistry::~CullingInfoRegistry()
{
}

CullingInfoRegistry::CullingInfoRegistry(const CullingInfoRegistry & rhs, const osg::CopyOp& copyop)
{
}

CullingInfoRegistry * CullingInfoRegistry::instance()
{
    static osg::ref_ptr<CullingInfoRegistry> s_reg = new CullingInfoRegistry;
    return s_reg.get();
}

CullingInfoForCamera * CullingInfoRegistry::getCullingInfoForCamera(osg::Camera * camera)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    CullingInfoForCamera * ret = nullptr;
    CameraCullingInfoMap::const_iterator it = _map.find(camera);
    if (it != _map.end())
        ret = it->second.get();
    return ret;
}

CullingInfoForCamera * CullingInfoRegistry::getOrCreateCullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    CullingInfoForCamera * ret = nullptr;
    CameraCullingInfoMap::const_iterator it = _map.find(camera);
    if (it != _map.end())
        ret = it->second.get();
    if(!ret)
        ret = CullingInfoForCamera::getOrCreateCullingInfoForCamera(camera, hostInterface);
    return ret;
}

void CullingInfoRegistry::add(osg::Camera * camera, CullingInfoForCamera * cullInfo)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    CameraCullingInfoMap::iterator it = _map.find(camera);
    if (it == _map.end())
        _map.insert(CameraCullingInfoMap::value_type(camera, cullInfo));
    else
        it->second = cullInfo;
}

void CullingInfoRegistry::remove(osg::Camera * camera, CullingInfoForCamera * cullInfo)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    CameraCullingInfoMap::iterator it = _map.find(camera);
    if (it != _map.end())
        _map.erase(it);
}


} // namespace osg_plugin
} // namespace sgi
