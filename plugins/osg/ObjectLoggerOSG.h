#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/ObjectLoggerBase>

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

} // namespace osg_plugin
} // namespace sgi
