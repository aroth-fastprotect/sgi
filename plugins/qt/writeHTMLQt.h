#pragma once
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemQt.h>

class QObject;
struct QMetaObject;
class QMetaMethod;
class QMetaProperty;
class QMetaEnum;
class QMetaClassInfo;
class QWidget;
class QDialog;
class QGLWidget;
class QPaintDevice;

namespace sgi {
namespace qt_plugin {

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaMethod & method);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaProperty & property);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaEnum & metaenum);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaClassInfo & metaclassInfo);

WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE(QMetaObject)
WRITE_PRETTY_HTML_IMPL_DECLARE(QObject)
WRITE_PRETTY_HTML_IMPL_DECLARE(QWidget)
WRITE_PRETTY_HTML_IMPL_DECLARE(QDialog)
WRITE_PRETTY_HTML_IMPL_DECLARE(QThread)
WRITE_PRETTY_HTML_IMPL_DECLARE(QGLWidget)

WRITE_PRETTY_HTML_IMPL_DECLARE(QPaintDevice)
WRITE_PRETTY_HTML_IMPL_DECLARE(QImage)
WRITE_PRETTY_HTML_IMPL_DECLARE(QIcon)
} // namespace qt_plugin

} // namespace sgi
