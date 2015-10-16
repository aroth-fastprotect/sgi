// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

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

namespace sgi {
    namespace qt_helpers {

const static int metaTypeQtSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtSGIItem>("sgi::qt_helpers::QtSGIItem");
const static int metaTypeQtMenuSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtMenuSGIItem>("sgi::qt_helpers::QtMenuSGIItem");
const static int metaTypeQtTableSGIItemType = qRegisterMetaType<sgi::qt_helpers::QtTableSGIItem>("sgi::qt_helpers::QtTableSGIItem");

QString fromLocal8Bit(const std::string & str)
{
    return QString::fromLocal8Bit(str.data(), str.size());
}

std::string toLocal8Bit(const QString & str)
{
    QByteArray qba = str.toLocal8Bit();
    return std::string(qba.constData(), qba.size());
}

std::string getObjectTypename(const QObject * object)
{
    const QMetaObject * meta = object?object->metaObject():NULL;
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
                buf << ' ' << toLocal8Bit(name);
        }
        ret = buf.str();
    }
    else
    {
        ret = object?toLocal8Bit(object->objectName()):std::string("(null)");
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
        default: os << v.toString(); break;
        }
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str)
    {
        return os << str.toLocal8Bit().constData();
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QStringRef& str)
    {
        const QString * p = str.string();
        if(p)
            return os << p->toLocal8Bit().constData();
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
        return os << "(" << size.width() << ", " << size.height() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizeF & size)
    {
        return os << "(" << size.width() << ", " << size.height() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRect & r)
    {
        return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRectF & r)
    {
        return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPoint & p)
    {
        return os << "(" << p.x() << ", " << p.y() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPointF & p)
    {
        return os << "(" << p.x() << ", " << p.y() << ")" << std::endl;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QChar & c)
    {
        return os << "0x" << std::hex << c.unicode() << std::dec;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QUrl & url)
    {
        return os << url.toString();
    }
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizePolicy::Policy & p)
    {
        switch(p)
        {
        case QSizePolicy::Fixed: os << "Fixed"; break;
        case QSizePolicy::Minimum: os << "Minimum"; break;
        case QSizePolicy::Maximum: os << "Maximum"; break;
        case QSizePolicy::Preferred: os << "Preferred"; break;
        case QSizePolicy::MinimumExpanding: os << "MinimumExpanding"; break;
        case QSizePolicy::Expanding: os << "Expanding"; break;
        case QSizePolicy::Ignored: os << "Ignored"; break;
        default: os << (int)p; break;
        }
        return os;
    }
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizePolicy & sp)
    {
        return os << "(" << sp.horizontalPolicy() << ", " << sp.verticalPolicy() << ")";
    }
}