#pragma once
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>

namespace sgi {
namespace log4cplus_plugin {

WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectBase)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectLogger)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectHierarchy)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectAppender)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectSimpleLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE(Log4cplusObjectPatternLayout)


} // namespace log4cplus_plugin

} // namespace sgi
