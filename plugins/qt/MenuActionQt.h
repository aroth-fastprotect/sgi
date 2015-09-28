#pragma once

#include <sgi/plugins/ActionHandlerImpl>

namespace sgi {
namespace qt_plugin {

enum MenuAction {
    MenuActionObjectInfo,
    MenuActionDumpObjectInfo,
    MenuActionDumpObjectTree,
    MenuActionWidgetGrab,
    MenuActionWidgetSetVisibility,
    MenuActionWidgetSetEnabled,
    MenuActionWidgetSetAcceptDrops,
    MenuActionWidgetSetAutoFillBackground,
    MenuActionWidgetHighlight,
    MenuActionObjectMethodInvoke,
    MenuActionImagePreview
};

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDumpObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionDumpObjectTree)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetGrab)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetSetVisibility)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetSetEnabled)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetSetAcceptDrops)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetSetAutoFillBackground)
ACTION_HANDLER_IMPL_DECLARE(MenuActionWidgetHighlight)
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectMethodInvoke)
ACTION_HANDLER_IMPL_DECLARE(MenuActionImagePreview)

} // namespace qt_plugin
} // namespace sgi
