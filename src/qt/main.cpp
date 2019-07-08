// kate: syntax C++;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#include "main.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QMainWindow>
#include <QJsonDocument>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/ContextMenuQt>
#include <sgi/Shutdown>
#include <sgi/AutoLoadQt>
#include <sgi/ImagePreviewDialog>
#include <sgi/SceneGraphDialog>
#include <sgi/LibraryInfo>
#include <sgi/helpers/qt_widgetwindow>
#include <sgi/plugins/SGIHostItemQt.h>

#include <QImage>
#include <QtCore/QDebug>

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


ApplicationEventFilter * ApplicationEventFilter::s_instance = nullptr;

class SGIEvent : public QEvent
{
public:
    static int SGIEvent_type;
    enum Command {
        CommandNone = 0,
        CommandImage,
        CommandObject,
        CommandListPlugins,
    };
    SGIEvent(Command command, const QString & filename=QString(), const QString & format=QString())
        : QEvent((QEvent::Type)SGIEvent_type)
        , _command(command), _filename(filename), _format(format)
    {
    }

    const Command & command() const { return _command; }
    const QString & filename() const { return _filename; }
    const QString & format() const { return _format; }

private:
    Command _command;
    QString _filename;
    QString _format;
};

int SGIEvent::SGIEvent_type = 0;

ApplicationEventFilter::ApplicationEventFilter(QCoreApplication * parent)
    : QObject(parent)
    , _inspectorContextMenuMouseButton(Qt::RightButton)
    , _inspectorContextMenuMouseModifier(Qt::ControlModifier|Qt::ShiftModifier)
{
    s_instance = this;
#ifdef _DEBUG
#ifdef SGI_USE_GAMMARAY
    qDebug() << "ApplicationEventFilter ctor" << this << "gammaray";
#else
    qDebug() << "ApplicationEventFilter ctor" << this;
#endif
#endif

    connect(parent, &QCoreApplication::aboutToQuit, this, &ApplicationEventFilter::uninstall);
    parent->installEventFilter(this);
    SGIEvent::SGIEvent_type = QEvent::registerEventType();
}

ApplicationEventFilter::~ApplicationEventFilter()
{
#ifdef _DEBUG
    qDebug() << "ApplicationEventFilter dtor" << this;
#endif
}

void ApplicationEventFilter::install()
{
    if(!s_instance)
    {
        s_instance = new ApplicationEventFilter(QCoreApplication::instance());
    }
}

void ApplicationEventFilter::postEvent(QEvent * ev)
{
    qApp->postEvent(ApplicationEventFilter::s_instance, ev);
}

void ApplicationEventFilter::uninstall()
{
#ifdef _DEBUG
    qDebug() << "ApplicationEventFilter uninstall" << this;
#endif
	if (!_contextMenu.isNull())
		delete _contextMenu;
    _hostCallback = nullptr;
    s_instance = nullptr;
    sgi::shutdown<sgi::autoload::Qt>();
    deleteLater();
}

void ApplicationEventFilter::initializeHostCallback()
{
    sgi::getHostCallback<sgi::autoload::Qt>(_hostCallback);
}
  

bool ApplicationEventFilter::contextMenu(QWidget * widget, QObject * obj, float x, float y)
{
    bool ret = false;
    if(_contextMenu.isNull())
        _contextMenu = sgi::createContextMenuQt(widget, obj, _hostCallback.get());
    else
        _contextMenu->setObject(obj);
    if(widget && !_contextMenu.isNull())
    {
        _contextMenu->popup(widget, x, y);
        ret = true;
    }
    return ret;
}

bool ApplicationEventFilter::imagePreviewDialog(QWidget * parent, QImage * image)
{
    initializeHostCallback();
    sgi::SGIHostItemQtPaintDevice item(image);
    IImagePreviewDialog * dialog = sgi::showImagePreviewDialog<sgi::autoload::Qt>(parent, static_cast<const SGIHostItemBase *>(&item), _hostCallback.get());
    if (dialog)
        dialog->show();
    return true;
}

bool ApplicationEventFilter::sceneGraphDialog(QWidget * parent, QObject * obj)
{
    initializeHostCallback();
    sgi::SGIHostItemQt item(obj);
    ISceneGraphDialog * dialog = sgi::showSceneGraphDialog<sgi::autoload::Qt>(parent, static_cast<const SGIHostItemBase *>(&item), _hostCallback.get());
    if (dialog)
        dialog->show();
    return true;
}

bool ApplicationEventFilter::handleEvent(SGIEvent * ev)
{
    bool ret = false;
    switch (ev->command())
    {
    case SGIEvent::CommandImage:
        {
/*
            QSharedPointer<QImage> image(new QImage(ev->filename()));
            imagePreviewDialog(nullptr, image.data());
*/

            QWidget * widget = QApplication::activeWindow();
            QImage * image = new QImage(ev->filename(), ev->format().toLocal8Bit().constData());
            qWarning() << "Open" << ev->filename() << "as" << ev->format() << "loaded" << image->format();

            imagePreviewDialog(widget, image);
            ret = true;
        }
        break;
    case SGIEvent::CommandListPlugins:
        {
            qWarning() << "List plugins";
        }
        break;
    case SGIEvent::CommandObject:
        {
            qWarning() << "Show dialog for object" << ev->filename();
            QWidget * widget = QApplication::activeWindow();
            QObject * obj = nullptr;
            if (ev->filename() == "app")
                obj = qApp;
            else if (ev->filename() == "sgi")
                obj = sgi::libraryInfoQObject<sgi::autoload::Qt>();
            else if (ev->filename() == "mainwindow")
            {
                obj = widget;
                for (QWidget *widget : qApp->topLevelWidgets())
                {
                    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(widget))
                        obj = mainWindow;
                }
            }
            else if (ev->filename() == "activewindow")
                obj = widget;
            sceneGraphDialog(widget, obj);
        }
        break;
    default:
        qCritical() << "Command" << ev->command() << "is not implemented.";
        break;
    }
    return ret;
}

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == SGIEvent::SGIEvent_type)
    {
        return this->handleEvent(static_cast<SGIEvent*>(event));
    }
    bool ret = false;
    switch(event->type())
    {
    case QEvent::MouseButtonRelease:
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if(mouseEvent->button() == _inspectorContextMenuMouseButton &&
                (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) == _inspectorContextMenuMouseModifier)
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
						//qDebug() << "ApplicationEventFilter" << widget << obj;
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
                (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) == _inspectorContextMenuMouseModifier)
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
    bool sendLogoImage = false;
    QByteArray data = device()->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
    {
        bSuccess = sendLogoImage = true;
    }
    else if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        if (obj.contains("image"))
        {
            QJsonObject param = obj.value("image").toObject();
            QString filename = param.value("filename").toString();
            QString format = param.value("format").toString();
            if(!filename.isEmpty())
                ApplicationEventFilter::postEvent(new SGIEvent(SGIEvent::CommandImage, filename, format));
            bSuccess = sendLogoImage = true;
        }
        else if(obj.contains("list-plugins"))
        {
            QJsonObject param = obj.value("list-plugins").toObject();
            ApplicationEventFilter::postEvent(new SGIEvent(SGIEvent::CommandListPlugins));
            bSuccess = sendLogoImage = true;
        }
        else if(obj.contains("object"))
        {
            QJsonObject param = obj.value("object").toObject();
            QString name = param.value("name").toString();
            if (!name.isEmpty())
                ApplicationEventFilter::postEvent(new SGIEvent(SGIEvent::CommandObject, name));
            bSuccess = sendLogoImage = true;
        }
    }

    if (bSuccess && sendLogoImage)
    {
        QImage img = logoImage();

        // Make sure we only write to \a image when we succeed.
        if (!img.isNull()) {
            bSuccess = true;
            *image = img;
        }
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
