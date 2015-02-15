#pragma once
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>

namespace sgi {
namespace log4cplus_plugin {

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectLogger)
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectHierarchy)
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectAppender)
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectLayout)
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectSimpleLayout)
OBJECT_TREE_BUILD_IMPL_DECLARE(Log4cplusObjectPatternLayout)

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(ISceneGraphDialog)

} // namespace qt_plugin
} // namespace sgi
