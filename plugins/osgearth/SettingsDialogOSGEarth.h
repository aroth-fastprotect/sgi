#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

namespace osg {
    class Camera;
}

namespace sgi {
namespace osgearth_plugin {

enum SettingsDialog {
    SettingsDialogNone = -1,
    SettingsDialogAutoClipPlaneCullCallback,
    SettingsDialogTileInspector,
    SettingsDialogRetrieveElevation,
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogAutoClipPlaneCullCallback)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogTileInspector)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE(SettingsDialogRetrieveElevation)

} // namespace osgearth_plugin
} // namespace sgi
