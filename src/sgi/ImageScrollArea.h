#pragma once

#include <QScrollArea>

namespace sgi {

class ImageScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ImageScrollArea(QWidget *parent = 0);

    void setImage(const QImage & image);

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setScaleFactor(float factor);
    float scaleFactor() const;

    bool fitToWindow() const;
    void setFitToWindow(bool enable);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QSize _imageSize;
    float _scaleFactor;
    bool _fitToWindow;
};

} // namespace sgi
