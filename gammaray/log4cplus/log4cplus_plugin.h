#pragma once

#include "core/toolfactory.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE

namespace sgi {
class Log4cplusPlugin : public QObject
{
    Q_OBJECT
public:
    explicit Log4cplusPlugin(GammaRay::ProbeInterface *probe, QObject *parent = nullptr);
    ~Log4cplusPlugin();
};

class Log4cplusPluginFactory : public QObject, public GammaRay::StandardToolFactory<QObject, Log4cplusPlugin>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_log4cplus.json")
public:
    explicit Log4cplusPluginFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}
