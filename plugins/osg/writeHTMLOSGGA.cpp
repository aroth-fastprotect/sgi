#include <ostream>
#include <sstream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGGA.h"

#include <osg/Version>
#include <osgGA/GUIEventHandler>
#include <osgGA/CameraManipulator>

#include <osgUtil/SceneView>

#include "SGIItemOsg"

#include <sgi/helpers/osg>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgGA::GUIEventHandler)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgGA::GUIEventAdapter)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgGA::CameraManipulator)

WRITE_PRETTY_HTML_IMPL_REGISTER(osgUtil::SceneView)

using namespace sgi::osg_helpers;

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osgUtil::SceneView::FusionDistanceMode t)
{
    switch(t)
    {
    case osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE: os << "use value"; break;
    case osgUtil::SceneView::PROPORTIONAL_TO_SCREEN_DISTANCE: os << "proportional"; break;
    default: os << (int)t; break;
    }
    return os;
}


bool writePrettyHTMLImpl<osgGA::GUIEventHandler>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgGA::GUIEventHandler * object = getObject<osgGA::GUIEventHandler,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::NodeCallback properties first
            callNextHandler(os);

            // add osg::Drawable::EventCallback properties second, but there's nothing new
            // so drop it
            // writePrettyHTML(os, (const osg::Drawable::EventCallback*)object, false);

            // add remaining GUIEventHandler properties
#if OSG_VERSION_LESS_THAN(3,3,1)
            os << "<tr><td>ignored handled event mask</td><td>" << std::hex
                << (object?object->getIgnoreHandledEventsMask():0) << std::dec << "</td></tr>" << std::endl;
#endif

            if(object)
            {
                osg::ref_ptr<osg::ApplicationUsage> usage = new osg::ApplicationUsage;
                object->getUsage(*usage);
                {
                    std::stringstream str;
                    usage->write(str, osg::ApplicationUsage::COMMAND_LINE_OPTION, 132, true);
                    os << "<tr><td>cmd line</td><td><pre>" << str.str() << "</pre></td></tr>" << std::endl;
                }
                {
                    std::stringstream str;
                    usage->write(str, osg::ApplicationUsage::ENVIRONMENTAL_VARIABLE, 132, true);
                    os << "<tr><td>environmental variables</td><td><pre>" << str.str() << "</pre></td></tr>" << std::endl;
                }
                {
                    std::stringstream str;
                    usage->write(str, osg::ApplicationUsage::KEYBOARD_MOUSE_BINDING, 132, true);
                    os << "<tr><td>keyboard/mouse bindings</td><td><pre>" << str.str() << "</pre></td></tr>" << std::endl;
                }
            }
            else
            {
                os << "<tr><td>cmd line</td><td></td></tr>" << std::endl;
                os << "<tr><td>environmental variables</td><td></td></tr>" << std::endl;
                os << "<tr><td>keyboard/mouse bindings</td><td></td></tr>" << std::endl;
            }
            os << "</pre></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgGA::GUIEventAdapter>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgGA::GUIEventAdapter * object = getObject<osgGA::GUIEventAdapter,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::NodeCallback properties first
            callNextHandler(os);

            os << "<tr><td>type</td><td>" << (object?object->getEventType():osgGA::GUIEventAdapter::NONE) << "</td></tr>" << std::endl;
            os << "<tr><td>time</td><td>" << (object?object->getTime():0.0) << "</td></tr>" << std::endl;
            os << "<tr><td>handled</td><td>" << (object?(object->getHandled()?"true":"false"):"&lt;null&gt;") << "</td></tr>" << std::endl;

            switch(object->getEventType())
            {
            case osgGA::GUIEventAdapter::NONE:
                break;
            case osgGA::GUIEventAdapter::PUSH:
            case osgGA::GUIEventAdapter::RELEASE:
            case osgGA::GUIEventAdapter::DOUBLECLICK:
            case osgGA::GUIEventAdapter::DRAG:
            case osgGA::GUIEventAdapter::MOVE:
            case osgGA::GUIEventAdapter::SCROLL:
                os << "<tr><td>button</td><td>" << (object?object->getButton():0) << "</td></tr>" << std::endl;
                os << "<tr><td>buttonmask</td><td>" << (object?(osgGA::GUIEventAdapter::MouseButtonMask)object->getButtonMask():(osgGA::GUIEventAdapter::MouseButtonMask)0) << "</td></tr>" << std::endl;
                os << "<tr><td>modkeymask</td><td>" << (object?(osgGA::GUIEventAdapter::ModKeyMask)object->getModKeyMask():(osgGA::GUIEventAdapter::ModKeyMask)0) << "</td></tr>" << std::endl;
                os << "<tr><td>x</td><td>" << (object?object->getX():0.0f) << " min=" << (object?object->getXmin():0.0f) << " max=" << (object?object->getXmax():0.0f) << "</td></tr>" << std::endl;
                os << "<tr><td>y</td><td>" << (object?object->getY():0.0f) << " min=" << (object?object->getYmin():0.0f) << " max=" << (object?object->getYmax():0.0f) << "</td></tr>" << std::endl;
                break;
            case osgGA::GUIEventAdapter::KEYDOWN:
            case osgGA::GUIEventAdapter::KEYUP:
                os << "<tr><td>modkeymask</td><td>" << (object?(osgGA::GUIEventAdapter::ModKeyMask)object->getModKeyMask():(osgGA::GUIEventAdapter::ModKeyMask)0) << "</td></tr>" << std::endl;
                os << "<tr><td>key</td><td>" << (object?(osgGA::GUIEventAdapter::KeySymbol)object->getKey():(osgGA::GUIEventAdapter::KeySymbol)0) << "</td></tr>" << std::endl;
                break;
            case osgGA::GUIEventAdapter::RESIZE:
                os << "<tr><td>windowX</td><td>" << (object?object->getWindowX():0) << "</td></tr>" << std::endl;
                os << "<tr><td>windowY</td><td>" << (object?object->getWindowY():0) << "</td></tr>" << std::endl;
                os << "<tr><td>windowWidth</td><td>" << (object?object->getWindowWidth():0) << "</td></tr>" << std::endl;
                os << "<tr><td>windowHeight</td><td>" << (object?object->getWindowHeight():0) << "</td></tr>" << std::endl;
                break;
            case osgGA::GUIEventAdapter::FRAME:
            case osgGA::GUIEventAdapter::PEN_PRESSURE:
            case osgGA::GUIEventAdapter::PEN_ORIENTATION:
            case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
            case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
            case osgGA::GUIEventAdapter::CLOSE_WINDOW:
            case osgGA::GUIEventAdapter::QUIT_APPLICATION:
            case osgGA::GUIEventAdapter::USER:
                break;
            default:
                break;
            }

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

bool writePrettyHTMLImpl<osgGA::CameraManipulator>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    bool brief = true;
    osgGA::CameraManipulator * object = getObject<osgGA::CameraManipulator,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgGA::GUIEventHandler properties first
            callNextHandler(os);

            // add remaining CameraManipulator properties
            os << "<tr><td>coordinate frame callback</td><td>" << getObjectNameAndType(object->getCoordinateFrameCallback()) << "</td></tr>" << std::endl;

            os << "<tr><td>matrix</td><td>";
            writePrettyHTML(os, object->getMatrix(), MatrixUsageTypeModelView, (const osg::Node*)NULL);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>inverse matrix</td><td>";
            writePrettyHTML(os, object->getInverseMatrix(), MatrixUsageTypeModelView, (const osg::Node*)NULL);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>fusion distance mode</td><td>" << object->getFusionDistanceMode() << "</td></tr>" << std::endl;
            os << "<tr><td>fusion distance</td><td>" << object->getFusionDistanceValue() << "</td></tr>" << std::endl;

            os << "<tr><td>intersect traversal mask</td><td>" << castToEnumValueString<osgNodeMask>(object->getIntersectTraversalMask()) << "</td></tr>" << std::endl;

            os << "<tr><td>node</td><td>" << getObjectNameAndType(const_cast<osgGA::CameraManipulator*>(object)->getNode()) << "</td></tr>" << std::endl;

            os << "<tr><td>auto home position</td><td>" << (object->getAutoComputeHomePosition()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>home position</td><td>";
            osg::Vec3d homeEye, homeCenter, homeUp;
            object->getHomePosition(homeEye, homeCenter, homeUp);
            os << "eye&nbsp;"; writePrettyPositionHTML(os, homeEye, (const osg::Node *)NULL); os << "<br/>";
            os << "center&nbsp;"; writePrettyPositionHTML(os, homeCenter, (const osg::Node *)NULL); os << "<br/>";
            os << "up&nbsp;"; writePrettyHTML(os, homeUp); os << "<br/>";
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

bool writePrettyHTMLImpl<osgUtil::SceneView>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgUtil::SceneView * object = getObject<osgUtil::SceneView,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osgGA::GUIEventHandler properties first
            callNextHandler(os);

            // add remaining SceneView properties
            os << "<tr><td>prioritize textures</td><td>" << (object->getPrioritizeTextures()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>frame stamp</td><td>" << getObjectNameAndType(object->getFrameStamp()) << "</td></tr>" << std::endl;
            os << "<tr><td>compute stereo matrices callback</td><td>" << getObjectNameAndType(object->getComputeStereoMatricesCallback()) << "</td></tr>" << std::endl;

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

