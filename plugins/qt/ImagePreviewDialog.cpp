#include "stdafx.h"
#include "ImagePreviewDialog.h"

#include <sgi/plugins/SGIItemQt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {

ImagePreviewDialog::ImagePreviewDialog(QWidget * parent, SGIItemBase * item)
    : ImagePreviewDialogBase(parent, item)
{
}

ImagePreviewDialog::~ImagePreviewDialog()
{
}

void ImagePreviewDialog::updateImageAndLabel()
{
    SGIItemQtPaintDevice * paintDeviceItem = dynamic_cast<SGIItemQtPaintDevice*>(_item.get());
    SGIItemQtIcon * iconItem = dynamic_cast<SGIItemQtIcon*>(_item.get());
    QImage * image = paintDeviceItem?getObject<QImage, SGIItemQtPaintDevice, DynamicCaster>():NULL;
    QPixmap * pixmap = paintDeviceItem?getObject<QPixmap, SGIItemQtPaintDevice, DynamicCaster>():NULL;
    QIcon * icon = iconItem?iconItem->object():NULL;

    if(image)
        ImagePreviewDialogBase::setImage(*image);
    else if(pixmap)
        ImagePreviewDialogBase::setImage(*pixmap);
    else if(icon)
    {
        QList<QSize> availableSizes = icon->availableSizes();
        if(!availableSizes.isEmpty())
        {
            QSize biggestSize = availableSizes.front();
            foreach(const QSize & s, availableSizes)
            {
                if(s.width() > biggestSize.width() || s.height() > biggestSize.height())
                    biggestSize = s;
            }
            ImagePreviewDialogBase::setImage(icon->pixmap(biggestSize));
        }
    }
}


} // namespace qt_plugin
} // namespace sgi
