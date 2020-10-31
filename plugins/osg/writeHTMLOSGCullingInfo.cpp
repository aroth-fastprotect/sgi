#include "stdafx.h"
#include <ostream>
#include <sstream>
#include "writeHTMLOSG.h"
#include "SGIItemOsg"
#include "ObjectLoggerOSG.h"

#include <osg/Camera>
#include <sgi/helpers/osg>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(CullingInfo)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(CullingNodeInfo)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(CullingInfoForCamera)

using namespace sgi::osg_helpers;

extern void writePrettyHTML(std::basic_ostream<char>& os, const osg::CullStack * object_);

bool writePrettyHTMLImpl<CullingInfo>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    CullingInfo * object = getObject<CullingInfo,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>node</td><td>" << osg_helpers::getObjectNameAndType(object->node()) << "</td></tr>" << std::endl;
			os << "<tr><td>path</td><td>";
			for (const CullingNodeInfoPath& path : object->pathlist())
			{
				os << "<ol>";
				for (const auto & ptr : path)
				{
					const CullingNodeInfo& info = (*ptr);
					os << "<li>" << (&info) << "=" << osg_helpers::getObjectNameAndType(info.node) << "</li>";
				}
				os << "</ol>";
			}
			
			os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            unsigned pathnum = itemNumber();
            const CullingInfo::CullingNodeInfoPathList & pathlist = object->pathlist();
            if (pathnum < pathlist.size())
            {
                os << "<ol>";
                const CullingNodeInfoPath & path = pathlist[pathnum];
                for (const CullingNodeInfoPtr & cullinfo : path)
                {
                    os << "<li>" << getObjectNameAndType(cullinfo->node) << "</li>";
                }
                os << "</ol>";
            }
            else
                os << "<i>Path is unavailable</i>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

void writePrettyHTMLImpl_CullingNodeInfo_Info(std::basic_ostream<char>& os, const CullingNodeInfo::Info & info)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
	os << "<tr><td>this</td><td>" << (void*)&info << "</td></tr>" << std::endl;
    os << "<tr><td>boundingSphere</td><td>" << info.boundingSphere << "</td></tr>" << std::endl;
    os << "<tr><td>boundingSphereComputed</td><td>" << (info.boundingSphereComputed ? "true" : "false") << "</td></tr>" << std::endl;
    os << "<tr><td>cullingMask</td><td>" << cullingMaskToString(info.cullingMask) << "</td></tr>" << std::endl;
    os << "<tr><td>cullStack</td><td>";
    writePrettyHTML(os, &info.cullStack);
    os << "</td></tr>" << std::endl;
    os << "</table>" << std::endl;
}

bool writePrettyHTMLImpl<CullingNodeInfo>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    CullingNodeInfo * object = getObject<CullingNodeInfo, SGIItemOsg>();
    switch (itemType())
    {
        case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>node</td><td>" << osg_helpers::getObjectNameAndType(object->node) << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCullingInfoBefore:
        writePrettyHTMLImpl_CullingNodeInfo_Info(os, object->before);
        ret = true;
        break;
    case SGIItemTypeCullingInfoAfter:
        writePrettyHTMLImpl_CullingNodeInfo_Info(os, object->after);
        ret = true;
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<CullingInfoForCamera>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    CullingInfoForCamera * object = getObject<CullingInfoForCamera, SGIItemOsg>();
    switch (itemType())
    {
        case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>camera</td><td>" << osg_helpers::getObjectNameAndType(object->camera()) << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
        default:
            ret = callNextHandler(os);
            break;
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi

