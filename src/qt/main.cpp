// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include "main.h"
#include "main.moc"
#include <QApplication>
#include <QMouseEvent>
#include <QWidget>

#include <sgi/ContextMenuQt>

#include <QDebug>

#include <iostream>

namespace sgi {
    namespace qt_loader {

ApplicationEventFilter * ApplicationEventFilter::s_instance = NULL;

ApplicationEventFilter::ApplicationEventFilter(QCoreApplication * parent)
    : QObject(parent)
    , _inspectorContextMenuMouseButton(Qt::RightButton)
    , _inspectorContextMenuMouseModifier(Qt::ControlModifier|Qt::ShiftModifier)
{
    s_instance = this;
    connect(parent, &QCoreApplication::aboutToQuit, this, &ApplicationEventFilter::uninstall);
    parent->installEventFilter(this);
}

ApplicationEventFilter::~ApplicationEventFilter()
{
    qDebug() << "ApplicationEventFilter dtor";
}

bool ApplicationEventFilter::loadSGI()
{
    return true;
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
    qDebug() << "ApplicationEventFilter uninstall";
    _contextMenu = NULL;
    s_instance = NULL;
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
                int x = mouseEvent->screenPos().x();
                int y = mouseEvent->screenPos().y();
                QWidget* widget = dynamic_cast<QWidget*>(obj);
                if(widget)
                {
                    x = mouseEvent->x();
                    y = mouseEvent->y();
                }
                else
                {
                    QPoint pt;
                    widget = QApplication::activeWindow();
                    if(widget)
                        pt = widget->mapFromGlobal(mouseEvent->screenPos().toPoint());
                    else
                        pt = mouseEvent->screenPos().toPoint();
                    x = pt.x();
                    y = pt.y();
                }
                qDebug() << "ApplicationEventFilter" << widget << obj;
                contextMenu(widget, obj, x, y);
                ret = true;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if(mouseEvent->button() == _inspectorContextMenuMouseButton &&
                (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) != 0)
            {
                ret = true;
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
