// kate: syntax C++11;

#pragma once

#include <QWidget>

class QTimer;

namespace sgi {
namespace qt_helpers {
namespace details {
class HighlightWidget : public QWidget
{
    Q_OBJECT

public:
    enum { DEFAULT_INTERVAL=330, DEFAULT_AUTOCLOSE_TIME = 10000 };
    HighlightWidget(QWidget *parent=0);
    HighlightWidget(unsigned interval, unsigned autoCloseTime= DEFAULT_AUTOCLOSE_TIME, QWidget *parent=0);
    virtual ~HighlightWidget();

protected:
    void blink();
    virtual void paintEvent(QPaintEvent* ev) override;

private:
    void init(unsigned interval, unsigned autoCloseTime);

private:
    Q_DISABLE_COPY(HighlightWidget)
    QTimer * _timer;
    unsigned _counter;
    unsigned _maxCounter;
};
        } // namespace details
    } // namespace qt_helpers
} // namespace sgi
