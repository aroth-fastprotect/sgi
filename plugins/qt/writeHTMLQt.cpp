#include "stdafx.h"
#include <ostream>
#include <QThread>
#include <QApplication>
#include <QProcessEnvironment>
#include <QDialog>
#ifdef WITH_QTOPENGL
#include <QGLWidget>
#endif
#include <QOpenGLWindow>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QSurfaceFormat>
#include <QWindow>
#include <QSurface>
#include <QScreen>
#include <QStyle>
#include <QMetaProperty>
#include <QFormLayout>

#include "writeHTMLQt.h"
#include "SGIItemQt"

#include <sgi/helpers/qt>
#include <sgi/helpers/qt_widgetwindow>
#include <sgi/helpers/html>
#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QWidget)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QWindow)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QWidgetWindow)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QSurface)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QDialog)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QThread)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QCoreApplication)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QApplication)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QOpenGLContext)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QOpenGLWidget)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QOpenGLShaderProgram)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QOpenGLShader)
#ifdef WITH_QTOPENGL
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QGLWidget)
#endif

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QGridLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(QFormLayout)

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaMethod & method);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaProperty & property);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaEnum & metaenum);
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMetaClassInfo & metaclassInfo);
extern void writePrettyHTMLImpl_QMetaMethod(std::basic_ostream<char>& os, const QMetaMethod & method);

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat::SwapBehavior t)
{
    switch(t)
    {
    case QSurfaceFormat::DefaultSwapBehavior: os << "default"; break;
    case QSurfaceFormat::SingleBuffer: os << "single buffer"; break;
    case QSurfaceFormat::DoubleBuffer: os << "double buffer"; break;
    case QSurfaceFormat::TripleBuffer: os << "tripple"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat::RenderableType t)
{
    switch(t)
    {
    case QSurfaceFormat::DefaultRenderableType: os << "default"; break;
    case QSurfaceFormat::OpenGL: os << "OpenGL"; break;
    case QSurfaceFormat::OpenGLES: os << "OpenGLES"; break;
    case QSurfaceFormat::OpenVG: os << "OpenVG"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat::FormatOptions & o)
{
    if(o.testFlag(QSurfaceFormat::StereoBuffers))
        os << "StereoBuffers";
    if(o.testFlag(QSurfaceFormat::DebugContext))
        os << "DebugContext";
    if(o.testFlag(QSurfaceFormat::DeprecatedFunctions))
        os << "DeprecatedFunctions";
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    if(o.testFlag(QSurfaceFormat::ResetNotification))
        os << "ResetNotification";
#endif

    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat::OpenGLContextProfile & p)
{
    switch(p)
    {
    case QSurfaceFormat::NoProfile: os << "NoProfile"; break;
    case QSurfaceFormat::CoreProfile: os << "CoreProfile"; break;
    case QSurfaceFormat::CompatibilityProfile: os << "CompatibilityProfile"; break;
    default: os << (int)p; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurfaceFormat & format)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
    os << "<tr><td>depthBufferSize</td><td>" << format.depthBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>redBufferSize</td><td>" << format.redBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>greenBufferSize</td><td>" << format.greenBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>blueBufferSize</td><td>" << format.blueBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>alphaBufferSize</td><td>" << format.alphaBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>stencilBufferSize</td><td>" << format.stencilBufferSize() << "</td></tr>" << std::endl;
    os << "<tr><td>samples</td><td>" << format.samples() << "</td></tr>" << std::endl;
    os << "<tr><td>swapInterval</td><td>" << format.swapInterval() << "</td></tr>" << std::endl;
    os << "<tr><td>swapBehavior</td><td>" << format.swapBehavior() << "</td></tr>" << std::endl;
    os << "<tr><td>alpha</td><td>" << (format.hasAlpha()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>renderableType</td><td>" << format.renderableType() << "</td></tr>" << std::endl;
    os << "<tr><td>options</td><td>" << format.options() << "</td></tr>" << std::endl;
    os << "<tr><td>stereo</td><td>" << (format.stereo()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>version</td><td>" << format.majorVersion() << "." << format.minorVersion() << "</td></tr>" << std::endl;
    os << "<tr><td>profile</td><td>" << format.profile() << "</td></tr>" << std::endl;
    os << "</table>" << std::endl;
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurface::SurfaceClass t)
{
    switch (t)
    {
    case QSurface::Window: os << "Window"; break;
    case QSurface::Offscreen: os << "Offscreen"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSurface::SurfaceType t)
{
    switch (t)
    {
    case QSurface::RasterSurface: os << "RasterSurface"; break;
    case QSurface::OpenGLSurface: os << "OpenGLSurface"; break;
    case QSurface::RasterGLSurface: os << "RasterGLSurface"; break;
    case QSurface::OpenVGSurface: os << "OpenVGSurface"; break;
    default: os << (int)t; break;
    }
    return os;
}

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

            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>name</td><td>" << object->objectName() << "</td></tr>" << std::endl;
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
            const QMetaObject * metaObject = object->metaObject();
            os << "<tr><td>className</td><td>" << helpers::html_encode(metaObject?metaObject->className():"(null)") << "</td></tr>" << std::endl;
            // do not show the full meta object here, because we actually also have
            // the values for the properties and the list becomes quite long here.
            // if the user is interested in the meta object he should call writePrettyHTML
            // for the metaObject explicitly.
            os << "<tr><td>meta object</td><td>0x" << (void*)metaObject << "</td></tr>" << std::endl;

            while(metaObject)
            {
                int propertyOffset = metaObject->propertyOffset();
                int propertyCount = metaObject->propertyCount();
                std::vector<const char*> properties(propertyCount - propertyOffset);
                for (int i = propertyOffset; i < propertyCount; ++i)
                {
                    QMetaProperty metaproperty = metaObject->property(i);
                    properties[i - propertyOffset] = metaproperty.name();
                }
                std::sort(properties.begin(), properties.end(), [](char const *lhs, char const *rhs) { return qstricmp(lhs, rhs) < -1; });
                for (const char * name : properties)
                {
                    int index = metaObject->indexOfProperty(name);
                    QMetaProperty metaproperty = metaObject->property(index);
                    const char *typeName = metaproperty.typeName();
                    QVariant value = object->property(name);

                    if(value.type() == QVariant::Palette)
                        value = QString("palette:%1").arg(value.value<QPalette>().cacheKey());

                    os << "<tr><td>" << metaObject->className() << "::" << name << "(" << typeName << ")</td><td>";
                    writeVariant(os, value, &metaproperty); 
                    os << "</td></tr>" << std::endl;
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
    case SGIItemTypeProperties:
        {
            os << "<table border=\'1\' align=\'left\'><tr><th>Property</th><th>Value</th></tr>" << std::endl;

            const QMetaObject * metaObject = object->metaObject();
            while (metaObject)
            {
                int propertyOffset = metaObject->propertyOffset();
                int propertyCount = metaObject->propertyCount();
                for (int i = propertyOffset; i < propertyCount; ++i)
                {
                    QMetaProperty metaproperty = metaObject->property(i);
                    const char *name = metaproperty.name();
                    const char *typeName = metaproperty.typeName();
                    QVariant value = object->property(name);

                    if(value.type() == QVariant::Palette)
                        value = QString("palette:%1").arg(value.value<QPalette>().cacheKey());

                    os << "<tr><td>" << metaObject->className() << "::" << name << "(" << typeName << ")</td><td>";
                    writeVariant(os, value, &metaproperty);
                    os << "</td></tr>" << std::endl;
                }
                metaObject = metaObject->superClass();
            }

            os << "</table>" << std::endl;

            ret = true;
        }
        break;
    case SGIItemTypeMethods:
        {
            if (_item->number() == ~0u)
            {
                os << "<ol>";
                const QMetaObject * metaObject = object->metaObject();
                while (metaObject)
                {
                    int methodOffset = metaObject->methodOffset();
                    int methodCount = metaObject->methodCount();
                    for (int i = methodOffset; i < methodCount; ++i)
                    {
                        QMetaMethod method = metaObject->method(i);
                        os << "<li>" << method << "</li>" << std::endl;
                    }
                    metaObject = metaObject->superClass();
                }
                os << "</ol>" << std::endl;
            }
            else
            {
                const QMetaObject * metaObject = object->metaObject();
                bool found = false;
                while (metaObject && !found)
                {
                    int methodOffset = metaObject->methodOffset();
                    int methodCount = metaObject->methodCount();
                    for (int i = methodOffset; !found && i < methodCount; ++i)
                    {
                        if (i == _item->number())
                        {
                            QMetaMethod method = metaObject->method(i);
                            writePrettyHTMLImpl_QMetaMethod(os, method);
                        }
                    }
                    metaObject = metaObject->superClass();
                }
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

extern void writePrettyHTMLImpl_QPaintDevice_process(std::basic_ostream<char>& os, QPaintDevice* object);

bool writePrettyHTMLImpl<QWidget>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
	QWidget * object = getObjectMulti<QWidget, SGIItemQt, SGIItemQtPaintDevice>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            writePrettyHTMLImpl_QPaintDevice_process(os, dynamic_cast<QPaintDevice*>(object));

            // add QWidget properties
            // DO NOT call winId() here, because this would trigger the widget to change
            // into a native window.
            //os << "<tr><td>winId</td><td>" << object->winId() << "</td></tr>" << std::endl;
            os << "<tr><td>internalWinId</td><td>" << object->internalWinId() << "</td></tr>" << std::endl;
            os << "<tr><td>effectiveWinId</td><td>" << object->effectiveWinId() << "</td></tr>" << std::endl;

            os << "<tr><td>windowHandle</td><td>" << qt_helpers::getObjectNameAndType(object->windowHandle()) << "</td></tr>" << std::endl;
            os << "<tr><td>style</td><td>" << qt_helpers::getObjectNameAndType(object->style()) << "</td></tr>" << std::endl;
            os << "<tr><td>isTopLevel</td><td>" << (object->isTopLevel()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isWindow</td><td>" << (object->isWindow()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePalette:
        {
            os << object->palette();
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<QWindow>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QWindow * object = getObjectMulti<QWindow, SGIItemQt, SGIItemQtSurface>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            // add QWindow properties

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
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QWidgetWindow>::process(std::basic_ostream<char>& os)
{
	bool ret = false;
	QWidgetWindow * object = getObjectMulti<QWidgetWindow, SGIItemQt, SGIItemQtSurface>();
	switch (itemType())
	{
	case SGIItemTypeObject:
		{
			if (_table)
				os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

			// add QWindow properties first
			callNextHandler(os);

			// add properties
			os << "<tr><td>widget</td><td>" << qt_helpers::getObjectNameAndType((const QObject*)object->widget(), true) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<QSurface>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QSurface * object = getObject<QSurface, SGIItemQtSurface>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QSurface properties
            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>surfaceClass</td><td>" << object->surfaceClass() << "</td></tr>" << std::endl;
            os << "<tr><td>surfaceType</td><td>" << object->surfaceType() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsOpenGL</td><td>" << (object->supportsOpenGL() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;

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


bool writePrettyHTMLImpl<QCoreApplication>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QCoreApplication * object = getObject<QCoreApplication, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);


            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeSystemEnvironment:
        {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            if(env.isEmpty())
                os << "<i>empty</i>";
            else
            {
                const QStringList path_env_vars = QStringList() 
                    << "PATH" 
#ifdef _WIN32
                    << "PATHEXT"
#else
                    << "LD_LIBRARY_PATH" 
#endif
                    << "PYTHONPATH" << "INCLUDE" << "LIB";
                os << "<ul>";
                QStringList keys = env.keys();
                keys.sort();
                for(const auto & key : keys)
                {
                    if (key.isEmpty())
                        continue;
                    QString value = env.value(key);
#ifdef _WIN32
                    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
#else
                    Qt::CaseSensitivity cs = Qt::CaseSensitive;
#endif
                    if (path_env_vars.contains(key, cs))
                    {
                        os << "<li>" << key << "<ol>";
#ifdef _WIN32
                        const QChar path_split_char = ';';
#else
                        const QChar path_split_char = ':';
#endif
                        for (const QString & dir : value.split(path_split_char))
                        {
                            if (dir.isEmpty())
                                continue;
                            os << "<li>" << dir << "</li>";
                        }
                        os << "</ol></li>";
                    }
                    else
                    {
                        os << "<li>" << key << "=" << value << "</li>";
                    }
                }
                os << "</ul>";
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


bool writePrettyHTMLImpl<QApplication>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QApplication * object = getObject<QApplication, SGIItemQt>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);


            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePalette:
        {
            os << object->palette(static_cast<const char*>(nullptr));
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#ifdef WITH_QTOPENGL
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QGLFormat::OpenGLContextProfile t)
{
    switch (t)
    {
        case QGLFormat::NoProfile: os << "NoProfile"; break;
        case QGLFormat::CoreProfile: os << "CoreProfile"; break;
        case QGLFormat::CompatibilityProfile: os << "CompatibilityProfile"; break;
        default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QGLFormat::OpenGLVersionFlag t)
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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QGLContext * object)
{
    if(object)
    {
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
        os << "<tr><td>isValid</td><td>" << (object->isValid()?"true":"false") << "</td></tr>" << std::endl;
        os << "<tr><td>isSharing</td><td>" << (object->isSharing()?"true":"false") << "</td></tr>" << std::endl;

        os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
        os << "<tr><td>device</td><td>" << qt_helpers::getObjectNameAndType(object->device(), true) << "</td></tr>" << std::endl;
        //os << "<tr><td>functions</td><td>" << (void*)object->functions() << "</td></tr>" << std::endl;
        os << "</table>" << std::endl;
    }
    else
        os << "(null)";
    return os;
}
#endif // WITH_QTOPENGL

bool writePrettyHTMLImpl<QOpenGLContext>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QOpenGLContext * object = getObject<QOpenGLContext, SGIItemQt>();
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
            os << "<tr><td>isOpenGLES</td><td>" << (object->isOpenGLES()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
            os << "<tr><td>screen</td><td>" << qt_helpers::getObjectNameAndType(object->screen(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>nativeHandle</td><td>" << object->nativeHandle() << "</td></tr>" << std::endl;
            os << "<tr><td>defaultFramebufferObject</td><td>" << object->defaultFramebufferObject() << "</td></tr>" << std::endl;
            os << "<tr><td>shareContext</td><td>" << qt_helpers::getObjectNameAndType(object->shareContext(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>shareGroup</td><td>" << qt_helpers::getObjectNameAndType(object->shareGroup(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>surface</td><td>" << (void*)object->surface() << "</td></tr>" << std::endl;
            os << "<tr><td>handle</td><td>" << object->handle() << "</td></tr>" << std::endl;
            os << "<tr><td>shareHandle</td><td>" << object->shareHandle() << "</td></tr>" << std::endl;
            //os << "<tr><td>functions</td><td>" << (void*)object->functions() << "</td></tr>" << std::endl;
            os << "<tr><td>numExtensions</td><td>" << object->extensions().size() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeContextExtensions:
        {
            QSet<QByteArray> extensions = object->extensions();
            os << "Total number of extensions: " << extensions.size() << "<br/>" << std::endl;

            QStringList amd_ati_extensions;
            QStringList arb_extensions;
            QStringList ext_extensions;
            QStringList intel_extensions;
            QStringList nvidia_extensions;
            QStringList s3_extensions;
            QStringList sgi_extensions;
            QStringList other_extensions;
            for(QSet<QByteArray>::const_iterator it = extensions.begin(); it != extensions.end(); it++)
            {
                const QByteArray & extension = *it;
                if(extension.startsWith("GL_AMD") || extension.startsWith("GL_ATI"))
                    amd_ati_extensions.push_back(extension);
                else if(extension.startsWith("GL_ARB"))
                    arb_extensions.push_back(extension);
                else if(extension.startsWith("GL_EXT"))
                    ext_extensions.push_back(extension);
                else if(extension.startsWith("GL_INTEL"))
                    intel_extensions.push_back(extension);
                else if(extension.startsWith("GL_NV"))
                    nvidia_extensions.push_back(extension);
                else if(extension.startsWith("GL_S3"))
                    s3_extensions.push_back(extension);
                else if(extension.startsWith("GL_SGI"))
                    sgi_extensions.push_back(extension);
                else
                    other_extensions.push_back(extension);
            }
            arb_extensions.sort(Qt::CaseInsensitive);
            os << "ARB extensions: " << arb_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = arb_extensions.begin(); it != arb_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            ext_extensions.sort(Qt::CaseInsensitive);
            os << "EXT extensions: " << ext_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = ext_extensions.begin(); it != ext_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            amd_ati_extensions.sort(Qt::CaseInsensitive);
            os << "AMD/ATI extensions: " << amd_ati_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = amd_ati_extensions.begin(); it != amd_ati_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            nvidia_extensions.sort(Qt::CaseInsensitive);
            os << "NVIDIA extensions: " << nvidia_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = nvidia_extensions.begin(); it != nvidia_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            intel_extensions.sort(Qt::CaseInsensitive);
            os << "Intel extensions: " << intel_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = intel_extensions.begin(); it != intel_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            s3_extensions.sort(Qt::CaseInsensitive);
            os << "S3 extensions: " << s3_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = s3_extensions.begin(); it != s3_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            sgi_extensions.sort(Qt::CaseInsensitive);
            os << "SGI extensions: " << sgi_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = sgi_extensions.begin(); it != sgi_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
            other_extensions.sort(Qt::CaseInsensitive);
            os << "Other extensions: " << other_extensions.size() << "<ol>" << std::endl;
            for(QStringList::const_iterator it = other_extensions.begin(); it != other_extensions.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ol>" << std::endl;
        }
        break;
    case SGIItemTypeSurface:
        {
            QSurface * surface = object->surface();
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>surfaceClass</td><td>" << surface->surfaceClass() << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << surface->format() << "</td></tr>" << std::endl;
            os << "<tr><td>surfaceHandle</td><td>" << (void*)surface->surfaceHandle() << "</td></tr>" << std::endl;
            os << "<tr><td>surfaceType</td><td>" << surface->surfaceType() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsOpenGL</td><td>" << (surface->supportsOpenGL() ? "true" : "false") << "</td></tr>" << std::endl;
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

bool writePrettyHTMLImpl<QOpenGLWidget>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QOpenGLWidget * object = getObject<QOpenGLWidget, SGIItemQt>();
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

            os << "<tr><td>format</td><td>" << object->format() << "</td></tr>" << std::endl;
            os << "<tr><td>context</td><td>" << qt_helpers::getObjectNameAndType(object->context(), true) << "</td></tr>" << std::endl;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            os << "<tr><td>updateBehavior</td><td>" << object->updateBehavior() << "</td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeContext:
        {
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QOpenGLShaderProgram>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QOpenGLShaderProgram * object = getObject<QOpenGLShaderProgram, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            os << "<tr><td>isLinked</td><td>" << (object->isLinked()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>programId</td><td>" << object->programId() << "</td></tr>" << std::endl;
            os << "<tr><td>maxGeometryOutputVertices</td><td>" << object->maxGeometryOutputVertices() << "</td></tr>" << std::endl;
            os << "<tr><td>patchVertexCount</td><td>" << object->patchVertexCount() << "</td></tr>" << std::endl;
            QList<QOpenGLShader *> shaders = object->shaders();
            os << "<tr><td>shaders</td><td>" << shaders.size() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaderProgramShaders:
        {
            QList<QOpenGLShader *> shaders = object->shaders();
            os << "<ol>";
            for(auto shader : shaders)
            {
                os << "<li>" << qt_helpers::getObjectNameAndType(shader, true) << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<QOpenGLShader>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QOpenGLShader * object = getObject<QOpenGLShader, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QObject properties first
            callNextHandler(os);

            os << "<tr><td>isCompiled</td><td>" << (object->isCompiled()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>id</td><td>" << object->shaderId() << "</td></tr>" << std::endl;
            os << "<tr><td>type</td><td>" << object->shaderType() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaderLog:
        {
            os << "<pre>" << helpers::html_encode(object->log().toStdString()) << "</pre>";
            ret = true;
        }
        break;
    case SGIItemTypeShaderSourceCode:
        {
            os << "<pre>" << helpers::html_encode(object->sourceCode().toStdString()) << "</pre>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#ifdef WITH_QTOPENGL
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
            os << "<tr><td>context</td><td>" << object->context() << "</td></tr>" << std::endl;
            os << "<tr><td>overlayContext</td><td>" << object->overlayContext() << "</td></tr>" << std::endl;
            os << "<tr><td>colormap</td><td>" << (void*)&object->colormap() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeContext:
        {
            os << object->context();
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
#endif // WITH_QTOPENGL

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


bool writePrettyHTMLImpl<QLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QLayout* object = getObject<QLayout, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QWidget properties first
            callNextHandler(os);

            // add QLayout properties
            os << "<tr><td>geometry</td><td>" << object->geometry() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumSize</td><td>" << object->minimumSize() << "</td></tr>" << std::endl;
            os << "<tr><td>maximumSize</td><td>" << object->maximumSize() << "</td></tr>" << std::endl;
            os << "<tr><td>items</td><td><ol>";
            for(int n = 0; n < object->count(); ++n)
            {
                const auto * item = object->itemAt(n);
                os << "<li>" << qt_helpers::getObjectNameAndType(item) << "</li>";
            }

            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeLayoutItem:
        {
            if(itemNumber() != ~0u)
            {
                const auto* item = object->itemAt(itemNumber());
                os << *item;
            }
            else
            {
                os << "<ol>";
                for (int n = 0; n < object->count(); ++n)
                {
                    const auto* item = object->itemAt(n);
                    os << "<li>" << qt_helpers::getObjectNameAndType(item) << "</li>";
                }
                os << "</ol>" << std::endl;

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

bool writePrettyHTMLImpl<QGridLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QGridLayout* object = getObject<QGridLayout, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QWidget properties first
            callNextHandler(os);

            // add QGridLayout properties
            os << "<tr><td>columnCount</td><td>" << object->columnCount() << "</td></tr>" << std::endl;
            os << "<tr><td>rowCount</td><td>" << object->rowCount() << "</td></tr>" << std::endl;
            os << "<tr><td>originCorner</td><td>" << object->originCorner() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeLayoutItem:
        {
            if(itemNumber() != ~0u)
            {
                const auto* item = object->itemAt(itemNumber());
                os << *item;
            }
            else
            {
                struct GridItem {
                    int row;
                    int column;
                    int rowspan;
                    int colspan;
                };
                std::vector<struct GridItem> items;
                for(int i = 0; i < object->count(); ++i)
                {
                    GridItem item;
                    object->getItemPosition(i, &item.row, &item.column, &item.rowspan, &item.colspan);
                    items.push_back(item);
                }
                struct GridItemSort
                {
                    inline bool operator() (const GridItem& lhs, const GridItem& rhs)
                    {
                        if(lhs.row < rhs.row)
                            return true;
                        else if(lhs.row == rhs.row)
                            return (lhs.column < rhs.column);
                        else
                            return false;
                    }
                };
                std::sort(items.begin(), items.end(), GridItemSort());

                auto it = items.begin();
                const GridItem * currentItem = items.empty() ? nullptr : &*it;
                const GridItem* lastItem = nullptr;
                os << "<table border=\'1\' align=\'left\'>";
                for (int row = 0; row < object->rowCount(); ++row)
                {
                    if(currentItem && currentItem->row == row && lastItem != currentItem)
                    {
                        if(lastItem)
                            os << "</tr>";
                        os << "<tr rowspan=\"" << currentItem->rowspan << "\">";
                        lastItem = currentItem;
                    }
                    for (int col = 0; col < object->columnCount(); ++col)
                    {
                        if(currentItem && currentItem->row == row && currentItem->column == col)
                        {
                            os << "<td colspan=\"" << currentItem->colspan << "\">";
                            os << col << ',' << row;
                            if(currentItem->colspan > 1 || currentItem->rowspan > 1)
                            {
                                os << '(' << currentItem->colspan << '/' << currentItem->rowspan << ')';
                            }
                            if (it != items.end())
                            {
                                ++it;
                                currentItem = (it == items.end()) ? nullptr : &*it;
                            }
                            os << "</td>";
                        }
                    }
                    os << "</tr>";
                }
                if(lastItem)
                    os << "</tr>";
                os << "</table>" << std::endl;
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

bool writePrettyHTMLImpl<QFormLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    QFormLayout* object = getObject<QFormLayout, SGIItemQt>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add QLayout properties first
            callNextHandler(os);

            // add QFormLayout properties
            os << "<tr><td>rowCount</td><td>" << object->rowCount() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeLayoutItem:
        {
            if(itemNumber() != ~0u)
            {
                const auto* item = object->itemAt(itemNumber());
                os << *item;
            }
            else
            {
                os << "<table border=\'1\' align=\'left\'><tr><th>Label</th><th>Field</th><th>Span</th></tr>" << std::endl;
                for (int row = 0; row < object->rowCount(); ++row)
                {
                    os << "<tr>";
                    os << "<td>" << qt_helpers::getObjectNameAndType(object->itemAt(row, QFormLayout::LabelRole)) << "</td>";
                    os << "<td>" << qt_helpers::getObjectNameAndType(object->itemAt(row, QFormLayout::FieldRole)) << "</td>";
                    os << "<td>" << qt_helpers::getObjectNameAndType(object->itemAt(row, QFormLayout::SpanningRole)) << "</td>";
                    os << "</tr>";
                }
                os << "</table>" << std::endl;
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

} // namespace qt_plugin
} // namespace sgi
