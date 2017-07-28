// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include "main.h"
#include "main.moc"
#include <QApplication>
#include <QMouseEvent>
#include <QWidget>

#include <sgi/ContextMenuQt>
#include <sgi/Shutdown>
#include <sgi/AutoLoadQt>
#include <sgi/helpers/qt_widgetwindow>

#ifdef SGI_USE_GAMMARAY
#include <QLibraryInfo>
#include <QDir>
#endif

#include <QDebug>

#if defined(_DEBUG)
#if defined(_MSC_VER)
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else // defined(_MSC_VER)
#define DEBUG_NEW new
#endif // defined(_MSC_VER)
#endif // defined(_DEBUG)

#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
    namespace qt_loader {

#ifdef SGI_USE_GAMMARAY
#if GAMMARAY_USE_FROM_SYSTEM
        #define GAMMARAY_PROBE_LIBRARY_NAME "gammaray_probe" SGI_LIBRARY_SHARED_MODULE_SUFFIX
#else
        #define GAMMARAY_PROBE_LIBRARY_NAME "gammaray_probe" SGI_LIBRARY_POSTFIX SGI_LIBRARY_SHARED_MODULE_SUFFIX
#endif

namespace gammaray {
    class ProbeABI {
    public:
        static bool isDebugRelevant()
        {
        #if defined(Q_OS_MACX)
            return true;
        #elif defined(_MSC_VER)
            return true;
        #else
            return false;
        #endif
        }
        static bool isDebug()
        {
        #ifdef _DEBUG
            return true;
        #else
            return false;
        #endif
        }

        static QString id()
        {
            QStringList idParts;
            idParts.push_back(QStringLiteral("qt%1_%2").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));

        #ifdef Q_OS_WIN
            #ifdef _MSC_VER
                idParts.push_back(QStringLiteral("MSVC"));
            #endif
        #endif

        #ifdef _MSC_VER
            #if defined(_M_AMD64) || defined(_M_X64)
                idParts.push_back(QStringLiteral("x86_64"));
            #else
                idParts.push_back(QStringLiteral("x86"));
            #endif
        #else
            #if defined(__amd64__) || defined(__x86_64__)
                idParts.push_back(QStringLiteral("x86_64"));
            #else
                idParts.push_back(QStringLiteral("x86"));
            #endif
        #endif
            return idParts.join(QStringLiteral("-")).append((isDebugRelevant() && isDebug()) ?
                    QStringLiteral(SGI_LIBRARY_POSTFIX_DEBUG) : QString());
        }
        static QString probePath()
        {
            QString libdir = QLibraryInfo::location(QLibraryInfo::PrefixPath);
            return libdir + QDir::separator()
                + QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR) + QDir::separator()
                + QLatin1String(GAMMARAY_PLUGIN_VERSION) + QDir::separator()
                + id() + QDir::separator()
                + QLatin1String(GAMMARAY_PROBE_LIBRARY_NAME);
        }
    };
} // namespace gammaray
#endif

ApplicationEventFilter * ApplicationEventFilter::s_instance = NULL;

ApplicationEventFilter::ApplicationEventFilter(QCoreApplication * parent)
    : QObject(parent)
    , _inspectorContextMenuMouseButton(Qt::RightButton)
    , _inspectorContextMenuMouseModifier(Qt::ControlModifier|Qt::ShiftModifier)
{
    s_instance = this;
#ifdef SGI_USE_GAMMARAY
    qDebug() << "ApplicationEventFilter ctor" << this << "gammaray" << gammaray::ProbeABI::probePath();
#else
    qDebug() << "ApplicationEventFilter ctor" << this;
#endif

    connect(parent, &QCoreApplication::aboutToQuit, this, &ApplicationEventFilter::uninstall);
    parent->installEventFilter(this);
}

ApplicationEventFilter::~ApplicationEventFilter()
{
    qDebug() << "ApplicationEventFilter dtor" << this;
}

void ApplicationEventFilter::install()
{
    if(!s_instance)
    {
        s_instance = new ApplicationEventFilter(QCoreApplication::instance());
    }
}

void ApplicationEventFilter::uninstall()
{
    qDebug() << "ApplicationEventFilter uninstall" << this;
	if (!_contextMenu.isNull())
		delete _contextMenu;
    s_instance = NULL;
    sgi::shutdown<sgi::autoload::Qt>();
    deleteLater();
}

bool ApplicationEventFilter::contextMenu(QWidget * widget, QObject * obj, float x, float y)
{
    bool ret = false;
    if(_contextMenu.isNull())
        _contextMenu = sgi::createContextMenuQt(widget, obj, NULL);
    else
        _contextMenu->setObject(obj);
    if(widget)
    {
        _contextMenu->popup(widget, x, y);
        ret = true;
    }
    return ret;
}

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    bool ret = false;
    switch(event->type())
    {
    case QEvent::MouseButtonRelease:
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if(mouseEvent->button() == _inspectorContextMenuMouseButton &&
                (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) != 0)
            {
				bool sgi_skip_object = obj->property("sgi_skip_object").toBool();
				if (!sgi_skip_object)
				{
					int x = mouseEvent->globalPos().x();
					int y = mouseEvent->globalPos().y();
					QWidget* widget = dynamic_cast<QWidget*>(obj);
					if (!widget)
					{
						QWidgetWindow* w = dynamic_cast<QWidgetWindow*>(obj);
						if (w)
							widget = w->widget();
					}
					if (widget)
					{
                        // use the found widget as initial object
                        obj = widget;
						x = mouseEvent->x();
						y = mouseEvent->y();
						sgi_skip_object = widget->property("sgi_skip_object").toBool();
						if (!sgi_skip_object)
						{
							QWidget * child = widget->childAt(x, y);
                            if (child)
                            {
                                sgi_skip_object = child->property("sgi_skip_object").toBool();
                                // use child widget as initial item
                                obj = child;
                            }
						}
					}
					else
					{
						QPoint pt;
						widget = QApplication::activeWindow();
						if (widget)
							pt = widget->mapFromGlobal(mouseEvent->globalPos());
						else
							pt = mouseEvent->globalPos();
						x = pt.x();
						y = pt.y();
					}
					if (!sgi_skip_object)
					{
						qDebug() << "ApplicationEventFilter" << widget << obj;
						contextMenu(widget, obj, x, y);
						ret = true;
					}
				}
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if(mouseEvent->button() == _inspectorContextMenuMouseButton &&
                (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) != 0)
            {
				bool sgi_skip_object = obj->property("sgi_skip_object").toBool();
				if (!sgi_skip_object)
				{
					QWidget* widget = dynamic_cast<QWidget*>(obj);
					if (!widget)
					{
						QWidgetWindow* w = dynamic_cast<QWidgetWindow*>(obj);
						if (w)
							widget = w->widget();
					}
					if (widget)
					{
						int x = mouseEvent->x();
						int y = mouseEvent->y();
						sgi_skip_object = widget->property("sgi_skip_object").toBool();
						if (!sgi_skip_object)
						{
							QWidget * child = widget->childAt(x, y);
							if (child)
								sgi_skip_object = child->property("sgi_skip_object").toBool();
						}
					}
				}
				if (!sgi_skip_object)
				{
					ret = true;
				}
            }
        }
        break;
    }

    // standard event processing
    if(!ret)
        return QObject::eventFilter(obj, event);
    else
        return true;
}

QImageIOPlugin::Capabilities sgi_loader_plugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "sgi_loader")
        return CanRead;
    else
        return 0;
}


QImage sgiImageIOHandler::logoImage()
{
    static QImage s_logo;
    if(s_logo.isNull())
    {
        s_logo.load(":/microscope64.png");
        Q_ASSERT(!s_logo.isNull());
    }
    return s_logo;
}

sgiImageIOHandler::sgiImageIOHandler(QIODevice *device)
    : QImageIOHandler()
{
    ApplicationEventFilter::install();
    setDevice(device);
}

sgiImageIOHandler::~sgiImageIOHandler()
{
}

/*!
 * Return the common identifier of the format.
 */
QByteArray sgiImageIOHandler::name() const
{
    return "sgi_loader";
}

QVariant sgiImageIOHandler::option(ImageOption option) const
{
    if (option == Size) {
        QImage img = logoImage();
        return img.size();
    }
    return QVariant();
}

bool sgiImageIOHandler::supportsOption(ImageOption option) const
{
    return option == Size;
}

bool sgiImageIOHandler::canRead() const
{
    QImage img = logoImage();
    return !img.isNull();
}

bool sgiImageIOHandler::read(QImage *image)
{
    bool bSuccess = false;
    QImage img = logoImage();

    // Make sure we only write to \a image when we succeed.
    if (!img.isNull()) {
        bSuccess = true;
        *image = img;
    }

    return bSuccess;
}

QImageIOHandler * sgi_loader_plugin::create(QIODevice *device, const QByteArray &format) const
{
    //qDebug() << "sgi_loader_plugin::create" << device << format;
    QImageIOHandler *handler = new sgiImageIOHandler(device);
    handler->setFormat(format);
    return handler;
}

} // namespace qt_loader
} // namespace sgi
