#pragma once

#include <sstream>
#include <iomanip>

#include <osg/Vec4f>
#include <sgi/helpers/rtti>

namespace osg {
    class Object;
    class EllipsoidModel;
}

namespace sgi {

/** Makes an HTML color ("#rrggbb" or "#rrggbbaa") from an OSG color. */
template<typename T>
inline std::string vec4fToHtmlColor( const T& c )
{
    std::stringstream buf;
    buf << "#";
    buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.r()*255.0f);
    buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.g()*255.0f);
    buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.b()*255.0f);
    if ( c.a() < 1.0f )
        buf << std::hex << std::setw(2) << std::setfill('0') << (int)(c.a()*255.0f);
    std::string ssStr;
    ssStr = buf.str();
    return ssStr;
}

// create an empty enum for the string representation of the
// osg::NodeMask (which is a typedef to unsigned int).
enum osgNodeMask { };
// extra type for GL constants
enum GLConstant { };

// extra type for GL modes values
enum GLModeValue { };
enum GLModeOverrideValue { };

inline std::string getObjectTypename(const osg::Referenced * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

inline std::string getObjectTypename(const osg::Observer * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

inline std::string getObjectTypename(const osg::Object * object)
{
    return object?(std::string(object->libraryName()) + std::string("::") + std::string(object->className())):"(null)";
}

inline std::string getObjectName(const osg::Referenced * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << helpers::getRTTITypename_html(object);
        buf << '(';
        buf << (void*)object;
        buf << ')';
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

inline std::string getObjectName(const osg::Observer * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << helpers::getRTTITypename_html(object);
        buf << '(';
        buf << (void*)object;
        buf << ')';
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

inline std::string getObjectName(const osg::Object * object)
{
    std::string ret = object?(object->getName()):"(null)";
    if(ret.empty())
    {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    return ret;
}

inline std::string getObjectNameAndType(const osg::Referenced * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << (void*)object;
        buf << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

inline std::string getObjectNameAndType(const osg::Observer * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << (void*)object;
        buf << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

inline std::string getObjectNameAndType(const osg::Object * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object) << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}
template<typename INT_TYPE>
inline void appendToCSVString(std::string & str, const INT_TYPE & n)
{
    std::stringstream buf;
    if(!str.empty())
        buf << str << ',' << n;
    else
        buf << n;
    str = buf.str();
}

inline sgi::Color osgColor(const osg::Vec4f & c)
{
    return sgi::Color(c.r(), c.g(), c.b(), c.a());
}

inline osg::Vec4f osgColor(const sgi::Color & c)
{
    return osg::Vec4f(c.r, c.g, c.b, c.a);
}

namespace osg_plugin {
std::string glValueName(unsigned n);
std::string glOverrideValueName(unsigned n);

} // namespace osg_plugin

} // namespace sgi
