#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>

namespace sgi {

class SGIItemLog4cplus;

namespace log4cplus_plugin {

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_NAME_IMPL_TEMPLATE()
GET_OBJECT_NAME_IMPL_DECLARE(Log4cplusObjectBase)
GET_OBJECT_NAME_IMPL_DECLARE(Log4cplusObjectLogger)
GET_OBJECT_NAME_IMPL_DECLARE(Log4cplusObjectAppender)

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE(Log4cplusObjectBase)

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------
GET_OBJECT_PATH_IMPL_TEMPLATE()
GET_OBJECT_PATH_IMPL_DECLARE(Log4cplusObjectLogger)

} // namespace log4cplus_plugin
} // namespace sgi
