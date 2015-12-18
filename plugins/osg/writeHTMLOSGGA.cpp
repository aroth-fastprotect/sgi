#include "stdafx.h"
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
#include <sgi/helpers/string>

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgGA::EventHandler)
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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::KeySymbol & k)
{
	switch (k)
	{
	case 0: os << "zero"; break;
	case osgGA::GUIEventAdapter::KEY_Space: os << "Space"; break;
	default: os << "0x" << std::hex << (int)k; break;
	}
	return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::MouseButtonMask & m)
{
	if(m == 0)
		os << "zero";
	else
	{
		std::vector<std::string> buttons;
		bool needComma = false;
		if (m & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			buttons.push_back("left");
		if (m & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
			buttons.push_back("middle");
		if (m & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
			buttons.push_back("right");
		os << helpers::joinStrings(buttons, ',');
	}
	return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::ModKeyMask & m)
{
	if (m == 0)
		os << "zero";
	else
	{
		std::vector<std::string> mods;
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT)
			mods.push_back("left-shift");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT)
			mods.push_back("right-shift");
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL)
			mods.push_back("left-ctrl");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL)
			mods.push_back("right-ctrl");
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_ALT)
			mods.push_back("left-alt");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT)
			mods.push_back("right-alt");
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_META)
			mods.push_back("left-meta");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_META)
			mods.push_back("right-meta");
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_SUPER)
			mods.push_back("left-super");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_SUPER)
			mods.push_back("right-super");
		if (m & osgGA::GUIEventAdapter::MODKEY_LEFT_HYPER)
			mods.push_back("left-hyper");
		if (m & osgGA::GUIEventAdapter::MODKEY_RIGHT_HYPER)
			mods.push_back("right-hyper");
		if (m & osgGA::GUIEventAdapter::MODKEY_NUM_LOCK)
			mods.push_back("num-lock");
		if (m & osgGA::GUIEventAdapter::MODKEY_CAPS_LOCK)
			mods.push_back("caps-lock");
		os << helpers::joinStrings(mods, ',');
	}
	return os;
}


bool writePrettyHTMLImpl<osgGA::EventHandler>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgGA::EventHandler * object = getObject<osgGA::EventHandler, SGIItemOsg, DynamicCaster>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::NodeCallback properties first
            callNextHandler(os);

            osg::ref_ptr<osg::ApplicationUsage> usage = new osg::ApplicationUsage;
            object->getUsage(*usage);
            {
                os << "<tr><td>cmd line</td><td><pre>";
                usage->write(os, osg::ApplicationUsage::COMMAND_LINE_OPTION, 132, true);
                os << "</pre></td></tr>" << std::endl;
            }
            {
                os << "<tr><td>environmental variables</td><td><pre>";
                usage->write(os, osg::ApplicationUsage::ENVIRONMENTAL_VARIABLE, 132, true);
                os << "</pre></td></tr>" << std::endl;
            }
            {
                os << "<tr><td>keyboard/mouse bindings</td><td><pre>";
                usage->write(os, osg::ApplicationUsage::KEYBOARD_MOUSE_BINDING, 132, true);
                os << "</pre></td></tr>" << std::endl;
            }

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

            os << "<tr><td>type</td><td>" << object->getEventType() << "</td></tr>" << std::endl;
            os << "<tr><td>time</td><td>" << object->getTime() << "</td></tr>" << std::endl;
            os << "<tr><td>handled</td><td>" << (object->getHandled()?"true":"false") << "</td></tr>" << std::endl;
			os << "<tr><td>multi touch event</td><td>" << (object->isMultiTouchEvent() ? "true" : "false") << "</td></tr>" << std::endl;
			os << "<tr><td>num pointer data</td><td>" << object->getNumPointerData() << "</td></tr>" << std::endl;

            switch(object->getEventType())
            {
            case osgGA::GUIEventAdapter::NONE:
                break;
			case osgGA::GUIEventAdapter::SCROLL:
				os << "<tr><td>scrollingMotion</td><td>" << object->getScrollingMotion() << "</td></tr>" << std::endl;
				os << "<tr><td>scrollingDelta</td><td>" << object->getScrollingDeltaX() << "," << object->getScrollingDeltaY() << "</td></tr>" << std::endl;
				// fall through
			case osgGA::GUIEventAdapter::PUSH:
            case osgGA::GUIEventAdapter::RELEASE:
            case osgGA::GUIEventAdapter::DOUBLECLICK:
            case osgGA::GUIEventAdapter::DRAG:
            case osgGA::GUIEventAdapter::MOVE:
                os << "<tr><td>button</td><td>" << (osgGA::GUIEventAdapter::MouseButtonMask)object->getButton() << "</td></tr>" << std::endl;
                os << "<tr><td>buttonmask</td><td>" << (osgGA::GUIEventAdapter::MouseButtonMask)object->getButtonMask() << "</td></tr>" << std::endl;
                os << "<tr><td>modkeymask</td><td>" << (osgGA::GUIEventAdapter::ModKeyMask)object->getModKeyMask() << "</td></tr>" << std::endl;
                os << "<tr><td>x</td><td>" << object->getX() << " min=" << object->getXmin() << " max=" << object->getXmax() << "</td></tr>" << std::endl;
                os << "<tr><td>y</td><td>" << object->getY() << " min=" << object->getYmin() << " max=" << object->getYmax() << "</td></tr>" << std::endl;
                break;
            case osgGA::GUIEventAdapter::KEYDOWN:
            case osgGA::GUIEventAdapter::KEYUP:
                os << "<tr><td>modkeymask</td><td>" << (osgGA::GUIEventAdapter::ModKeyMask)object->getModKeyMask() << "</td></tr>" << std::endl;
                os << "<tr><td>key</td><td>" << (osgGA::GUIEventAdapter::KeySymbol)object->getKey() << "</td></tr>" << std::endl;
                break;
            case osgGA::GUIEventAdapter::RESIZE:
                os << "<tr><td>windowX</td><td>" << object->getWindowX() << "</td></tr>" << std::endl;
                os << "<tr><td>windowY</td><td>" << object->getWindowY() << "</td></tr>" << std::endl;
                os << "<tr><td>windowWidth</td><td>" << object->getWindowWidth() << "</td></tr>" << std::endl;
                os << "<tr><td>windowHeight</td><td>" << object->getWindowHeight() << "</td></tr>" << std::endl;
                break;
			case osgGA::GUIEventAdapter::PEN_PRESSURE:
			case osgGA::GUIEventAdapter::PEN_ORIENTATION:
			case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
			case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
				break;
			case osgGA::GUIEventAdapter::FRAME:
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

            os << "<tr><td>node</td><td>" << getObjectNameAndType(const_cast<osgGA::CameraManipulator*>(object)->getNode(), true) << "</td></tr>" << std::endl;

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

