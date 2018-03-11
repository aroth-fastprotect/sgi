#pragma once

#include <QLabel>

namespace sgi {

class ImageQtWidget : public QLabel
{
    Q_OBJECT
public:
    ImageQtWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

    void setImage(const QImage & image);

    QSize sizeHint() const Q_DECL_OVERRIDE;

signals:
    void mouseMoved(float x, float y);

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QSize _imageSize;
};

} // namespace sgi
