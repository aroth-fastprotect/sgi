#include "stdafx.h"
#include <ostream>
#include <QThread>
#include <QDialog>
#include <QGLWidget>
#include <QMetaProperty>
#include "writeHTMLQt.h"
#include "SGIItemQt"

#include <sgi/helpers/qt>
#include <sgi/helpers/html>
#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {
WRITE_PRETTY_HTML_IMPL_REGISTER(QObject)
WRITE_PRETTY_HTML_IMPL_REGISTER(QWidget)
WRITE_PRETTY_HTML_IMPL_REGISTER(QDialog)
WRITE_PRETTY_HTML_IMPL_REGISTER(QThread)
WRITE_PRETTY_HTML_IMPL_REGISTER(QGLWidget)

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaMethod & method);

bool writePrettyHTMLImpl<QObject>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QObject * object = getObject<QObject, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>name</td><td>" << object->objectName() << "</td></tr>" << std::endl;
            const QMetaObject * metaObject = object->metaObject();
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
            os << "<tr><td>className</td><td>" << helpers::html_encode(metaObject?metaObject->className():"(null)") << "</td></tr>" << std::endl;
            // do not show the full meta object here, because we actually also have
            // the values for the properties and the list becomes quite long here.
            // if the user is interested in the meta object he should call writePrettyHTML
            // for the metaObject explicitly.
            os << "<tr><td>meta object</td><td>" << (void*)metaObject << "</td></tr>" << std::endl;

            while(metaObject)
            {
                int propertyOffset = metaObject->propertyOffset();
                int propertyCount = metaObject->propertyCount();
                for (int i=propertyOffset; i<propertyCount; ++i)
                {
                    QMetaProperty metaproperty = metaObject->property(i);
                    const char *name = metaproperty.name();
                    const char *typeName = metaproperty.typeName();
                    QVariant value = object->property(name);

                    os << "<tr><td>" << metaObject->className() << "::" << name << "(" << typeName << ")</td><td>" << value << "</td></tr>" << std::endl;
                }
                metaObject = metaObject->superClass();
            }

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            os << "<ul>";
            const QObjectList & children = object->children ();
            for(QObjectList::const_iterator it = children.begin(); it != children.end(); it++)
            {
                QObject * child = *it;
                os << "<li>" << qt_helpers::getObjectNameAndType(child) << "</li>" << std::endl;
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeMethods:
        {
            os << "<ol>";
            const QMetaObject * metaObject = object->metaObject();
            while(metaObject)
            {
                int methodOffset = metaObject->methodOffset();
                int methodCount = metaObject->methodCount();
                for (int i=methodOffset; i<methodCount; ++i)
                {
                    QMetaMethod method = metaObject->method(i);
                    os << "<li>" << method << "</li>" << std::endl;
                }
                metaObject = metaObject->superClass();
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QWidget>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QWidget * object = getObject<QWidget, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            // add QWidget properties

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

bool writePrettyHTMLImpl<QThread>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QThread * object = getObject<QThread, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            // add QThread properties
            os << "<tr><td>priority</td><td>" << object->priority() << "</td></tr>" << std::endl;
            os << "<tr><td>stackSize</td><td>" << object->stackSize() << "</td></tr>" << std::endl;
            os << "<tr><td>isRunning</td><td>" << (object->isRunning()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isFinished</td><td>" << (object->isFinished()?"true":"false") << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QGLFormat::OpenGLContextProfile t)
{
    switch(t)
    {
    case QGLFormat::OpenGL_Version_None: os << "None"; break;
    case QGLFormat::OpenGL_Version_1_1: os << "OGL1.1"; break;
    case QGLFormat::OpenGL_Version_1_2: os << "OGL1.2"; break;
    case QGLFormat::OpenGL_Version_1_3: os << "OGL1.3"; break;
    case QGLFormat::OpenGL_Version_1_4: os << "OGL1.4"; break;
    case QGLFormat::OpenGL_Version_1_5: os << "OGL1.5"; break;
    case QGLFormat::OpenGL_Version_2_0: os << "OGL2.0"; break;
    case QGLFormat::OpenGL_Version_2_1: os << "OGL2.1"; break;
    case QGLFormat::OpenGL_Version_3_0: os << "OGL3.0"; break;
    case QGLFormat::OpenGL_Version_3_1: os << "OGL3.1"; break;
    case QGLFormat::OpenGL_Version_3_2: os << "OGL3.2"; break;
    case QGLFormat::OpenGL_Version_3_3: os << "OGL3.3"; break;
    case QGLFormat::OpenGL_Version_4_0: os << "OGL4.0"; break;
    case QGLFormat::OpenGL_ES_Common_Version_1_0: os << "OGLES1.0"; break;
    case QGLFormat::OpenGL_ES_Common_Version_1_1: os << "OGLES1.1"; break;
    case QGLFormat::OpenGL_ES_CommonLite_Version_1_0: os << "OGLESLT1.0"; break;
    case QGLFormat::OpenGL_ES_CommonLite_Version_1_1: os << "OGLESLT1.1"; break;
    case QGLFormat::OpenGL_ES_Version_2_0: os << "OGLES2.0"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QGLFormat & format)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>depthBufferSize</td><td>" << format.depthBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>accumBufferSize</td><td>" << format.accumBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>redBufferSize</td><td>" << format.redBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>greenBufferSize</td><td>" << format.greenBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>blueBufferSize</td><td>" << format.blueBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>alphaBufferSize</td><td>" << format.alphaBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>stencilBufferSize</td><td>" << format.stencilBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>sampleBuffers</td><td>" << (format.sampleBuffers()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>samples</td><td>" << format.samples() << "</td></tr>" << std::endl;
    os << "<tr><td>swapInterval</td><td>" << format.swapInterval() << "</td></tr>" << std::endl;
    os << "<tr><td>doubleBuffer</td><td>" << (format.doubleBuffer()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>depth</td><td>" << (format.depth()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>rgba</td><td>" << (format.rgba()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>alpha</td><td>" << (format.alpha()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>accum</td><td>" << (format.accum()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>stencil</td><td>" << (format.stencil()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>stereo</td><td>" << (format.stereo()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>directRendering</td><td>" << (format.directRendering()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>hasOverlay</td><td>" << (format.hasOverlay()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>plane</td><td>" << format.plane() << "</td></tr>" << std::endl;
    os << "<tr><td>version</td><td>" << format.majorVersion() << "." << format.minorVersion() << "</td></tr>" << std::endl;
    os << "<tr><td>profile</td><td>" << format.profile() << "</td></tr>" << std::endl;
    os << "</table>" << std::endl;
    return os;
}

bool writePrettyHTMLImpl<QGLWidget>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QGLWidget * object = getObject<QGLWidget, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QWidget properties first
            callNextHandler(os);

            // add QGLWidget properties

            os << "<tr><td>isValid</td><td>" << (object->isValid()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isSharing</td><td>" << (object->isSharing()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>doubleBuffer</td><td>" << (object->doubleBuffer()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
            os << "<tr><td>context</td><td>" << (void*)object->context() << "</td></tr>" << std::endl;
            os << "<tr><td>overlayContext</td><td>" << (void*)object->overlayContext() << "</td></tr>" << std::endl;
            os << "<tr><td>colormap</td><td>" << (void*)&object->colormap() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<QDialog>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QDialog * object = getObject<QDialog, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QWidget properties first
            callNextHandler(os);

            // add QDialog properties
            os << "<tr><td>result</td><td>" << object->result() << "</td></tr>" << std::endl;
            os << "<tr><td>isModal</td><td>" << (object->isModal()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>orientation</td><td>" << object->orientation() << "</td></tr>" << std::endl;
            os << "<tr><td>sizeHint</td><td>" << object->sizeHint() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumSizeHint</td><td>" << object->minimumSizeHint() << "</td></tr>" << std::endl;
            os << "<tr><td>isSizeGripEnabled</td><td>" << (object->isSizeGripEnabled()?"true":"false") << "</td></tr>" << std::endl;

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
} // namespace qt_plugin
} // namespace sgi
