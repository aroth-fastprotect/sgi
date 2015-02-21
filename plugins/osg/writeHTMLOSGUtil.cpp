#include <ostream>
#include <sstream>
#include <iomanip>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGUtil.h"

#include <osgUtil/RenderStage>
#include <osgUtil/RenderLeaf>

#include "SGIItemOsg"
#include <sgi/helpers/osg>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgUtil::RenderBin)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgUtil::RenderStage)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgUtil::RenderLeaf)

using namespace sgi::osg_helpers;

class RenderBinAccess : public osgUtil::RenderBin
{
public:
    bool isSorted() const { return _sorted; }
};


bool writePrettyHTMLImpl<osgUtil::RenderBin>::process(std::basic_ostream<char>& os)
{
    osgUtil::RenderBin * object = getObject<osgUtil::RenderBin,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            RenderBinAccess * access = (RenderBinAccess*)object;

            // add remaining osgUtil::RenderBin properties
            os << "<tr><td>binNum</td><td>" << object->getBinNum() << "</td></tr>" << std::endl;
            os << "<tr><td>sortMode</td><td>" << object->getSortMode() << "</td></tr>" << std::endl;
            os << "<tr><td>isSorted</td><td>" << (access->isSorted()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>sortCallback</td><td>" << getObjectNameAndType(object->getSortCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>drawCallback</td><td>" << getObjectNameAndType(object->getDrawCallback()) << "</td></tr>" << std::endl;

            if(_table)
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

bool writePrettyHTMLImpl<osgUtil::RenderStage>::process(std::basic_ostream<char>& os)
{
    osgUtil::RenderStage * object = getObject<osgUtil::RenderStage,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining osgUtil::RenderStage properties
            os << "<tr><td>drawBuffer</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(object->getDrawBuffer()) << "</td></tr>" << std::endl;
            os << "<tr><td>drawBufferApplyMask</td><td>" << (object->getDrawBufferApplyMask()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>readBuffer</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(object->getReadBuffer()) << "</td></tr>" << std::endl;
            os << "<tr><td>readBufferApplyMask</td><td>" << (object->getReadBufferApplyMask()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>clearMask</td><td>0x" << std::hex << object->getClearMask() << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>clearColor</td><td>" << vec4fToHtmlColor(object->getClearColor()) << "</td></tr>" << std::endl;
            os << "<tr><td>clearAccum</td><td>" << vec4fToHtmlColor(object->getClearAccum()) << "</td></tr>" << std::endl;
            os << "<tr><td>clearDepth</td><td>" << object->getClearDepth() << "</td></tr>" << std::endl;
            os << "<tr><td>clearStencil</td><td>" << object->getClearStencil() << "</td></tr>" << std::endl;
            os << "<tr><td>cameraRequiresSetUp</td><td>" << (object->getCameraRequiresSetUp()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>imageReadPixelFormat</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(object->getImageReadPixelFormat()) << "</td></tr>" << std::endl;
            os << "<tr><td>imageReadPixelDataType</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLConstant>(object->getImageReadPixelDataType()) << "</td></tr>" << std::endl;
            os << "<tr><td>disableFboAfterRender</td><td>" << (object->getDisableFboAfterRender()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
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
bool writePrettyHTMLImpl<osgUtil::RenderLeaf>::process(std::basic_ostream<char>& os)
{
    osgUtil::RenderLeaf * object = getObject<osgUtil::RenderLeaf,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining osgUtil::RenderLeaf properties
            os << "<tr><td>depth</td><td>" << object->_depth << "</td></tr>" << std::endl;
            os << "<tr><td>dynamic</td><td>" << (object->_dynamic?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>traversalNumber</td><td>" << object->_traversalNumber << "</td></tr>" << std::endl;

            os << "<tr><td>projection</td><td>";
            if(object->_projection.valid())
                writePrettyHTML(os, *object->_projection.get(), MatrixUsageTypePerspective, (osg::Node*)NULL);
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;

            os << "<tr><td>modelview</td><td>";
            if(object->_modelview.valid())
                writePrettyHTML(os, *object->_modelview.get(), MatrixUsageTypeModelView, (osg::Node*)NULL);
            else
                os << "&lt;null&gt;";
            os << "</td></tr>" << std::endl;


            if(_table)
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
