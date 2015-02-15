#pragma once

#include <sstream>
#include <iomanip>

namespace sgi {

inline QString fromLocal8Bit(const std::string & str)
{
    return QString::fromLocal8Bit(str.data(), str.size());
}

inline std::string toLocal8Bit(const QString & str)
{
    QByteArray qba = str.toLocal8Bit();
    return std::string(qba.constData(), qba.size());
}

inline std::string getObjectTypename(const QObject * object)
{
    const QMetaObject * meta = object?object->metaObject():NULL;
    return meta?std::string(meta->className()):"(null)";
}

inline std::string getObjectName(const QObject * object)
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

inline std::string getObjectNameAndType(const QObject * object)
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

} // namespace sgi
