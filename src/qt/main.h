// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth
#pragma once

#include <QImageIOHandler>

class QCoreApplication;

namespace sgi {
    namespace qt_loader {

class ApplicationEventFilter : public QObject
{
    Q_OBJECT
public:
    static void install();
protected:
    ApplicationEventFilter(QCoreApplication * parent=NULL);

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    static ApplicationEventFilter * s_instance;

    int                 _inspectorContextMenuMouseButton;
    int                 _inspectorContextMenuMouseModifier;
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
