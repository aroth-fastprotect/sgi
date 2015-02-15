#include <ostream>
#include <QtGui/QImage>
#include <QtGui/QPicture>
#include "writeHTMLQt.h"
#include "SGIItemQt.h"

#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(QPaintDevice)
WRITE_PRETTY_HTML_IMPL_REGISTER(QImage)

bool writePrettyHTMLImpl<QPaintDevice>::process(std::basic_ostream<char>& os)
{
    QPaintDevice * object = getObject<QPaintDevice, SGIItemQtPaintDevice>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;

            os << "<tr><td>paintingActive</td><td>" << (object->paintingActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>devType</td><td>" << object->devType() << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << object->width() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->height() << "</td></tr>" << std::endl;
            os << "<tr><td>widthMM</td><td>" << object->widthMM() << "</td></tr>" << std::endl;
            os << "<tr><td>heightMM</td><td>" << object->heightMM() << "</td></tr>" << std::endl;
            os << "<tr><td>logicalDpiX</td><td>" << object->logicalDpiX() << "</td></tr>" << std::endl;
            os << "<tr><td>logicalDpiY</td><td>" << object->logicalDpiY() << "</td></tr>" << std::endl;
            os << "<tr><td>physicalDpiX</td><td>" << object->physicalDpiX() << "</td></tr>" << std::endl;
            os << "<tr><td>physicalDpiY</td><td>" << object->physicalDpiY() << "</td></tr>" << std::endl;
            os << "<tr><td>colorCount</td><td>" << object->colorCount() << "</td></tr>" << std::endl;
            os << "<tr><td>depth</td><td>" << object->depth() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<QImage>::process(std::basic_ostream<char>& os)
{
    QImage * object = getObject<QImage, SGIItemQtPaintDevice>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

            os << "<tr><td>isNull</td><td>" << (object->isNull()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isDetached</td><td>" << (object->isDetached()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
            os << "<tr><td>bitPlaneCount</td><td>" << object->bitPlaneCount() << "</td></tr>" << std::endl;
            os << "<tr><td>allGray</td><td>" << (object->allGray()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isGrayscale</td><td>" << (object->isGrayscale()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasAlphaChannel</td><td>" << (object->hasAlphaChannel()?"true":"false") << "</td></tr>" << std::endl;
#if QT_VERSION < 0x050000
            os << "<tr><td>serialNumber</td><td>" << object->serialNumber() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>cacheKey</td><td>" << object->cacheKey() << "</td></tr>" << std::endl;

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
