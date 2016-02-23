#pragma once
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemQt.h>

class QObject;
struct QMetaObject;
class QWidget;
class QWindow;
class QWidgetWindow;
class QSurface;
#ifdef WITH_QTOPENGL
class QGLWidget;
#endif
class QCoreApplication;
class QApplication;
class QOpenGLContext;

namespace sgi {
namespace qt_plugin {

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE(QMetaObject)
OBJECT_TREE_BUILD_IMPL_DECLARE(QObject)
OBJECT_TREE_BUILD_IMPL_DECLARE(QWidget)
OBJECT_TREE_BUILD_IMPL_DECLARE(QWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE(QWidgetWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE(QSurface)
OBJECT_TREE_BUILD_IMPL_DECLARE(QCoreApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE(QApplication)
OBJECT_TREE_BUILD_IMPL_DECLARE(QPaintDevice)
OBJECT_TREE_BUILD_IMPL_DECLARE(QImage)
OBJECT_TREE_BUILD_IMPL_DECLARE(QOpenGLContext)

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(ISceneGraphDialog)

} // namespace qt_plugin
} // namespace sgi
