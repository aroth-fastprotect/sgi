#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemQt.h>

struct QMetaObject;
class QPaintDevice;

namespace sgi {

class SGIItemQt;

namespace qt_plugin {

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_NAME_IMPL_TEMPLATE()
GET_OBJECT_NAME_IMPL_DECLARE(QObject)
GET_OBJECT_NAME_IMPL_DECLARE(QMetaObject)
GET_OBJECT_NAME_IMPL_DECLARE(QPaintDevice)

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE(QObject)
GET_OBJECT_TYPE_IMPL_DECLARE(QMetaObject)
GET_OBJECT_TYPE_IMPL_DECLARE(QPaintDevice)

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------
GET_OBJECT_PATH_IMPL_TEMPLATE()
GET_OBJECT_PATH_IMPL_DECLARE(QObject)
GET_OBJECT_PATH_IMPL_DECLARE(QMetaObject)

} // namespace qt_plugin
} // namespace sgi
