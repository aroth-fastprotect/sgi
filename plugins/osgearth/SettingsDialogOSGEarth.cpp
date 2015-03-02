#include "stdafx.h"
#include "SettingsDialogOSGEarth.h"
#include "string_helpers.h"
#include "../osg/SGIItemOsg"

#include <sgi/plugins/SettingsDialog>
#include <osgEarthUtil/AutoClipPlaneHandler>

#include "AutoClipPlaneCullCallbackDialog.h"
#include "TileInspectorDialog.h"
#include "RetrieveElevationDialog.h"

namespace sgi {
namespace osgearth_plugin {

SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogAutoClipPlaneCullCallback)
SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogTileInspector)
SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogRetrieveElevation)

bool settingsDialogCreateImpl<SettingsDialogAutoClipPlaneCullCallback>::execute(ISettingsDialogPtr & dialog)
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback,SGIItemOsg,DynamicCaster>();
    AutoClipPlaneCullCallbackDialog * qtdialog = new AutoClipPlaneCullCallbackDialog(parent(), object, info());
    dialog = qtdialog->dialogInterface();
    return true;
}

bool settingsDialogCreateImpl<SettingsDialogTileInspector>::execute(ISettingsDialogPtr & dialog)
{
    TileInspectorDialog * qtdialog = new TileInspectorDialog(parent(), dynamic_cast<SGIItemOsg*>(_item.get()), info());
    dialog = qtdialog->dialogInterface();
    return true;
}

bool settingsDialogCreateImpl<SettingsDialogRetrieveElevation>::execute(ISettingsDialogPtr & dialog)
{
    RetrieveElevationDialog * qtdialog = new RetrieveElevationDialog(parent(), _item.get(), info());
    dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace osgearth_plugin
} // namespace sgi
