#include "stdafx.h"
#include "ImagePreviewDialog.h"

#include <sgi/plugins/SGIItemQt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace qt_plugin {

ImagePreviewDialog::ImagePreviewDialog(SGIPluginHostInterface * hostInterface, SGIItemBase * item, QWidget * parent)
    : ImagePreviewDialog(hostInterface, item, parent)
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
        ImagePreviewDialog::setImage(*image);
    else if(pixmap)
        ImagePreviewDialog::setImage(*pixmap);
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
            ImagePreviewDialog::setImage(icon->pixmap(biggestSize));
        }
    }
}


} // namespace qt_plugin
} // namespace sgi
