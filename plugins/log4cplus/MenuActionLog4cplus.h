#pragma once

#include <sgi/plugins/ActionHandlerImpl>

namespace sgi {
namespace log4cplus_plugin {

enum MenuAction {
    MenuActionObjectInfo,
    MenuActionLoggerLogLevel
};

ACTION_HANDLER_IMPL_TEMPLATE()

} // namespace log4cplus_plugin
} // namespace sgi
