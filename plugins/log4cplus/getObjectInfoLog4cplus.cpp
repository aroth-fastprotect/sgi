#include "stdafx.h"
#include "getObjectInfoLog4cplus.h"

#include "SGIItemLog4cplus"

#include <sstream>
#include <sgi/helpers/rtti>

namespace sgi {

namespace log4cplus_plugin {

GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectBase)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectLogger)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectAppender)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectBase)
GET_OBJECT_PATH_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectLogger)

std::string getObjectNameImpl<Log4cplusObjectBase>::process()
{
    Log4cplusObjectBase * object_ptr = getObject<Log4cplusObjectBase,SGIItemLog4cplus>();
    std::string ret;
    std::stringstream ss;
    ss << "Log4cplusObjectBase(" << (void*)object_ptr << ")";
    ret = ss.str();
    return ret;
}

std::string getObjectNameImpl<Log4cplusObjectLogger>::process()
{
    std::string ret;
    Log4cplusObjectLogger * object_ptr = getObject<Log4cplusObjectLogger,SGIItemLog4cplus>();
    if(object_ptr->isRoot())
        ret = "log4cplus::Logger::Root";
    else
        ret = object_ptr->name();
    return ret;
}

std::string getObjectNameImpl<Log4cplusObjectAppender>::process()
{
    Log4cplusObjectAppender * object_ptr = getObject<Log4cplusObjectAppender,SGIItemLog4cplus>();
    log4cplus::SharedAppenderPtr object = object_ptr->object();
    std::string ret = object->getName();
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
std::string getObjectTypeImpl<Log4cplusObjectBase>::process()
{
    Log4cplusObjectBase * object_ptr = getObject<Log4cplusObjectBase,SGIItemLog4cplus>();
    return helpers::getRTTITypename(object_ptr);
}

//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameImpl
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// getObjectSuggestedFilenameExtensionImpl
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// getObjectFilenameFiltersImpl
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// getObjectPathImpl
//--------------------------------------------------------------------------------

SGIItemBasePtrPath getObjectPathImpl<Log4cplusObjectLogger>::process()
{
    Log4cplusObjectLogger * object_ptr = getObject<Log4cplusObjectLogger,SGIItemLog4cplus>();
    log4cplus::Logger object = object_ptr->object();
    SGIItemBasePtrPath ret;
    log4cplus::Logger parent = object;
    bool isRoot = false;
    do
    {
        SGIHostItemLog4cplus pathHostItem(new Log4cplusObjectLogger(parent));
        SGIItemBasePtr pathItem;
        if(_hostInterface->generateItem(pathItem, &pathHostItem))
            ret.push_back(pathItem.get());

        const Log4cplusLoggerCompare & compareObject = (const Log4cplusLoggerCompare&)parent;
        isRoot = compareObject.equal(log4cplus::Logger::getRoot());
        if(!isRoot)
            parent = parent.getParent();
    }
    while(!isRoot);
    std::reverse(ret.begin(), ret.end());
    return ret;
}


} // namespace log4cplus_plugin
} // namespace sgi
