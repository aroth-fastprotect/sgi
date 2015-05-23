#include "stdafx.h"
#include "SettingsDialogQt.h"
#include <sgi/plugins/SettingsDialog>
#include "ImagePreviewDialog.h"

namespace sgi {
namespace qt_plugin {

SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogImagePreview)

bool settingsDialogCreateImpl<SettingsDialogImagePreview>::execute(ISettingsDialogPtr & dialog)
{
    ImagePreviewDialog * qtdialog = NULL;
    qtdialog = new ImagePreviewDialog(parent(), _item.get());
    dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace qt_plugin
} // namespace sgi
