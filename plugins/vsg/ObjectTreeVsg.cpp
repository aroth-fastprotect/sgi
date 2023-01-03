#include "stdafx.h"
#include "ObjectTreeVsg.h"
#include "SGIItemVsg"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/string>

#include <vsg/io/ObjectFactory.h>

namespace sgi {
namespace vsg_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(vsg::Object)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(vsg::ObjectFactory)

bool objectTreeBuildImpl<vsg::Object>::build(IObjectTreeItem * treeItem)
{
    vsg::Object * object_ptr = getObject<vsg::Object,SGIItemVsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<vsg::ObjectFactory>::build(IObjectTreeItem * treeItem)
{
    vsg::ObjectFactory * object_ptr = getObject<vsg::ObjectFactory,SGIItemVsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
            ret = callNextHandler(treeItem);
            if(ret)
            {
            }
            break;
    default:
            ret = callNextHandler(treeItem);
            break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

struct ObjectFactorySingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemVsg hostItem(vsg::ObjectFactory::instance());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

    SGIHostItemInternal ObjectFactoryItem(new SGIProxyItemT<ObjectFactorySingleton>(_hostInterface, "vsg::ObjectFactory"));
    treeItem->addChild(std::string(), &ObjectFactoryItem);

    return true;
}


} // namespace vsg_plugin
} // namespace sgi
