#include "stdafx.h"
#include "getObjectInfoVsg.h"

#include "SGIItemVsg"

#include <sstream>
#include <sgi/helpers/rtti>
#include <sgi/helpers/vsg>

using namespace sgi::vsg_helpers;

namespace sgi {

namespace vsg_plugin {

GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(vsg::Object)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(vsg::Object)
GET_OBJECT_PATH_IMPL_DECLARE_AND_REGISTER(vsg::Object)

std::string getObjectNameImpl<vsg::Object>::process()
{
    vsg::Object * object = getObject<vsg::Object,SGIItemVsg>();
    std::string ret = getObjectName(object);
    return ret;
}

//--------------------------------------------------------------------------------
// getObjectTypeImpl
//--------------------------------------------------------------------------------
std::string getObjectTypeImpl<vsg::Object>::process()
{
    vsg::Object * object_ptr = getObject<vsg::Object,SGIItemVsg>();
    return getObjectTypename(object_ptr);
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

SGIItemBasePtrPath getObjectPathImpl<vsg::Object>::process()
{
    vsg::Object * object_ptr = getObject<vsg::Object,SGIItemVsg>();
    SGIItemBasePtrPath ret;
    return ret;
}


} // namespace vsg_plugin
} // namespace sgi