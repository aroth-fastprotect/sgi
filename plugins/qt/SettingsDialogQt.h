#pragma once

#include <sgi/plugins/SettingsDialogImpl>

namespace sgi {
namespace qt_plugin {

enum SettingsDialog {
    SettingsDialogNone = -1,
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()

} // namespace qt_plugin
} // namespace sgi
