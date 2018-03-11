#include "stdafx.h"
#include "ImageQtWidget.h"
#include <QMouseEvent>
#include <QStyle>
#include <QLayout>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

ImageQtWidget::ImageQtWidget(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    setMouseTracking(true);
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setSizePolicy(sizePolicy);
}

void ImageQtWidget::mouseMoveEvent(QMouseEvent *ev)
{
    QLabel::mouseMoveEvent(ev);

    float x = float(ev->x()) / float(width());
    float y = float(ev->y()) / float(height());

    emit mouseMoved(x, y);
}

void ImageQtWidget::resizeEvent(QResizeEvent *event)
{
    QSize s = _imageSize;
    s.scale(event->size(), Qt::KeepAspectRatio);
    s = QLayout::closestAcceptableSize(this, s);
    QRect imageRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, s, rect());
    setGeometry(imageRect);

    QLabel::resizeEvent(event);
}

void ImageQtWidget::setImage(const QImage & image)
{
    if(image.isNull())
    {
        _imageSize = QSize(0,0);
        QLabel::clear();
    }
    else
    {
        _imageSize = image.size();
        QLabel::setPixmap(QPixmap::fromImage(image));
    }
}

QSize ImageQtWidget::sizeHint() const
{
    return _imageSize;
}

} // namespace sgi
