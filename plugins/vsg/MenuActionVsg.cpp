#include "stdafx.h"
#include "MenuActionVsg.h"
#include "SGIItemVsg"

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIHostItemLog4cplus.h>

namespace sgi {

namespace vsg_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
    return true;
}

} // namespace vsg_plugin
} // namespace sgi
