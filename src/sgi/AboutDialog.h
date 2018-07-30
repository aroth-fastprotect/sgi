#pragma once

#include <sgi/plugins/SGISettingsDialogImpl>

QT_BEGIN_NAMESPACE
class Ui_AboutDialog;
QT_END_NAMESPACE

namespace sgi {
namespace internal_plugin {


class AboutDialog : public SettingsQDialogImpl
{
	Q_OBJECT

public:
    AboutDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~AboutDialog() override;

private:
    Ui_AboutDialog  * ui;
};

} // namespace internal_plugin
} // namespace sgi
