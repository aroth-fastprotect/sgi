#pragma once

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace sgi {
namespace Ui {
class Log4cplusWidget;
}

class Log4cplusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Log4cplusWidget(QWidget *parent = nullptr);
    ~Log4cplusWidget();

private:
    QScopedPointer<Ui::Log4cplusWidget> ui;
    GammaRay::UIStateManager m_stateManager;
};

class Log4cplusWidgetFactory : public QObject, public GammaRay::StandardToolUiFactory<Log4cplusWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_log4cplus.json")
};

}
