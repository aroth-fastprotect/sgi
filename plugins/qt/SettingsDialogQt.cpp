#include "stdafx.h"
#include "SettingsDialogQt.h"
#include "SGIItemQt"
#include <sgi/plugins/SettingsDialog>
#include "ImagePreviewDialog.h"

namespace sgi {
namespace qt_plugin {

SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogImagePreview)

bool settingsDialogCreateImpl<SettingsDialogImagePreview>::execute(ISettingsDialogPtr & dialog)
{
    QImage * image = getObject<QImage,SGIItemQtPaintDevice>();
    QPixmap * pixmap = getObject<QPixmap,SGIItemQtPaintDevice>();
    ImagePreviewDialog * qtdialog = NULL;
    if(image)
        qtdialog = new ImagePreviewDialog(parent(), image);
    else if(pixmap)
        qtdialog = new ImagePreviewDialog(parent(), pixmap);

    if(qtdialog)
        dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace qt_plugin
} // namespace sgi
