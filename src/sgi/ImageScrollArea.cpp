#include "stdafx.h"
#include "ImageScrollArea.h"
#include <QScrollBar>
#include <QResizeEvent>
#include <QStyle>
#include <QLayout>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

ImageScrollArea::ImageScrollArea(QWidget *parent)
    : QScrollArea(parent)
    , _imageSize(0,0)
    , _scaleFactor(1.0f)
{
    setMouseTracking(true);
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setSizePolicy(sizePolicy);
}


#if 0
void ImagePreviewDialog::ImagePreviewDialogImpl::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
        + ((factor - 1) * scrollBar->pageStep() / 2)));
}
#endif

void ImageScrollArea::setScaleFactor(float factor)
{
    _scaleFactor = factor;

    int width = _imageSize.width() * _scaleFactor;
    int height = _imageSize.height() * _scaleFactor;
    auto hbar = horizontalScrollBar();
    auto vbar = verticalScrollBar();

    hbar->setValue(int(factor * hbar->value()
        + ((factor - 1) * hbar->pageStep() / 2)));
    hbar->setMaximum(width);
    hbar->setPageStep((width / 10));

    vbar->setValue(int(factor * vbar->value()
        + ((factor - 1) * vbar->pageStep() / 2)));
    vbar->setMaximum(height);
    vbar->setPageStep((height / 10));

    //_priv->ui->scrollAreaImageQt->setWidgetResizable(fitToWindow);
    //_priv->ui->scrollAreaImageGL->setWidgetResizable(fitToWindow);

}

float ImageScrollArea::scaleFactor() const
{
    return _scaleFactor;
}

void ImageScrollArea::resizeEvent(QResizeEvent *event)
{
    QSize s = _imageSize * _scaleFactor;
    s.scale(event->size(), Qt::KeepAspectRatio);
    s = QLayout::closestAcceptableSize(this, s);
    QRect imageRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, s, rect());
    setGeometry(imageRect);

    QScrollArea::resizeEvent(event);
}

void ImageScrollArea::setImage(const QImage & image)
{
    if(image.isNull())
        _imageSize = QSize(0,0);
    else
        _imageSize = image.size();
    _scaleFactor = 1.0f;
}

QSize ImageScrollArea::sizeHint() const
{
    return _imageSize * _scaleFactor;
}

} // namespace sgi
