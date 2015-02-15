#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>


namespace sgi {
namespace log4cplus_plugin {

enum MenuAction {
    MenuActionObjectInfo,
    MenuActionLoggerLogLevel
};

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLoggerLogLevel)

} // namespace log4cplus_plugin
} // namespace sgi
