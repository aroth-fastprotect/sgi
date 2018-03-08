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
    MenuActionObjectModifyProperty,
    MenuActionImagePreview,
};

ACTION_HANDLER_IMPL_TEMPLATE()

} // namespace qt_plugin
} // namespace sgi
