#include "stdafx.h"
#include <ostream>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include "writeHTMLQtQuick.h"
#include "SGIItemQtQuick"

#include <sgi/plugins/SGIItemQt>

#include <sgi/helpers/qt>
#include <sgi/helpers/qt_widgetwindow>
#include <sgi/helpers/html>
#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qtquick_plugin {
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QQmlContext)

bool writePrettyHTMLImpl<QQmlContext>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QQmlContext* object = getObject<QQmlContext, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            os << "<tr><td>engine</td><td>" << qt_helpers::getObjectNameAndType(object->engine()) << "</td></tr>" << std::endl;
            os << "<tr><td>parentContext</td><td>" << qt_helpers::getObjectNameAndType(object->parentContext()) << "</td></tr>" << std::endl;
            os << "<tr><td>contextObject</td><td>" << qt_helpers::getObjectNameAndType(object->contextObject()) << "</td></tr>" << std::endl;

            os << "<tr><td>isValid</td><td>" << (object->isValid() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>baseUrl</td><td>" << object->baseUrl() << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
} // namespace qtquick_plugin
} // namespace sgi
