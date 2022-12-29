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
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QQmlEngine)

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
	case SGIItemTypeContextProperties:
		{
			if (_item->number() == ~0u)
			{
			}
			ret = true;
		}
		break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QQmlEngine>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QQmlEngine* object = getObject<QQmlEngine, SGIItemQt>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            os << "<tr><td>baseUrl</td><td>" << object->baseUrl() << std::endl;
            os << "<tr><td>offlineStoragePath</td><td>" << object->offlineStoragePath() << std::endl;

            os << "<tr><td>importPathList</td><td><ol>";
            for (const QString& s : object->importPathList())
                os << "<li>" << s << "</li>";
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>pluginPathList</td><td><ol>";
            for (const QString& s : object->pluginPathList())
                os << "<li>" << s << "</li>";
            os << "</ol></td></tr>" << std::endl;

#if QT_CONFIG(qml_network)
            os << "<tr><td>networkAccessManager</td><td>" << qt_helpers::getObjectNameAndType((QObject*)object->networkAccessManager()) << "</td></tr>" << std::endl;
            os << "<tr><td>networkAccessManagerFactory</td><td>" << qt_helpers::getObjectNameAndType((QObject*)object->networkAccessManagerFactory()) << "</td></tr>" << std::endl;
#endif

            os << "<tr><td>urlInterceptor</td><td>" << qt_helpers::getObjectNameAndType((QObject*)object->urlInterceptor()) << "</td></tr>" << std::endl;
            os << "<tr><td>incubationController</td><td>" << qt_helpers::getObjectNameAndType((QObject*)object->incubationController()) << "</td></tr>" << std::endl;

            os << "<tr><td>outputWarningsToStandardError</td><td>" << (object->outputWarningsToStandardError() ? "true" : "false") << "</td></tr>" << std::endl;

            if (_table)
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
