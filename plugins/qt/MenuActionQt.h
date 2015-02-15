#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemQt.h>


namespace sgi {
namespace qt_plugin {

enum MenuAction {
    MenuActionObjectInfo,
    MenuActionDumpObjectInfo,
    MenuActionDumpObjectTree,
    MenuActionObjectMethodInvoke,
    MenuActionImagePreview
};

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDumpObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDumpObjectTree)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectMethodInvoke)
ACTION_HANDLER_IMPL_DECLARE(MenuActionImagePreview)

} // namespace qt_plugin
} // namespace sgi
