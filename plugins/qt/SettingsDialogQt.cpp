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
    ImagePreviewDialog * qtdialog = NULL;
    QImage * image = NULL;
    QPixmap * pixmap = NULL;
    QIcon * icon = NULL;
    SGIItemQtIcon * iconItem = dynamic_cast<SGIItemQtIcon *>(_item.get());
    if(iconItem)
        icon = getObject<QIcon,SGIItemQtIcon>();
    else
    {
        image = getObject<QImage,SGIItemQtPaintDevice>();
        pixmap = getObject<QPixmap,SGIItemQtPaintDevice>();
    }
    if(image)
        qtdialog = new ImagePreviewDialog(parent(), image);
    else if(pixmap)
        qtdialog = new ImagePreviewDialog(parent(), pixmap);
    else if(icon)
        qtdialog = new ImagePreviewDialog(parent(), icon);

    if(qtdialog)
        dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace qt_plugin
} // namespace sgi
