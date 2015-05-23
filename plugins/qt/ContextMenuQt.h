#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

class QImage;
class QMetaMethod;

namespace sgi {
namespace qt_plugin {

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QObject)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QWidget)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QMetaObject)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QPaintDevice)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QImage)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(QIcon)

typedef ReferencedDataT<QMetaMethod> ReferencedDataMetaMethod;

} // namespace qt_plugin
} // namespace sgi
