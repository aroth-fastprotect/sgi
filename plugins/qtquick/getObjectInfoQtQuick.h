#pragma once

#include <sgi/plugins/GetObjectInfoImpl>
#include <sgi/plugins/ConvertToImageImpl>

namespace sgi {
namespace qt_plugin {

//--------------------------------------------------------------------------------
// getObjectNameImpl
//--------------------------------------------------------------------------------
GET_OBJECT_NAME_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
GET_OBJECT_TYPE_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------
GET_OBJECT_PATH_IMPL_TEMPLATE()

//--------------------------------------------------------------------------------
// convertToImageConvertImpl
//--------------------------------------------------------------------------------
CONVERT_TO_IMAGE_CONVERT_IMPL_TEMPLATE()

} // namespace qt_plugin
} // namespace sgi
