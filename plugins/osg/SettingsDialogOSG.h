#pragma once

#include <sgi/plugins/SettingsDialogImpl>

namespace osg {
    class Camera;
}

namespace sgi {
namespace osg_plugin {

enum SettingsDialog {
    SettingsDialogNone = -1,
    SettingsDialogCamera,
    SettingsDialogObjectLogger,
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()

} // namespace osg_plugin
} // namespace sgi
