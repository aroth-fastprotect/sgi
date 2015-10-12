#include "stdafx.h"
#include "MenuActionLog4cplus.h"
#include "SGIItemLog4cplus"

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIHostItemLog4cplus.h>

namespace sgi {

class SGIItemOsg;

namespace log4cplus_plugin {

ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionLoggerLogLevel)

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    if(callback)
        callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
    else
    {
        ISceneGraphDialog * dlg = _hostInterface->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
        if(dlg)
            dlg->show();
    }
    return true;
}

bool actionHandlerImpl<MenuActionLoggerLogLevel>::execute()
{
    Log4cplusObjectLogger * object_ptr = static_cast<Log4cplusObjectLogger*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Logger object = object_ptr->object();

    object.setLogLevel((log4cplus::LogLevel)menuAction()->mode());
    return true;
}

} // namespace log4cplus_plugin
} // namespace sgi
