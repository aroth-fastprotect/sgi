#include "stdafx.h"
#include "ContextMenuVsg.h"
#include "SGIItemVsg"
#include <sgi/helpers/string>
#include "MenuActionVsg.h"

namespace sgi {

namespace vsg_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(vsg::Object)

bool contextMenuPopulateImpl<vsg::Object>::populate(IContextMenuItem * menuItem)
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
