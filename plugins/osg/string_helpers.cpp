#include <osg/StateAttribute>
#include <osg/Texture>
#include <sgi/plugins/SGIItemBase.h>
#include "osg_string_helpers"

namespace sgi {
    namespace osg_plugin {

std::string glValueName(unsigned n)
{
    std::string ret;
    if(n & osg::StateAttribute::ON)
        ret = "ON";
    else
        ret = "OFF";
    if(n & osg::StateAttribute::OVERRIDE)
        appendToCSVString(ret, "OVERRIDE");
    if(n & osg::StateAttribute::PROTECTED)
        appendToCSVString(ret, "PROTECTED");
    if(n & osg::StateAttribute::INHERIT)
        appendToCSVString(ret, "INHERIT");
    else
        appendToCSVString(ret, "NOINHERIT");
    std::stringstream ss;
    ss << ret << "(0x" << std::hex << n << ')';
    ret = ss.str();
    return ret;
}

std::string glOverrideValueName(unsigned n)
{
    std::string ret;
    if(n == 0)
        ret = "NOOVERRIDE";
    else
    {
        if(n & osg::StateAttribute::ON)
            ret = "Override disabled";
        else
        {
            if(n & osg::StateAttribute::OVERRIDE)
                appendToCSVString(ret, "OVERRIDE");
            if(n & osg::StateAttribute::INHERIT)
                appendToCSVString(ret, "INHERIT");
            if(n & osg::StateAttribute::PROTECTED)
                appendToCSVString(ret, "PROTECTED");
            if(ret.empty())
            {
                std::stringstream ss;
                ss << "0x" << std::hex << n;
                ret = ss.str();
            }
        }
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi

