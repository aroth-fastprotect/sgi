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
	, _counter(0)
{
    init(DEFAULT_INTERVAL, DEFAULT_AUTOCLOSE_TIME);
}

HighlightWidget::HighlightWidget(unsigned interval, unsigned autoCloseTime, QWidget *parent)
    : QWidget(parent)
    , _timer(new QTimer(this))
	, _counter(0)
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
	QRect r = parentWidget()->geometry();
#ifdef _WIN32
	// On Win32 the m_overlayWidget is created a parent-less window and the
	// position and size is absolute to the screen/global
	r.moveTo(parentWidget()->mapToGlobal(QPoint(0, 0)));
#else
	// On Linux the m_overlayWidget is created as a child of the main window,
	// so position and size is relative to the main window
	//r.moveTo(QPoint(0,0));
	r.moveTo(parentWidget()->mapToGlobal(QPoint(0, 0)));
#endif
	setGeometry(r);

	show();
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

    QPen pen((_counter % 2) ? Qt::blue : Qt::red, 10, Qt::SolidLine, Qt::SquareCap);
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawPolyline(points, 5);
    painter.end();
}

    } // namespace qt_helpers
} // namespace sgi
