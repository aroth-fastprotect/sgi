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
    SettingsDialogImagePreview,
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogCamera)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogObjectLogger)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogImagePreview)

} // namespace osg_plugin
} // namespace sgi
