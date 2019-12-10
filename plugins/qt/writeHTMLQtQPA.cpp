#include "stdafx.h"
#include <ostream>
#include <QtGui/QImage>
#include <QtGui/QPicture>
#include "writeHTMLQt.h"
#include "SGIItemQt"

#include <sgi/helpers/rtti>
#include <sgi/helpers/qt>

#include <qpa/qplatformwindow.h>
#include <qpa/qplatformscreen.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QPlatformSurface)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QPlatformWindow)

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat& format);

bool writePrettyHTMLImpl<QPlatformSurface>::process(std::basic_ostream<char>& os)
{
	bool ret = false;
	QPlatformSurface* object = getObject<QPlatformSurface, SGIItemQtPlatformSurface>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
			os << "<tr><td>surface</td><td>" << helpers::getRTTIObjectNameAndType(object->surface()) << "</td></tr>" << std::endl;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
			os << "<tr><td>screen</td><td>" << helpers::getRTTIObjectNameAndType(object->screen()) << "</td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;

			ret = true;
        }
        break;
    case SGIItemTypeSurfaceFormat:
        {
            os << object->format();
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QPlatformWindow>::process(std::basic_ostream<char>& os)
{
	QPlatformWindow* object = getObject<QPlatformWindow, SGIItemQtPlatformSurface>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

			os << "<tr><td>geometry</td><td>" << object->geometry() << "</td></tr>" << std::endl;
			os << "<tr><td>normalGeometry</td><td>" << object->normalGeometry() << "</td></tr>" << std::endl;
			os << "<tr><td>frameMargins</td><td>" << object->frameMargins() << "</td></tr>" << std::endl;
			os << "<tr><td>safeAreaMargins</td><td>" << object->safeAreaMargins() << "</td></tr>" << std::endl;

			os << "<tr><td>isExposed</td><td>" << (object->isExposed() ? "true" : "false") << "</td></tr>" << std::endl;
			os << "<tr><td>isActive</td><td>" << (object->isActive() ? "true" : "false") << "</td></tr>" << std::endl;
			os << "<tr><td>isEmbedded</td><td>" << (object->isEmbedded() ? "true" : "false") << "</td></tr>" << std::endl;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
			os << "<tr><td>hasPendingUpdateRequest</td><td>" << (object->hasPendingUpdateRequest() ? "true" : "false") << "</td></tr>" << std::endl;
#endif

			os << "<tr><td>winId</td><td>" << object->winId() << "</td></tr>" << std::endl;
			os << "<tr><td>devicePixelRatio</td><td>" << object->devicePixelRatio() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;

            ret = true;
        }
        break;
    default:
        // add node properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

} // namespace qt_plugin
} // namespace sgi
