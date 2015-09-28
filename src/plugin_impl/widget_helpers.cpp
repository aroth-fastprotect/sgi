#include <sgi/helpers/qt_widgets>
#include <__/__/include/sgi/helpers/qt_widgets.moc>
#include <QTimer>
#include <QPen>
#include <QPainter>

namespace sgi {
    namespace qt_helpers {

HighlightWidget::HighlightWidget(QWidget *parent)
    : QWidget(parent)
    , _timer(new QTimer(this))
{
    init(200, 10000);
}

HighlightWidget::HighlightWidget(unsigned interval, unsigned autoCloseTime, QWidget *parent)
    : QWidget(parent)
    , _timer(new QTimer(this))
{
    init(interval, autoCloseTime);
}
HighlightWidget::~HighlightWidget()
{
}

void HighlightWidget::init(unsigned interval, unsigned autoCloseTime)
{
    _maxCounter = autoCloseTime / interval;

    connect(_timer, &QTimer::timeout, this, &HighlightWidget::blink);
    _timer->start(interval);

    // set transparency for rendering
    // setting Qt::WA_TranslucentBackground automatically sets Qt::WA_NoSystemBackground
    setAttribute(Qt::WA_TranslucentBackground);
    // set transparency for mouse events
    setAttribute(Qt::WA_TransparentForMouseEvents);

#ifdef _WIN32
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#else
    setWindowFlags(Qt::FramelessWindowHint
                    | Qt::Window
                    | Qt::BypassWindowManagerHint
                    | Qt::NoDropShadowWindowHint
                    | Qt::WindowDoesNotAcceptFocus
                    | Qt::WindowTransparentForInput
                    | Qt::BypassGraphicsProxyWidget
        );
#endif
}

void HighlightWidget::blink()
{
    ++_counter;
    if(_counter > _maxCounter)
        deleteLater();
    else
        update();
}

void HighlightWidget::paintEvent(QPaintEvent* ev)
{
    QRect rectangle = geometry();

    const QPoint points[5] = {
        QPoint(0, 0),
        QPoint(rectangle.width(), 0),
        QPoint(rectangle.width(), rectangle.height()),
        QPoint(0,rectangle.height()),
        QPoint(0, 0)
    };

    QPen pen((_counter % 2) ? Qt::blue : Qt::darkBlue, 10, Qt::SolidLine, Qt::SquareCap);
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawPolyline(points, 5);
    painter.end();
}

    } // namespace qt_helpers
} // namespace sgi
