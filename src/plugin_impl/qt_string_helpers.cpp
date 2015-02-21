// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/helpers/qt_string_helpers>

#include <sstream>
#include <iomanip>
#include <QObject>
#include <QMetaObject>

namespace sgi {
    namespace qt_helpers {

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

std::string getObjectName(const QObject * object)
{
    std::string ret = object?toLocal8Bit(object->objectName()):std::string("(null)");
    if(ret.empty())
    {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    return ret;
}

std::string getObjectNameAndType(const QObject * object)
{
std::string name = object?toLocal8Bit(object->objectName()):std::string("(null)");
    std::stringstream buf;
    if(name.empty())
        buf << (void*)object;
    else
        buf << name;
    buf << " (" << getObjectTypename(object) << ")";
    std::string ret = buf.str();
    return ret;
}

    } // namespace qt_helpers
} // namespace sgi
