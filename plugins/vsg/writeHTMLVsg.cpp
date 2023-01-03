#include "stdafx.h"
#include <ostream>
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <log4cplus/version.h>
#include <log4cplus/hierarchy.h>
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include "writeHTMLVsg.h"
#include "SGIItemVsg"
#include <sgi/plugins/SGIHostItemVsg.h>

#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace vsg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(vsg::Object)

bool writePrettyHTMLImpl<vsg::Object>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    vsg::Object * object_ptr = item<SGIItemVsg>()->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>type</td><td>" << helpers::getRTTITypename_html(object_ptr) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}


} // namespace vsg_plugin
} // namespace sgi
