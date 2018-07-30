#include "stdafx.h"
#include "SettingsDialogOSGEarth.h"
#include "string_helpers.h"
#include <sgi/plugins/SGIItemOsg>

#include <sgi/plugins/SettingsDialog>
#include <osgEarthUtil/AutoClipPlaneHandler>

#include "AutoClipPlaneCullCallbackDialog.h"
#include "TileInspectorDialog.h"
#include "RetrieveElevationDialog.h"
#include "LODScaleOverrideDialog.h"
#include "ShaderEditorDialog.h"

namespace sgi {
namespace osgearth_plugin {

SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogAutoClipPlaneCullCallback)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogTileInspector)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogRetrieveElevation)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogLODScaleOverride)
SETTINGS_DIALOG_CREATE_IMPL_DECLARE_AND_REGISTER(SettingsDialogShaderEditor)

bool settingsDialogCreateImpl<SettingsDialogAutoClipPlaneCullCallback>::execute(ISettingsDialogPtr & dialog)
{
    return openDialog<AutoClipPlaneCullCallbackDialog>(dialog);
}

bool settingsDialogCreateImpl<SettingsDialogTileInspector>::execute(ISettingsDialogPtr & dialog)
{
    return openDialog<TileInspectorDialog>(dialog);
}

bool settingsDialogCreateImpl<SettingsDialogRetrieveElevation>::execute(ISettingsDialogPtr & dialog)
{
    return openDialog<RetrieveElevationDialog>(dialog);
}

bool settingsDialogCreateImpl<SettingsDialogLODScaleOverride>::execute(ISettingsDialogPtr & dialog)
{
    return openDialog<LODScaleOverrideDialog>(dialog);
}

bool settingsDialogCreateImpl<SettingsDialogShaderEditor>::execute(ISettingsDialogPtr & dialog)
{
    return openDialog<ShaderEditorDialog>(dialog);
}

} // namespace osgearth_plugin
} // namespace sgi
