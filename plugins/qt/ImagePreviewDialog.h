#pragma once

#include <sgi/plugins/ImagePreviewDialog>

namespace sgi {
namespace qt_plugin {

class ImagePreviewDialog : public ImagePreviewDialog
{
    Q_OBJECT

public:
	ImagePreviewDialog(SGIPluginHostInterface * hostInterface, SGIItemBase * item, QWidget * parent = NULL);
    virtual ~ImagePreviewDialog();

protected:
    virtual void updateImageAndLabel();
};

} // namespace qt_plugin
} // namespace sgi
