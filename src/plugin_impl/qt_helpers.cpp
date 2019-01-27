// kate: syntax C++11;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#include <sgi/helpers/qt>

#include <sstream>
#include <iomanip>
#include <QObject>
#include <QMetaObject>
#include <QString>
#include <QVariant>
#include <QBitArray>
#include <QByteArray>
#include <QPoint>
#include <QLine>
#include <QSize>
#include <QRect>
#include <QUrl>
#include <QDateTime>
#include <QLocale>
#include <QEasingCurve>
#include <QSizePolicy>
#include <QPaintDevice>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QCursor>
#include <QKeySequence>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QMetaEnum>
#include <QUuid>
#include <QFont>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QTextLength>
#include <QTextFormat>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QPolygonF>
#include <QIcon>
#include <sgi/helpers/rtti>

namespace std {
    SGI_QT_HELPER_EXPORT std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, QImage::Format t);
}

namespace sgi {
    namespace qt_helpers {

const static int metaTypeQtSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtSGIItem>("sgi::qt_helpers::QtSGIItem");
const static int metaTypeQtMenuSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtMenuSGIItem>("sgi::qt_helpers::QtMenuSGIItem");
const static int metaTypeQtTableSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtTableSGIItem>("sgi::qt_helpers::QtTableSGIItem");

#define QENUM_OSTREAM(__enum_name) \
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const __enum_name & c) { \
        auto metaEnum = QMetaEnum::fromType<__enum_name>(); \
        return os << metaEnum.valueToKey(c); \
    }
#define QFLAG_OSTREAM(__enum_name) \
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const __enum_name & c) { \
        auto metaEnum = QMetaEnum::fromType<__enum_name>(); \
        return os << metaEnum.valueToKey(c); \
    }

// NOTE: Generally, do not add QT_Q_ENUM if a corresponding Q_Q_FLAG exists.
QENUM_OSTREAM(Qt::ScrollBarPolicy)
QENUM_OSTREAM(Qt::FocusPolicy)
QENUM_OSTREAM(Qt::ContextMenuPolicy)
QENUM_OSTREAM(Qt::ArrowType)
QENUM_OSTREAM(Qt::ToolButtonStyle)
QENUM_OSTREAM(Qt::PenStyle)
QENUM_OSTREAM(Qt::PenCapStyle)
QENUM_OSTREAM(Qt::PenJoinStyle)
QENUM_OSTREAM(Qt::BrushStyle)
QENUM_OSTREAM(Qt::FillRule)
QENUM_OSTREAM(Qt::MaskMode)
QENUM_OSTREAM(Qt::BGMode)
QENUM_OSTREAM(Qt::ClipOperation)
QENUM_OSTREAM(Qt::SizeMode)
QENUM_OSTREAM(Qt::Axis)
QENUM_OSTREAM(Qt::Corner)
QENUM_OSTREAM(Qt::Edge)
QENUM_OSTREAM(Qt::LayoutDirection)
QENUM_OSTREAM(Qt::SizeHint)
QENUM_OSTREAM(Qt::Orientation)
QENUM_OSTREAM(Qt::DropAction)
QENUM_OSTREAM(Qt::Alignment)
QENUM_OSTREAM(Qt::TextFlag)
QENUM_OSTREAM(Qt::Orientations)
QENUM_OSTREAM(Qt::DropActions)
QENUM_OSTREAM(Qt::Edges)
QENUM_OSTREAM(Qt::DockWidgetAreas)
QENUM_OSTREAM(Qt::ToolBarAreas)
QENUM_OSTREAM(Qt::DockWidgetArea)
QENUM_OSTREAM(Qt::ToolBarArea)
QENUM_OSTREAM(Qt::TextFormat)
QENUM_OSTREAM(Qt::TextElideMode)
QENUM_OSTREAM(Qt::DateFormat)
QENUM_OSTREAM(Qt::TimeSpec)
QENUM_OSTREAM(Qt::DayOfWeek)
QENUM_OSTREAM(Qt::CursorShape)
QENUM_OSTREAM(Qt::GlobalColor)
QENUM_OSTREAM(Qt::AspectRatioMode)
QENUM_OSTREAM(Qt::TransformationMode)
QENUM_OSTREAM(Qt::ImageConversionFlags)
QENUM_OSTREAM(Qt::Key)
QENUM_OSTREAM(Qt::ShortcutContext)
QENUM_OSTREAM(Qt::TextInteractionFlag)
QENUM_OSTREAM(Qt::TextInteractionFlags)
QENUM_OSTREAM(Qt::ItemSelectionMode)
QENUM_OSTREAM(Qt::ItemSelectionOperation)
QENUM_OSTREAM(Qt::ItemFlags)
QENUM_OSTREAM(Qt::CheckState)
QENUM_OSTREAM(Qt::ItemDataRole)
QENUM_OSTREAM(Qt::SortOrder)
QENUM_OSTREAM(Qt::CaseSensitivity)
QENUM_OSTREAM(Qt::MatchFlags)
QENUM_OSTREAM(Qt::KeyboardModifiers)
QENUM_OSTREAM(Qt::MouseButtons)
QENUM_OSTREAM(Qt::WindowType)
QENUM_OSTREAM(Qt::WindowState)
QENUM_OSTREAM(Qt::WindowModality)
QENUM_OSTREAM(Qt::WidgetAttribute)
QENUM_OSTREAM(Qt::ApplicationAttribute)
QENUM_OSTREAM(Qt::WindowFlags)
QENUM_OSTREAM(Qt::WindowStates)
QENUM_OSTREAM(Qt::FocusReason)
QENUM_OSTREAM(Qt::InputMethodHint)
QENUM_OSTREAM(Qt::InputMethodQuery)
QENUM_OSTREAM(Qt::InputMethodHints)
QENUM_OSTREAM(Qt::EnterKeyType)
QENUM_OSTREAM(Qt::InputMethodQueries)
QENUM_OSTREAM(Qt::TouchPointStates)
QENUM_OSTREAM(Qt::ScreenOrientation)
QENUM_OSTREAM(Qt::ScreenOrientations)
QENUM_OSTREAM(Qt::ConnectionType)
QENUM_OSTREAM(Qt::ApplicationState)
#ifndef QT_NO_GESTURES
QENUM_OSTREAM(Qt::GestureState)
QENUM_OSTREAM(Qt::GestureType)
QENUM_OSTREAM(Qt::NativeGestureType)
#endif
QENUM_OSTREAM(Qt::CursorMoveStyle)
QENUM_OSTREAM(Qt::TimerType)
QENUM_OSTREAM(Qt::ScrollPhase)
QENUM_OSTREAM(Qt::MouseEventSource)
QENUM_OSTREAM(Qt::MouseEventFlag)
QENUM_OSTREAM(Qt::ChecksumType)
QENUM_OSTREAM(Qt::TabFocusBehavior)

QString fromLocal8Bit(const std::string & str)
{
    return QString::fromLocal8Bit(str.data(), str.size());
}

std::string toLocal8Bit(const QString & str)
{
    QByteArray qba = str.toLocal8Bit();
    return std::string(qba.constData(), qba.size());
}

QString fromUtf8(const std::string & str)
{
    return QString::fromUtf8(str.data(), str.size());
}

std::string toUtf8(const QString & str)
{
    QByteArray qba = str.toUtf8();
    return std::string(qba.constData(), qba.size());
}

std::string getObjectTypename(const QObject * object)
{
    const QMetaObject * meta = object?object->metaObject():nullptr;
    return meta?std::string(meta->className()):"(null)";
}

std::string getObjectName(const QObject * object, bool includeAddr)
{
    std::string ret;
    if(includeAddr)
    {
        std::stringstream buf;
        buf << (void*)object;
        if(object)
        {
            QString name = object->objectName();
            if(!name.isEmpty())
                buf << ' ' << toUtf8(name);
        }
        ret = buf.str();
    }
    else
    {
        ret = object? toUtf8(object->objectName()):std::string("(null)");
        if(ret.empty())
        {
            std::stringstream buf;
            buf << (void*)object;
            ret = buf.str();
        }
    }
    return ret;
}

std::string getObjectNameAndType(const QObject * object, bool includeAddr)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object, includeAddr) << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectTypename(const QPaintDevice * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

std::string getObjectName(const QPaintDevice * object, bool includeAddr)
{
    std::string ret;
    std::stringstream buf;
    buf << (void*)object;
    ret = buf.str();
    return ret;
}

std::string getObjectNameAndType(const QPaintDevice * object, bool includeAddr)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object, includeAddr) << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

    } // namespace qt_helpers
} // namespace sgi


namespace std {

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QVariant & v)
    {
        switch(v.type())
        {
        case QVariant::Invalid: os << "(invalid)"; break;
        case QVariant::Bool: os << (v.toBool()?"true":"false"); break;
        case QVariant::Int: os << v.toInt(); break;
        case QVariant::UInt: os << v.toUInt(); break;
        case QVariant::LongLong: os << v.toLongLong(); break;
        case QVariant::ULongLong: os << v.toULongLong(); break;
        case QVariant::Double: os << v.toDouble(); break;
        case QVariant::Char: os << v.toChar(); break;
        case QVariant::Map: os << v.toMap(); break;
        case QVariant::List: os << v.toList(); break;
        case QVariant::String: os << v.toString(); break;
        case QVariant::StringList: os << v.toStringList(); break;
        case QVariant::ByteArray: os << v.toByteArray(); break;
        case QVariant::BitArray: os << v.toBitArray(); break;
        case QVariant::Date: os << v.toDate(); break;
        case QVariant::Time: os << v.toTime(); break;
        case QVariant::DateTime: os << v.toDateTime(); break;
        case QVariant::Url: os << v.toUrl(); break;
        case QVariant::Locale: os << v.toLocale(); break;
        case QVariant::Rect: os << v.toRect(); break;
        case QVariant::RectF: os << v.toRectF(); break;
        case QVariant::Size: os << v.toSize(); break;
        case QVariant::SizeF: os << v.toSizeF(); break;
        case QVariant::Line: os << v.toLine(); break;
        case QVariant::LineF: os << v.toLineF(); break;
        case QVariant::Point: os << v.toPoint(); break;
        case QVariant::PointF: os << v.toPointF(); break;
        case QVariant::RegExp: os << v.toRegExp(); break;
        case QVariant::Hash: os << v.toHash(); break;
        case QVariant::EasingCurve: os << v.toEasingCurve(); break;
        case QVariant::Uuid: os << v.toUuid(); break;
        case QVariant::ModelIndex: os << v.toModelIndex(); break;
        case QVariant::PersistentModelIndex: os << v.toPersistentModelIndex(); break;
        case QVariant::Font: os << v.value<QFont>(); break;
        case QVariant::Pixmap: os << v.value<QPixmap>(); break;
        case QVariant::Brush: os << v.value<QBrush>(); break;
        case QVariant::Color: os << v.value<QColor>(); break;
        case QVariant::Palette: os << v.value<QPalette>(); break;
        case QVariant::Image: os << v.value<QImage>(); break;
        case QVariant::Polygon: os << v.value<QPolygon>(); break;
        case QVariant::Region: os << v.value<QRegion>(); break;
        case QVariant::Bitmap: os << v.value<QBitmap>(); break;
        case QVariant::Cursor: os << v.value<QCursor>(); break;
        case QVariant::KeySequence: os << v.value<QKeySequence>(); break;
        case QVariant::Pen: os << v.value<QPen>(); break;
        case QVariant::TextLength: os << v.value<QTextLength>(); break;
        case QVariant::TextFormat: os << v.value<QTextFormat>(); break;
        case QVariant::Matrix: os << v.value<QMatrix>(); break;
        case QVariant::Transform: os << v.value<QTransform>(); break;
        case QVariant::Matrix4x4: os << v.value<QMatrix4x4>(); break;
        case QVariant::Vector2D: os << v.value<QVector2D>(); break;
        case QVariant::Vector3D: os << v.value<QVector3D>(); break;
        case QVariant::Vector4D: os << v.value<QVector4D>(); break;
        case QVariant::Quaternion: os << v.value<QQuaternion>(); break;
        case QVariant::PolygonF: os << v.value<QPolygonF>(); break;
        case QVariant::Icon: os << v.value<QIcon>(); break;
        default: os << v.toString(); break;
        }
        return os;
    }

    std::basic_ostream<char>& writeVariant(std::basic_ostream<char>& os, const QVariant & v, const QMetaProperty * property)
    {
        if(!property)
            os << v;
        else {
            if(property->isFlagType())
                os << property->enumerator().valueToKeys(v.toInt()).constData();
            else if(property->isEnumType())
                os << property->enumerator().valueToKey(v.toInt());
            else
                os << v;
        }
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str)
    {
        return os << str.toUtf8().constData();
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QStringRef& str)
    {
        const QString * p = str.string();
        if(p)
            return os << p->toUtf8().constData();
        else
            return os << "(null)";
    }

    namespace {
        QByteArray bitsToBytes(const QBitArray & bits)
        {
            QByteArray bytes;
            bytes.resize(bits.count()/8+1);
            bytes.fill(0);
            // Convert from QBitArray to QByteArray
            for(int b=0; b<bits.count(); ++b)
                bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
            return bytes;
        }
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QBitArray& v)
    {
        return os << QString::fromLocal8Bit(bitsToBytes(v).toHex());
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QByteArray& v)
    {
        return os << QString::fromLocal8Bit(v.toHex());
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QByteRef& str)
    {
        return os << (char) str;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QDate & date)
    {
        return os << qPrintable(date.toString(Qt::ISODate));
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QTime& t)
    {
        return os << qPrintable(t.toString(Qt::ISODate));
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QDateTime& date)
    {
        return os << qPrintable(date.toString(Qt::ISODate));
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QLocale& l)
    {
        return os << l.name();
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const _QtColorBox<char>& color)
    {
        if(color._color.isValid())
        {
            std::stringstream buf;
            buf << "#";
            buf << std::hex << std::setw(2) << std::setfill('0') << color._color.red();
            buf << std::hex << std::setw(2) << std::setfill('0') << color._color.green();
            buf << std::hex << std::setw(2) << std::setfill('0') << color._color.blue();
            if ( color._color.alpha() < 255)
                buf << std::hex << std::setw(2) << std::setfill('0') << color._color.alpha();
            os << "<div style=\"width: 16px; height: 16px; background: " << buf.str() << ";\">&nbsp;</div>";
        }
        else
            os << "invalid";
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QColor& color)
    {
        if(color.isValid())
        {
            std::stringstream buf;
            buf << "#";
            buf << std::hex << std::setw(2) << std::setfill('0') << color.red();
            buf << std::hex << std::setw(2) << std::setfill('0') << color.green();
            buf << std::hex << std::setw(2) << std::setfill('0') << color.blue();
            if ( color.alpha() < 255)
                buf << std::hex << std::setw(2) << std::setfill('0') << color.alpha();
            return os << buf.str();
        }
        else
            return os << "invalid";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSize & size)
    {
        return os << "(" << size.width() << ", " << size.height() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizeF & size)
    {
        return os << "(" << size.width() << ", " << size.height() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRect & r)
    {
        return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRectF & r)
    {
        return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPoint & p)
    {
        return os << "(" << p.x() << ", " << p.y() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPointF & p)
    {
        return os << "(" << p.x() << ", " << p.y() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QChar & c)
    {
        return os << "0x" << std::hex << c.unicode() << std::dec;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QUrl & url)
    {
        return os << url.toString();
    }
    QENUM_OSTREAM(QSizePolicy::Policy);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizePolicy & sp)
    {
        return os << "(" << sp.horizontalPolicy() << ", " << sp.verticalPolicy() << ")";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, QImage::Format t)
    {
        switch(t)
        {
        case QImage::Format_Invalid: os << "Invalid"; break;
        case QImage::Format_Mono: os << "Mono"; break;
        case QImage::Format_MonoLSB: os << "MonoLSB"; break;
        case QImage::Format_Indexed8: os << "Indexed8"; break;
        case QImage::Format_RGB32: os << "RGB32"; break;
        case QImage::Format_ARGB32: os << "ARGB32"; break;
        case QImage::Format_ARGB32_Premultiplied: os << "ARGB32_Premultiplied"; break;
        case QImage::Format_RGB16: os << "RGB16"; break;
        case QImage::Format_ARGB8565_Premultiplied: os << "ARGB8565_Premultiplied"; break;
        case QImage::Format_RGB666: os << "RGB666"; break;
        case QImage::Format_ARGB6666_Premultiplied: os << "ARGB6666_Premultiplied"; break;
        case QImage::Format_RGB555: os << "RGB555"; break;
        case QImage::Format_ARGB8555_Premultiplied: os << "ARGB8555_Premultiplied"; break;
        case QImage::Format_RGB888: os << "RGB888"; break;
        case QImage::Format_RGB444: os << "RGB444"; break;
        case QImage::Format_ARGB4444_Premultiplied: os << "ARGB4444_Premultiplied"; break;
        case QImage::Format_RGBX8888: os << "RGBX8888"; break;
        case QImage::Format_RGBA8888: os << "RGBA8888"; break;
        case QImage::Format_RGBA8888_Premultiplied: os << "RGBA8888_Premultiplied"; break;
        case QImage::Format_BGR30: os << "BGR30"; break;
        case QImage::Format_A2BGR30_Premultiplied: os << "A2BGR30_Premultiplied"; break;
        case QImage::Format_RGB30: os << "RGB30"; break;
        case QImage::Format_A2RGB30_Premultiplied: os << "A2RGB30_Premultiplied"; break;
        default: os << (int)t; break;
        }
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPalette & palette)
    {
        os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
        os << "<tr><td>cacheKey</td><td>" << palette.cacheKey() << "</td></tr>" << std::endl;

        QMetaEnum colorGroupEnum = QMetaEnum::fromType<QPalette::ColorGroup>();
        QMetaEnum colorRoleEnum = QMetaEnum::fromType<QPalette::ColorRole>();
        for(int cg = 0; cg < QPalette::NColorGroups; ++cg)
        {
            for(int cr = 0; cr < QPalette::NColorRoles; ++cr)
            {
                QColor c = palette.color((QPalette::ColorGroup)cg, (QPalette::ColorRole)cr);
                os << "<tr><td>"
                   << colorGroupEnum.key(cg)
                   << '/' << colorRoleEnum.key(cr)
                   << "</td><td>"
                   << c << std::colorBox<char>(c)
                   << "</td></tr>"
                   << std::endl;
            }
        }
        os << "</table>" << std::endl;
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QFont & f)
    {
        return os << f.family() << ',' << f.pixelSize();
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPixmap & p)
    {
        return os << '(' << p.size() << ',' << p.devType() << ')';
    }
    QENUM_OSTREAM(Qt::BrushStyle);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QBrush & b)
    {
        return os << '(' << b.color() << ',' << b.style() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QImage & i)
    {
        return os << '(' << i.size() << ',' << i.format() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPolygon & p)
    {
        return os << '(' << p.size() << " points)";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QBitmap & b)
    {
        return os << '(' << b.size() << ',' << b.devType() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRegion & r)
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
        QVector<QRect> rv = r.rects();
        for(QVector<QRect>::const_iterator it = rv.begin(); it != rv.end();)
        {
            os << *it;
            it++;
            if(it != rv.end())
                os << ';';
        }
#else
        for(QRegion::const_iterator it = r.begin(); it != r.end();)
        {
            os << *it;
            it++;
            if(it != r.end())
                os << ';';
        }
#endif
        return os;
    }

    QENUM_OSTREAM(Qt::CursorShape);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QCursor & c)
    {
        return os << '(' << c.shape() << ',' << c.hotSpot() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QKeySequence & k)
    {
        return os << k.toString();
    }
    QENUM_OSTREAM(Qt::PenStyle);
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPen & p)
    {
        return os << '(' << p.color() << ',' << p.style() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QTextLength & t)
    {
        return os << '(' << t.type() << ',' << t.rawValue() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QTextFormat & t)
    {
        return os << '(' << t.type() << ',' << t.objectIndex() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMatrix & t)
    {
        return os << '(' << t.m11() << ',' << t.m12() << '/' << t.m21() << ',' << t.m22() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QTransform & t)
    {
        return os << '(' << t.type() << '=' << t.m11() << ',' << t.m12() << '/' << t.m21() << ',' << t.m22() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QMatrix4x4 & t)
    {
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QVector2D & t)
    {
        return os << '(' << t.x() << ',' << t.y() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QVector3D & t)
    {
        return os << '(' << t.x() << ',' << t.y() << ',' << t.z() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QVector4D & t)
    {
        return os << '(' << t.x() << ',' << t.y() << ',' << t.z() << ',' << t.w() << ')';
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPolygonF & p)
    {
        return os << p.toPolygon();
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QIcon & i)
    {
        if(i.isNull())
            return os << "(null)";
        else
        {
            os << '(' << i.name() << ";cache=" << i.cacheKey();
            if(i.isMask())
                os << ";mask=1";
            for(const QSize & s : i.availableSizes())
                os << ";size=" << s;
            os << ')';
        }
        return os;
    }

}
