#pragma once

#include <sstream>
#include <iomanip>

#include <QtCore/QDateTime>
#include <QtCore/QPoint>
#include <QtCore/QUrl>
#include <QtCore/QLocale>
#include <QtGui/QColor>

namespace std {

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str)
{
    return os << str.toLocal8Bit().constData();
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QStringRef& str)
{
    const QString * p = str.string();
    if(p)
        return os << p->toLocal8Bit().constData();
    else
        return os << "(null)";
}

namespace {
    inline QByteArray bitsToBytes(const QBitArray & bits) 
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

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QBitArray& v)
{
    return os << QString::fromLocal8Bit(bitsToBytes(v).toHex());
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QByteArray& v)
{
    return os << QString::fromLocal8Bit(v.toHex());
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QByteRef& str)
{
    return os << (char) str;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QDate & date)
{
    return os << qPrintable(date.toString(Qt::ISODate));
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QTime& t)
{
    return os << qPrintable(t.toString(Qt::ISODate));
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QDateTime& date)
{
    return os << qPrintable(date.toString(Qt::ISODate));
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QLocale& l)
{
    return os << l.name();
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QColor& color)
{
    std::stringstream buf;
    buf << "#";
    buf << std::hex << std::setw(2) << std::setfill('0') << color.red();
    buf << std::hex << std::setw(2) << std::setfill('0') << color.green();
    buf << std::hex << std::setw(2) << std::setfill('0') << color.blue();
    if ( color.alpha() < 255)
        buf << std::hex << std::setw(2) << std::setfill('0') << color.alpha();
    std::string ssStr;
    return os << buf.str();
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSize & size)
{
    return os << "(" << size.width() << ", " << size.height() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QSizeF & size)
{
    return os << "(" << size.width() << ", " << size.height() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRect & r)
{
    return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QRectF & r)
{
    return os << "(" << r.left() << ", " << r.top() << " x " << r.width() << ", " << r.height() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPoint & p)
{
    return os << "(" << p.x() << ", " << p.y() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QPointF & p)
{
    return os << "(" << p.x() << ", " << p.y() << ")" << std::endl;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QChar & c)
{
    return os << "0x" << std::hex << c.unicode() << std::dec;
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QUrl & url)
{
    return os  << url.toString();
}

extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QVariant & v);

template<typename T>
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QList<T> & list)
{
    for(typename QList<T>::const_iterator it = list.begin(); it != list.end();)
    {
        os << (const T &)(*it);
        it++;
        if(it != list.end())
            os << ';';
    }
    return os;
}

} // namespace std
