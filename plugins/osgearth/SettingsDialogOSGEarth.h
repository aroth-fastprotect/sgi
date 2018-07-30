#pragma once

#include <sgi/plugins/SettingsDialogImpl>

namespace sgi {
namespace osgearth_plugin {

enum SettingsDialog {
    SettingsDialogNone = -1,
    SettingsDialogAutoClipPlaneCullCallback,
    SettingsDialogTileInspector,
    SettingsDialogRetrieveElevation,
    SettingsDialogLODScaleOverride,
    SettingsDialogShaderEditor
};

SETTINGS_DIALOG_CREATE_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
