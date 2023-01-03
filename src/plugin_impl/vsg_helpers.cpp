// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/vsg>

#include <vsg/core/Object.h>
#include <vsg/core/Auxiliary.h>

namespace sgi {

    namespace vsg_helpers {

std::string getObjectTypename(const vsg::Auxiliary * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

std::string getObjectTypename(const vsg::Object * object)
{
    return object?(std::string(object->className())):"(null)";
}


std::string getObjectName(const vsg::Auxiliary * object, bool includeAddr)
{
    std::string ret;
    if(object) {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectName(const vsg::Object * object, bool includeAddr)
{
    std::string ret;
    if(object) {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectNameAndType(const vsg::Auxiliary * object, bool includeAddr)
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

std::string getObjectNameAndType(const vsg::Object * object, bool includeAddr)
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

    } // namespace vsg_helpers
} // namespace sgi
