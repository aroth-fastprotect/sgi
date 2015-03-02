#pragma once

#include <sgi/plugins/SettingsDialogImpl>

namespace osg {
    class Camera;
}

namespace sgi {
namespace qt_plugin {

enum SettingsDialog {
    SettingsDialogNone = -1,
    SettingsDialogImagePreview,
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogImagePreview)

} // namespace qt_plugin
} // namespace sgi
