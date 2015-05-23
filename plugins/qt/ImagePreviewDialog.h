#pragma once

#include <sgi/plugins/ImagePreviewDialogBase>

namespace sgi {
namespace qt_plugin {

class ImagePreviewDialog : public ImagePreviewDialogBase
{
    Q_OBJECT

public:
	ImagePreviewDialog(QWidget * parent, SGIItemBase * item);
    virtual ~ImagePreviewDialog();

protected:
    virtual void updateImageAndLabel();
};

} // namespace qt_plugin
} // namespace sgi
