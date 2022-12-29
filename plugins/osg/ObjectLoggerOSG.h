#pragma once

#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/ObjectLoggerBase>
#include <sgi/plugins/ObjectLoggerImpl>

// can be removed when done
#include <osg/NodeVisitor>
#include <osg/CullStack>

namespace osg {
    class Node;
}

namespace sgi {
namespace osg_plugin {

GET_OR_CREATE_OBJECTLOGGER_IMPL_TEMPLATE()
GET_OR_CREATE_OBJECTLOGGER_IMPL_DECLARE(osg::Node)

class ObjectLogger : public sgi::ObjectLoggerBase
{
public:
    ObjectLogger(osg::Camera * camera=nullptr, SGIPluginHostInterface * hostInterface=nullptr);
    ObjectLogger(const ObjectLogger & rhs);
    ~ObjectLogger() override;

    SGI_Object(sgi, ObjectLogger)

public:
    static ObjectLogger *   getLogger(SGIItemBase * item);
    static ObjectLogger *   getOrCreateLogger(SGIItemBase * item, SGIPluginHostInterface * hostInterface);

    bool                    addItem(SGIItemBase * item, bool alsoChilds) override;
    bool                    removeItem(SGIItemBase * item) override;

    bool                    isNodeActive(osg::Node * node) const;

    IObjectLoggerDialog *   getOrCreateDialog(QWidget *parent, IHostCallback * callback) override;

protected:
    static ObjectLogger * getLoggerFromCamera(osg::Camera * camera);
    void setup(osg::Camera * camera);
    void addEntry(const osg::NodeVisitor * nv, bool preApply, osg::Node & node);

protected:
    class CullVisitor;
    class UpdateVisitor;
    class EventVisitor;
    class Inserter;
    typedef std::set<osg::Node*>    NodeSet;

protected:
    osg::ref_ptr<osg::Camera>       _activeCamera;
    osg::ref_ptr<CullVisitor>       _cullVisitor;
    osg::ref_ptr<UpdateVisitor>     _updateVisitor;
    osg::ref_ptr<EventVisitor>      _eventVisitor;
    NodeSet                         _activeNodes;
};

class CullingInfo;
typedef osg::ref_ptr<CullingInfo> CullingInfoPtr;
typedef std::vector<CullingInfoPtr> CullingInfoPtrList;

class CullingInfoForCamera : public osg::Object
{
public:
    CullingInfoForCamera(osg::Camera * camera = nullptr, SGIPluginHostInterface * hostInterface = nullptr);
    CullingInfoForCamera(const CullingInfoForCamera & rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~CullingInfoForCamera();

    META_Object(sgi, CullingInfoForCamera)

    static CullingInfoForCamera * getCullingInfoForCamera(osg::Camera * camera);
    static CullingInfoForCamera * getOrCreateCullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface);


public:
    bool                    isNodeActive(osg::Node * node) const;
    bool                    enableNode(osg::Node * node, bool enable);

    const CullingInfoPtrList & activeNodes() const {
        return _activeNodes;
    }

    osg::Camera*            camera() const {
        return _activeCamera.get();
    }

protected:
    void setup(osg::Camera * camera);

protected:
    class CullVisitor;
    class CullCallbackHandler;

protected:
    osg::ref_ptr<osg::Camera>       _activeCamera;
    osg::ref_ptr<CullVisitor>       _cullVisitor;
    CullingInfoPtrList              _activeNodes;
};

class CullingInfoRegistry : public osg::Object
{
public:
    CullingInfoRegistry();
    ~CullingInfoRegistry();
    CullingInfoRegistry(const CullingInfoRegistry & rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Object(sgi, CullingInfoRegistry)

    typedef osg::observer_ptr<osg::Camera> CameraPtr;
    typedef osg::ref_ptr<CullingInfoForCamera> CullingInfoForCameraPtr;
    typedef std::map<CameraPtr, CullingInfoForCameraPtr > CameraCullingInfoMap;
    const CameraCullingInfoMap & map() const {
        return _map;
    }

    static CullingInfoRegistry * instance();

    void add(osg::Camera * camera, CullingInfoForCamera * cullInfo);
    void remove(osg::Camera * camera, CullingInfoForCamera * cullInfo);

    CullingInfoForCamera * getCullingInfoForCamera(osg::Camera * camera);
    CullingInfoForCamera * getOrCreateCullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface);

protected:
    OpenThreads::Mutex _mutex;
    CameraCullingInfoMap _map;
};

class CullingNodeInfo : public osg::Referenced
{
public:
	static unsigned getCullingMask(osgUtil::CullVisitor* cv, osg::Node& node);
	static unsigned getCullingMask(osgUtil::CullVisitor* cv, osg::BoundingSphere& bs);

public:
	CullingNodeInfo(const osg::ref_ptr<osg::Node>& n) : node(n) {}
    osg::ref_ptr<osg::Node> node;

    struct Info {
        Info() : boundingSphereComputed(false), cullingMask(0) {}
        osg::CullStack cullStack;
        osg::BoundingSphere boundingSphere;
        bool boundingSphereComputed;
        unsigned cullingMask;
		void from(osgUtil::CullVisitor* cv, osg::Node& node);
    };
    Info before;
    Info after;
};
typedef osg::ref_ptr<CullingNodeInfo> CullingNodeInfoPtr;
typedef std::vector<CullingNodeInfoPtr> CullingNodeInfoPath;

class CullingInfo : public osg::Object
{
    friend class CullingInfoForCamera;
private:
    CullingInfo(osg::Node * node=nullptr, osg::Camera * camera=nullptr);
    CullingInfo(const CullingInfo & rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Object(sgi, CullingInfo)

public:
    static bool isPresent(osg::Node * node);
    static bool enable(osg::Node * node, bool enable, SGIPluginHostInterface * hostInterface);

    typedef std::vector<CullingNodeInfoPath> CullingNodeInfoPathList;

    const osg::Node * node() const {
        return _node;
    }
    const CullingNodeInfoPathList & pathlist() {
        return _pathlist;
    }

protected:
    osg::Node * _node;
    CullingNodeInfoPathList _pathlist;
};


} // namespace osg_plugin
} // namespace sgi
