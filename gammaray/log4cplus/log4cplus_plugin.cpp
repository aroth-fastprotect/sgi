
#include "log4cplus_plugin.h"
#include "log4cplus_model.h"

using namespace sgi;

Log4cplusPlugin::Log4cplusPlugin(GammaRay::ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    auto model = new Log4cplusModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Log4cplusModel"), model);
}

Log4cplusPlugin::~Log4cplusPlugin()
{
}
