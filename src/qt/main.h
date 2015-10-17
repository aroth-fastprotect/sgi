// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth
#pragma once

#include <QImageIOHandler>

class QCoreApplication;

namespace sgi {

    class IContextMenuQt;
    typedef QPointer<IContextMenuQt> IContextMenuQtPtr;

    namespace qt_loader {

class ApplicationEventFilter : public QObject
{
    Q_OBJECT
public:
    static void install();
protected:
    ApplicationEventFilter(QCoreApplication * parent=NULL);
    ~ApplicationEventFilter();

    bool eventFilter(QObject *obj, QEvent *event) override;
    void uninstall();

    bool contextMenu(QWidget * widget, QObject * item, float x, float y);
    bool loadSGI();

private:
    static ApplicationEventFilter * s_instance;

    int                     _inspectorContextMenuMouseButton;
    int                     _inspectorContextMenuMouseModifier;
    sgi::IContextMenuQtPtr  _contextMenu;
};

class sgi_loader_plugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "sgi_loader.json")
public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};
    } // namespace qt_loader
} // namespace sgi
