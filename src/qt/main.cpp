// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include "main.h"
#include "main.moc"
#include <QCoreApplication>
#include <QMouseEvent>

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
    parent->installEventFilter(this);
}

void ApplicationEventFilter::install()
{
    if(!s_instance)
    {
        s_instance = new ApplicationEventFilter(QCoreApplication::instance());
    }
}

bool ApplicationEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent->button() == _inspectorContextMenuMouseButton &&
            (mouseEvent->modifiers() & _inspectorContextMenuMouseModifier) != 0)
        {
            std::cout << "ApplicationEventFilter " << std::endl;
            return true;
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}

QImageIOPlugin::Capabilities sgi_loader_plugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "sgi_loader")
        return Capabilities(CanRead | CanWrite);
    if (!format.isEmpty())
        return 0;
    Capabilities cap;
    if (device->isReadable())
        cap |= CanRead;
    if (device->isWritable())
        cap |= CanWrite;
    return cap;
}

QImageIOHandler *sgi_loader_plugin::create(QIODevice *device, const QByteArray &format) const
{
    ApplicationEventFilter::install();
    return NULL;
}

} // namespace qt_loader
} // namespace sgi
