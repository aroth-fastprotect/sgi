#include "stdafx.h"
#include "SettingsDialogQt.h"
#include "string_helpers.h"
#include "SGIItemQt"

#include "ImagePreviewDialog.h"

namespace sgi {
namespace qt_plugin {

SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogImagePreview)

bool settingsDialogCreateImpl<SettingsDialogImagePreview>::execute(ISettingsDialogPtr & dialog)
{
    QImage * object = getObject<QImage,SGIItemQtPaintDevice>();
    ImagePreviewDialog * qtdialog = new ImagePreviewDialog(parent(), object);
    dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace qt_plugin
} // namespace sgi
