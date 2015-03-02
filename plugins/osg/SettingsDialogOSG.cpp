#include "stdafx.h"
#include "SettingsDialogOSG.h"
#include "SGIItemOsg"

#include <sgi/plugins/SettingsDialog>

#include <osg/Camera>
#include <osg/Shape>

#include "CameraSettings.h"
#include "ImagePreviewDialog.h"

namespace sgi {
namespace osg_plugin {

SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogCamera)
SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogObjectLogger)
SETTINGS_DIALOG_CREATE_IMPL_REGISTER(SettingsDialogImagePreview)

bool settingsDialogCreateImpl<SettingsDialogCamera>::execute(ISettingsDialogPtr & dialog)
{
    osg::Camera * object = getObject<osg::Camera,SGIItemOsg>();
    CameraSettings * qtdialog = new CameraSettings(parent(), object);
    dialog = qtdialog->dialogInterface();
    return true;
}

bool settingsDialogCreateImpl<SettingsDialogObjectLogger>::execute(ISettingsDialogPtr & dialog)
{
    osg::Node * object = getObject<osg::Node,SGIItemOsg>();
    /*
    ObjectLogger * inspector = ObjectLogger::getOrCreateInspector(object);

    if(inspector)
    {
        dialog = inspector->getDialog();
        if(!dialog.valid())
            dialog = new ObjectLoggerDialog(parent(), inspector);
    }
    */
    return true;
}

bool settingsDialogCreateImpl<SettingsDialogImagePreview>::execute(ISettingsDialogPtr & dialog)
{
    osg::Object * object = getObject<osg::Object,SGIItemOsg>();
    ImagePreviewDialog * qtdialog = NULL;
    if(osg::Image* image = dynamic_cast<osg::Image*>(object))
        qtdialog = new ImagePreviewDialog(parent(), image);
    else if(osg::Texture* texture = dynamic_cast<osg::Texture*>(object))
        qtdialog = new ImagePreviewDialog(parent(), texture);
    else if(osg::HeightField* hf = dynamic_cast<osg::HeightField*>(object))
        qtdialog = NULL;
    if(qtdialog)
        dialog = qtdialog->dialogInterface();
    return true;
}

} // namespace osg_plugin
} // namespace sgi
