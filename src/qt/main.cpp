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
    {
        qDebug() << "sgi_loader_plugin::capabilities" << format;
        ApplicationEventFilter::install();
        return Capabilities(CanRead);
    }
    else
        return 0;
}

QImageIOHandler *sgi_loader_plugin::create(QIODevice *device, const QByteArray &format) const
{
    qDebug() << "sgi_loader_plugin::create" << format;
    ApplicationEventFilter::install();
    return NULL;
}

} // namespace qt_loader
} // namespace sgi
