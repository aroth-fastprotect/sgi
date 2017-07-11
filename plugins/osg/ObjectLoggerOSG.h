#pragma once

#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/ObjectLoggerBase>
#include <sgi/plugins/ObjectLoggerImpl>

// can be removed when done
#include <osg/NodeVisitor>

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
    ObjectLogger(osg::Camera * camera=NULL, SGIPluginHostInterface * hostInterface=NULL);
    ObjectLogger(const ObjectLogger & rhs,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
    virtual ~ObjectLogger();

    META_Object(sgi, ObjectLogger);

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

class CullingInfoForCamera : public osg::Object
{
public:
    CullingInfoForCamera(osg::Camera * camera = NULL, SGIPluginHostInterface * hostInterface = NULL);
    CullingInfoForCamera(const CullingInfoForCamera & rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    virtual ~CullingInfoForCamera();

    META_Object(sgi, CullingInfoForCamera);

    static CullingInfoForCamera * getCullingInfoForCamera(osg::Camera * camera);
    static CullingInfoForCamera * getOrCreateCullingInfoForCamera(osg::Camera * camera, SGIPluginHostInterface * hostInterface);

public:
    bool                    isNodeActive(osg::Node * node) const;

    bool                    enableNode(osg::Node * node, bool enable);

protected:
    void setup(osg::Camera * camera);

protected:
    class CullVisitor;
    class CullCallbackHandler;
    
    typedef std::vector<CullingInfoPtr> CullingInfoPtrList;

protected:
    osg::ref_ptr<osg::Camera>       _activeCamera;
    osg::ref_ptr<CullVisitor>       _cullVisitor;
    CullingInfoPtrList              _activeNodes;
};

class CullingInfo : public osg::Object
{
    friend class CullingInfoForCamera;
private:
    CullingInfo(osg::Node * node=nullptr, osg::Camera * camera=nullptr);
    CullingInfo(const CullingInfo & rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

    META_Object(sgi, CullingInfo);

public:
    static bool isPresent(osg::Node * node);
    static bool enable(osg::Node * node, bool enable, SGIPluginHostInterface * hostInterface);

    struct NodeCullInfo {
        NodeCullInfo(const osg::ref_ptr<osg::Node> & n) : node(n), culled(false) {}
        osg::ref_ptr<osg::Node> node;
        bool culled;
    };
    typedef std::vector<NodeCullInfo> NodeCullInfoPath;
    typedef std::vector< NodeCullInfoPath> NodeCullInfoPathList;

    const osg::Node * node() const {
        return _node;
    }
    const NodeCullInfoPathList & pathlist() {
        return _pathlist;
    }

protected:
    osg::Node * _node;
    NodeCullInfoPathList _pathlist;
};


} // namespace osg_plugin
} // namespace sgi
