#pragma once
#include <sgi/plugins/ContextMenuImpl>

namespace sgi {
namespace qt_plugin {

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()

typedef details::ReferencedDataT<QMetaMethod> ReferencedDataMetaMethod;

} // namespace qt_plugin
} // namespace sgi
