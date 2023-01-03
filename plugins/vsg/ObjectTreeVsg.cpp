#include "stdafx.h"
#include "ObjectTreeVsg.h"
#include "SGIItemVsg"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/string>

namespace sgi {
namespace vsg_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(vsg::Object)

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

} // namespace vsg_plugin
} // namespace sgi
