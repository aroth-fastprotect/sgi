#include "sgi_viewer_base.h"
#include <osg/io_utils>
#include <osg/ImageStream>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/ShapeDrawable>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>


#ifdef SGI_USE_OSGEARTH
#include <osgEarth/Notify>
#include <osgEarth/MapNode>
#include <osgEarth/MapFrame>
#include <osgEarth/Registry>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/GLUtils>
#include <osgEarth/Lighting>
#include <osgEarth/PhongLightingEffect>
#include <osgEarth/NodeUtils>

#include <osgEarthUtil/Ephemeris>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Shadowing>
#endif // SGI_USE_OSGEARTH

#include <sgi/helpers/osg_helper_nodes>
#include <sgi/helpers/osg>

#include <iomanip>
#include <iostream>
#include <algorithm>

#if defined(__linux__)
#include <dlfcn.h>
#include <osgDB/Version>
#endif

namespace std {

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::NotifySeverity & t)
    {
        switch (t)
        {
        case osg::ALWAYS: os << "ALWAYS"; break;
        case osg::FATAL: os << "FATAL"; break;
        case osg::WARN: os << "WARN"; break;
        case osg::NOTICE: os << "NOTICE"; break;
        case osg::INFO: os << "INFO"; break;
        case osg::DEBUG_INFO: os << "DEBUG_INFO"; break;
        case osg::DEBUG_FP: os << "DEBUG_FP"; break;
        default: os << static_cast<int>(t); break;
        }
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BoundingSphere & bs)
    {
        return os << std::setprecision(12) << "{center=" << bs.center() << ";radius=" << bs.radius() << "}";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BoundingBox & bbox)
    {
        return os << std::setprecision(12) << "{center=" << bbox.center() << ";min=" << bbox._min << ";max=" << bbox._max << "}";
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::EventType & t)
    {
        switch (t)
        {
        case osgGA::GUIEventAdapter::NONE: os << "None"; break;
        case osgGA::GUIEventAdapter::PUSH: os << "Push"; break;
        case osgGA::GUIEventAdapter::RELEASE: os << "Release"; break;
        case osgGA::GUIEventAdapter::DOUBLECLICK: os << "DblClk"; break;
        case osgGA::GUIEventAdapter::DRAG: os << "Drag"; break;
        case osgGA::GUIEventAdapter::MOVE: os << "Move"; break;
        case osgGA::GUIEventAdapter::KEYDOWN: os << "KeyDown"; break;
        case osgGA::GUIEventAdapter::KEYUP: os << "KeyUp"; break;
        case osgGA::GUIEventAdapter::FRAME: os << "Frame"; break;
        case osgGA::GUIEventAdapter::RESIZE: os << "Resize"; break;
        case osgGA::GUIEventAdapter::SCROLL: os << "Scroll"; break;
        case osgGA::GUIEventAdapter::PEN_PRESSURE: os << "PenPress"; break;
        case osgGA::GUIEventAdapter::PEN_ORIENTATION: os << "PenOrient"; break;
        case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER: os << "PenProxEnter"; break;
        case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE: os << "PenProxLeave"; break;
        case osgGA::GUIEventAdapter::CLOSE_WINDOW: os << "CloseWnd"; break;
        case osgGA::GUIEventAdapter::QUIT_APPLICATION: os << "Quit"; break;
        case osgGA::GUIEventAdapter::USER: os << "User"; break;
        default: os << static_cast<int>(t); break;
        }
        return os;
    }

#define GUIEventAdapter_Check_MouseButtonMask(mask, modname) \
    if(m & osgGA::GUIEventAdapter::mask) { \
        if(str.empty()) \
            str = modname; \
        else \
            str = str + ("+" modname); \
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::MouseButtonMask & m)
    {
        std::string str;
        GUIEventAdapter_Check_MouseButtonMask(LEFT_MOUSE_BUTTON, "left");
        GUIEventAdapter_Check_MouseButtonMask(MIDDLE_MOUSE_BUTTON, "middle");
        GUIEventAdapter_Check_MouseButtonMask(RIGHT_MOUSE_BUTTON, "right");
        if (str.empty())
            str = "none";
        os << str;
        return os;
    }

#define GUIEventAdapter_Check_ModKeyMask(mask, modname) \
    if(m & osgGA::GUIEventAdapter::mask) { \
        if(str.empty()) \
            str = modname; \
        else \
            str = str + ("+" modname); \
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::KeySymbol & k)
    {
        switch (k)
        {
        case osgGA::GUIEventAdapter::KEY_Shift_L: os << "LShift"; break;
        case osgGA::GUIEventAdapter::KEY_Shift_R: os << "RShift"; break;
        case osgGA::GUIEventAdapter::KEY_Control_L: os << "LCtrl"; break;
        case osgGA::GUIEventAdapter::KEY_Control_R: os << "RCtrl"; break;
        case osgGA::GUIEventAdapter::KEY_Caps_Lock: os << "CapsLock"; break;
        case osgGA::GUIEventAdapter::KEY_Shift_Lock: os << "ShiftLock"; break;
        case osgGA::GUIEventAdapter::KEY_Num_Lock: os << "NumLock"; break;
        case osgGA::GUIEventAdapter::KEY_Meta_L: os << "LMeta"; break;
        case osgGA::GUIEventAdapter::KEY_Meta_R: os << "RMeta"; break;
        case osgGA::GUIEventAdapter::KEY_Alt_L: os << "LAlt"; break;
        case osgGA::GUIEventAdapter::KEY_Alt_R: os << "RAlt"; break;
        case osgGA::GUIEventAdapter::KEY_Super_L: os << "LSuper"; break;
        case osgGA::GUIEventAdapter::KEY_Super_R: os << "RSuper"; break;
        case osgGA::GUIEventAdapter::KEY_Hyper_L: os << "LHyper"; break;
        case osgGA::GUIEventAdapter::KEY_Hyper_R: os << "RHyper"; break;

        case osgGA::GUIEventAdapter::KEY_Left: os << "ArrowLeft"; break;
        case osgGA::GUIEventAdapter::KEY_Right: os << "ArrowRight"; break;
        case osgGA::GUIEventAdapter::KEY_Up: os << "ArrowUp"; break;
        case osgGA::GUIEventAdapter::KEY_Down: os << "ArrowDown"; break;

        case osgGA::GUIEventAdapter::KEY_Home: os << "Home"; break;
        case osgGA::GUIEventAdapter::KEY_Page_Up: os << "PgUp"; break;
        case osgGA::GUIEventAdapter::KEY_Page_Down: os << "PgDw"; break;
        case osgGA::GUIEventAdapter::KEY_End: os << "End"; break;
        case osgGA::GUIEventAdapter::KEY_Begin: os << "Begin"; break;
        case osgGA::GUIEventAdapter::KEY_Period: os << "Period"; break;
        case osgGA::GUIEventAdapter::KEY_Insert: os << "Insert"; break;
        case osgGA::GUIEventAdapter::KEY_Slash: os << "Slash"; break;

        case osgGA::GUIEventAdapter::KEY_Space: os << "Space"; break;
        case osgGA::GUIEventAdapter::KEY_BackSpace: os << "Backspace"; break;
        case osgGA::GUIEventAdapter::KEY_Tab: os << "Tab"; break;
        case osgGA::GUIEventAdapter::KEY_Linefeed: os << "Linefeed"; break;
        case osgGA::GUIEventAdapter::KEY_Clear: os << "Clear"; break;
        case osgGA::GUIEventAdapter::KEY_Return: os << "Return"; break;
        case osgGA::GUIEventAdapter::KEY_Pause: os << "Pause"; break;
        case osgGA::GUIEventAdapter::KEY_Scroll_Lock: os << "ScrollLock"; break;
        case osgGA::GUIEventAdapter::KEY_Sys_Req: os << "Return"; break;
        case osgGA::GUIEventAdapter::KEY_Escape: os << "Escape"; break;
        case osgGA::GUIEventAdapter::KEY_Delete: os << "Delete"; break;

        case osgGA::GUIEventAdapter::KEY_KP_Space: os << "KPSpace"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Tab: os << "KPTab"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Enter: os << "KPEnter"; break;
        case osgGA::GUIEventAdapter::KEY_KP_F1: os << "KPF1"; break;
        case osgGA::GUIEventAdapter::KEY_KP_F2: os << "KPF2"; break;
        case osgGA::GUIEventAdapter::KEY_KP_F3: os << "KPF3"; break;
        case osgGA::GUIEventAdapter::KEY_KP_F4: os << "KPF4"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Home: os << "KPHome"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Left: os << "KPLeft"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Up: os << "KPUp"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Right: os << "KPRight"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Down: os << "KPDown"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Page_Up: os << "KPPgUp"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Page_Down: os << "KPPgDw"; break;
        case osgGA::GUIEventAdapter::KEY_KP_End: os << "KPEnd"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Begin: os << "KPBegin"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Insert: os << "KPInsert"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Delete: os << "KPDel"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Equal: os << "KPEqual"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Multiply: os << "KPMulti"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Add: os << "KPAdd"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Separator: os << "KPSeparator"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Subtract: os << "KPSubtract"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Decimal: os << "KPDecimal"; break;
        case osgGA::GUIEventAdapter::KEY_KP_Divide: os << "KPDivide"; break;


        default:
        {
            if (k >= osgGA::GUIEventAdapter::KEY_0 && k <= osgGA::GUIEventAdapter::KEY_9)
            {
                os << "Key_" << (k - osgGA::GUIEventAdapter::KEY_0);
            }
            else if (k >= osgGA::GUIEventAdapter::KEY_A && k <= osgGA::GUIEventAdapter::KEY_Z)
            {
                os << "Key_" << static_cast<char>('A' + (k - osgGA::GUIEventAdapter::KEY_A));
            }
            else if (k >= osgGA::GUIEventAdapter::KEY_KP_0 && k <= osgGA::GUIEventAdapter::KEY_KP_9)
            {
                os << "Key_KP" << (k - osgGA::GUIEventAdapter::KEY_KP_0);
            }
            else if (k >= osgGA::GUIEventAdapter::KEY_F1 && k <= osgGA::GUIEventAdapter::KEY_F34)
            {
                os << "Key_" << 'F' << (k - osgGA::GUIEventAdapter::KEY_F1 + 1);
            }
            else
                os << "0x" << std::hex << static_cast<int>(k) << std::dec;
        }
        break;
        }
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter::ModKeyMask & m)
    {
        std::string str;
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_SHIFT, "RShift");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_SHIFT, "LShift");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_CTRL, "LCtrl");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_CTRL, "RCtrl");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_ALT, "LAlt");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_ALT, "RAlt");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_META, "LMeta");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_META, "RMeta");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_SUPER, "LSuper");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_SUPER, "RSuper");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_LEFT_HYPER, "LHyper");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_RIGHT_HYPER, "RHyper");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_NUM_LOCK, "NumLock");
        GUIEventAdapter_Check_ModKeyMask(MODKEY_CAPS_LOCK, "CapsLock");
        if (str.empty())
            str = "none";
        os << str;
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::PointerData& pd)
    {
        os << "{obj=" << pd.object.get()
            << ";x=" << pd.x << ";y=" << pd.y
            << ";xMin=" << pd.xMin << ";xMax=" << pd.xMax
            << ";yMin=" << pd.yMin << ";yMax=" << pd.yMax
            << "}";
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIEventAdapter& ea)
    {
        os << "{type=" << ea.getEventType() << ";time=" << ea.getTime();
        switch (ea.getEventType())
        {
        case osgGA::GUIEventAdapter::NONE:
            break;
        case osgGA::GUIEventAdapter::PUSH:
        case osgGA::GUIEventAdapter::RELEASE:
        case osgGA::GUIEventAdapter::DOUBLECLICK:
        case osgGA::GUIEventAdapter::DRAG:
        case osgGA::GUIEventAdapter::MOVE:
        case osgGA::GUIEventAdapter::SCROLL:
            os << ";buttons=" << ea.getButton() << ";buttonMask=" << (osgGA::GUIEventAdapter::MouseButtonMask)ea.getButtonMask()
                << ";mask=" << (osgGA::GUIEventAdapter::ModKeyMask)ea.getModKeyMask()
                << ";numPt=" << ea.getNumPointerData()
                << ";x=" << ea.getX() << ";y=" << ea.getY() << ";yOrient=" << ea.getMouseYOrientation()
                << ";xMin=" << ea.getXmin() << ";xMax=" << ea.getXmax()
                << ";yMin=" << ea.getYmin() << ";yMax=" << ea.getYmax();
            if (ea.getNumPointerData())
                os << ";lastPd=" << *ea.getPointerData(ea.getNumPointerData() - 1);
            break;
        case osgGA::GUIEventAdapter::KEYDOWN:
        case osgGA::GUIEventAdapter::KEYUP:
            os << ";key=" << (osgGA::GUIEventAdapter::KeySymbol)ea.getKey()
                << ";mask=" << (osgGA::GUIEventAdapter::ModKeyMask)ea.getModKeyMask();
            break;
        case osgGA::GUIEventAdapter::RESIZE:
            os << ";x=" << ea.getWindowX() << ";y=" << ea.getWindowY() << ";w=" << ea.getWindowWidth() << ";h=" << ea.getWindowHeight();
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
            os << ";unhandled" << std::dec << ea.getEventType();
            break;
        }
        os << "}";
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgGA::GUIActionAdapter& aa)
    {
        os << "{view=" << const_cast<osgGA::GUIActionAdapter&>(aa).asView() << "}";
        return os;
    }


} // namespace std

osg::NotifySeverity severityFromString(const std::string & input)
{
    osg::NotifySeverity ret = (osg::NotifySeverity)-1;

    std::string str = input;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (str == "always")
        ret = osg::ALWAYS;
    else if (str == "fatal")
        ret = osg::FATAL;
    else if (str == "warn")
        ret = osg::WARN;
    else if (str == "notice")
        ret = osg::NOTICE;
    else if (str == "info")
        ret = osg::INFO;
    else if (str == "debug")
        ret = osg::DEBUG_INFO;
    else if (str == "full")
        ret = osg::DEBUG_FP;
    return ret;
}

void initializeNotifyLevels(osg::ArgumentParser & arguments)
{
    std::string osgnotifylevel;
#ifdef SGI_USE_OSGEARTH
    std::string osgearthnotifylevel;
#endif
    if (arguments.read("--debug"))
    {
        osgnotifylevel = "debug";
#ifdef SGI_USE_OSGEARTH
        osgearthnotifylevel = "debug";
#endif
    }
    else
    {
        if (!arguments.read("--osgdebug", osgnotifylevel))
            osgnotifylevel.clear();
#ifdef SGI_USE_OSGEARTH
        if (!arguments.read("--earthdebug", osgearthnotifylevel))
            osgearthnotifylevel.clear();
#endif
    }
    if (!osgnotifylevel.empty())
    {
        osg::NotifySeverity level = severityFromString(osgnotifylevel);
        if (level >= 0)
            osg::setNotifyLevel(level);
    }
#ifdef SGI_USE_OSGEARTH
    if (!osgearthnotifylevel.empty())
    {
        osg::NotifySeverity level = severityFromString(osgearthnotifylevel);
        if (level >= 0)
            osgEarth::setNotifyLevel(level);
    }
#endif
}

bool KeyboardDumpHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv)
{
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::KEYDOWN:
    case osgGA::GUIEventAdapter::KEYUP:
        std::cout << "ea=" << ea << " aa=" << aa << std::endl;
        break;
    default: break;
    }
    return osgGA::GUIEventHandler::handle(ea, aa, obj, nv);
}

bool MouseDumpHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv)
{
    switch (ea.getEventType())
    {
    case osgGA::GUIEventAdapter::PUSH:
    case osgGA::GUIEventAdapter::RELEASE:
    case osgGA::GUIEventAdapter::DOUBLECLICK:
    case osgGA::GUIEventAdapter::DRAG:
        std::cout << "ea=" << ea << " aa=" << aa << std::endl;
        break;
    case osgGA::GUIEventAdapter::MOVE:
        // no messages for mouse move (because too much)
        break;
    default: break;
    }
    return osgGA::GUIEventHandler::handle(ea, aa, obj, nv);
}


void MovieEventHandler::setTrackMouse(bool tm)
{
    if (tm == _trackMouse) return;

    _trackMouse = tm;

    std::cout << "tracking mouse: " << (_trackMouse ? "ON" : "OFF") << std::endl;

    for (ImageStreamList::iterator itr = _imageStreamList.begin();
        itr != _imageStreamList.end();
        ++itr)
    {
        if ((*itr)->getStatus() == osg::ImageStream::PLAYING)
        {
            (*itr)->pause();
        }
        else
        {
            (*itr)->play();
        }
    }

}

class MovieEventHandler::FindImageStreamsVisitor : public osg::NodeVisitor
{
public:
    FindImageStreamsVisitor(ImageStreamList& imageStreamList, TraversalMode mode = TRAVERSE_ALL_CHILDREN)
        : osg::NodeVisitor(mode)
        , _imageStreamList(imageStreamList) {}

    virtual void apply(osg::Geode& geode) override
    {
        apply(geode.getStateSet());

        for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
        {
            apply(geode.getDrawable(i)->getStateSet());
        }

        traverse(geode);
    }

    virtual void apply(osg::Node& node) override
    {
        apply(node.getStateSet());
        traverse(node);
    }

    inline void apply(osg::StateSet* stateset)
    {
        if (!stateset) return;

        osg::StateAttribute* attr = stateset->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
        if (attr)
        {
            osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(attr);
            if (texture2D) apply(dynamic_cast<osg::ImageStream*>(texture2D->getImage()));

            osg::TextureRectangle* textureRec = dynamic_cast<osg::TextureRectangle*>(attr);
            if (textureRec) apply(dynamic_cast<osg::ImageStream*>(textureRec->getImage()));
        }
    }

    inline void apply(osg::ImageStream* imagestream)
    {
        if (imagestream)
        {
            _imageStreamList.push_back(imagestream);
            s_imageStream = imagestream;
        }
    }

    ImageStreamList& _imageStreamList;

protected:

    FindImageStreamsVisitor & operator = (const FindImageStreamsVisitor&) { return *this; }
};

osg::ImageStream* MovieEventHandler::s_imageStream = nullptr;

void MovieEventHandler::set(osg::Node* node)
{
    _imageStreamList.clear();
    if (node)
    {
        FindImageStreamsVisitor fisv(_imageStreamList);
        node->accept(fisv);
    }
    _imageStreamPlayBackSpeedList.resize(_imageStreamList.size());
}


bool MovieEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&, osg::Object*, osg::NodeVisitor*)
{
    switch (ea.getEventType())
    {
    case(osgGA::GUIEventAdapter::FRAME):
    {
        double t = ea.getTime();
        bool printed(false);

        ImageStreamPlayBackSpeedList::iterator fps_itr = _imageStreamPlayBackSpeedList.begin();
        for (ImageStreamList::iterator itr = _imageStreamList.begin();
            itr != _imageStreamList.end();
            ++itr, ++fps_itr)
        {
            if (((*itr)->getStatus() == osg::ImageStream::PLAYING) && ((*itr)->data() != (*fps_itr).lastData))
            {
                ImageStreamPlaybackSpeedData& data(*fps_itr);
                double dt = (data.timeStamp > 0) ? t - data.timeStamp : 1 / 60.0;
                data.lastData = (*itr)->data();
                data.fps = (*fps_itr).fps * 0.8 + 0.2 * (1 / dt);
                data.timeStamp = t;

                if (t - data.lastOutput > 1)
                {
                    std::cout << data.fps << " ";
                    data.lastOutput = t;
                    printed = true;
                }

            }
        }
        if (printed)
            std::cout << std::endl;
    }
    break;
    case(osgGA::GUIEventAdapter::MOVE):
    {
        if (_trackMouse)
        {
            for (ImageStreamList::iterator itr = _imageStreamList.begin();
                itr != _imageStreamList.end();
                ++itr)
            {
                double dt = (*itr)->getLength() * ((1.0 + ea.getXnormalized()) / 2.0);
                (*itr)->seek(dt);
                std::cout << "seeking to " << dt << " length: " << (*itr)->getLength() << std::endl;
            }
        }
        return false;
    }

    case(osgGA::GUIEventAdapter::KEYDOWN):
    {
        if (ea.getKey() == 'p')
        {
            for (ImageStreamList::iterator itr = _imageStreamList.begin();
                itr != _imageStreamList.end();
                ++itr)
            {
                if ((*itr)->getStatus() == osg::ImageStream::PLAYING)
                {
                    // playing, so pause
                    std::cout << "Pause" << std::endl;
                    (*itr)->pause();
                }
                else
                {
                    // playing, so pause
                    std::cout << "Play" << std::endl;
                    (*itr)->play();
                }
            }
            return true;
        }
        else if (ea.getKey() == 'r')
        {
            for (ImageStreamList::iterator itr = _imageStreamList.begin();
                itr != _imageStreamList.end();
                ++itr)
            {
                std::cout << "Restart" << std::endl;
                (*itr)->rewind();
            }
            return true;
        }
        else if (ea.getKey() == 'L')
        {
            for (ImageStreamList::iterator itr = _imageStreamList.begin();
                itr != _imageStreamList.end();
                ++itr)
            {
                if ((*itr)->getLoopingMode() == osg::ImageStream::LOOPING)
                {
                    std::cout << "Toggle Looping Off" << std::endl;
                    (*itr)->setLoopingMode(osg::ImageStream::NO_LOOPING);
                }
                else
                {
                    std::cout << "Toggle Looping On" << std::endl;
                    (*itr)->setLoopingMode(osg::ImageStream::LOOPING);
                }
            }
            return true;
        }
        else if (ea.getKey() == 'i')
        {
            setTrackMouse(!_trackMouse);


        }
        return false;
    }

    default:
        return false;
    }

    return false;
}

void MovieEventHandler::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding("i", "toggle interactive mode, scrub via mouse-move");
    usage.addKeyboardMouseBinding("p", "Play/Pause movie");
    usage.addKeyboardMouseBinding("r", "Restart movie");
    usage.addKeyboardMouseBinding("l", "Toggle looping of movie");
}



bool iequals(const std::string& a, const std::string& b)
{
    size_t sz = a.size();
    if (b.size() != sz)
        return false;
    for (size_t i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

#ifdef __linux__
namespace {
    std::string getOSGDBModuleFilename()
    {
        std::string ret;
        Dl_info info;
        const char* (* addr) () = osgDBGetVersion;
        if(dladdr(reinterpret_cast<const void*>(addr), &info) != 0)
        {
            ret = info.dli_fname;
        }
        return ret;
    }
    std::string getOSGDBModuleDirectory()
    {
        std::string modulefilename = getOSGDBModuleFilename();
        std::string ret(modulefilename, 0, modulefilename.find_last_of('/'));
        return ret;
    }
}

#endif

sgi_CommonHelper::sgi_CommonHelper(osg::ArgumentParser& args)
    : glprofile(GLContextProfileNone)
    , glversion()
    , addSceneGraphInspector(true)
    , showSceneGraphInspector(true)
{
    args.read("--glver", glversion);
    if (args.read("--core"))
        glprofile = GLContextProfileCore;
    if (args.read("--compat"))
        glprofile = GLContextProfileCompatibility;

    if (args.read("--nosgi"))
        addSceneGraphInspector = false;
    if (args.read("--hidesgi"))
        showSceneGraphInspector = false;
}

sgi_CommonHelper::sgi_CommonHelper(const sgi_CommonHelper & rhs)
    : glprofile(rhs.glprofile)
    , glversion(rhs.glversion)
    , addSceneGraphInspector(rhs.addSceneGraphInspector)
    , showSceneGraphInspector(rhs.showSceneGraphInspector)
{
}

sgi_CommonHelper::~sgi_CommonHelper()
{
}

sgi_MapNodeHelper::sgi_MapNodeHelper(osg::ArgumentParser& args)
    : sgi_CommonHelper(args)
    , m_errorMessages()
    , m_files()
#ifdef SGI_USE_OSGEARTH
    , _mapNodeHelper(new osgEarth::Util::MapNodeHelper)
    , _useOELighting(true)
#endif
    , _usageMessage(nullptr)
    , _onlyImages(false)
    , _addKeyDumper(false)
    , _addMouseDumper(false)
    , _movieTrackMouse(false)
    , _viewpointNum(-1)
    , _viewpointName()
{
#ifdef __linux__
    osgDB::Registry * registry = osgDB::Registry::instance();
    registry->initLibraryFilePathList();
    osgDB::FilePathList libdirs = registry->getLibraryFilePathList();
    libdirs.push_back(getOSGDBModuleDirectory());
    registry->setLibraryFilePathList(libdirs);
#endif
}

sgi_MapNodeHelper::sgi_MapNodeHelper(const sgi_MapNodeHelper & rhs)
    : sgi_CommonHelper(rhs)
{
}

sgi_MapNodeHelper::~sgi_MapNodeHelper()
{
#ifdef SGI_USE_OSGEARTH
    delete _mapNodeHelper;
#endif
}


void sgi_MapNodeHelper::setupInitialPosition(osgViewer::View* view) const
{
    osgGA::CameraManipulator * manip = view->getCameraManipulator();

#ifdef SGI_USE_OSGEARTH
    osgEarth::Util::EarthManipulator* earth_manip = dynamic_cast<osgEarth::Util::EarthManipulator*>(manip);
#endif
    osgGA::TrackballManipulator* trackball_manip = dynamic_cast<osgGA::TrackballManipulator*>(manip);

#ifdef SGI_USE_OSGEARTH
    if (earth_manip)
    {
        osgEarth::Viewpoint selectedViewpoint;
        bool gotViewpoint = false;

        osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode(view->getSceneData());
        if (mapNode.valid())
        {
            // look for external data in the map node:
            const osgEarth::Config& externals = mapNode->externalConfig();
            osgEarth::Config        viewpointsConf = externals.child("viewpoints");

            // backwards-compatibility: read viewpoints at the top level:
            const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
            for (osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
                viewpointsConf.add(*i);

            const osgEarth::ConfigSet& children = viewpointsConf.children();
            if (children.size() > 0)
            {
                int currentViewpointNum = 0;
                for (osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i, ++currentViewpointNum)
                {
                    osgEarth::Viewpoint curvp(*i);
                    if (currentViewpointNum == _viewpointNum || (!_viewpointName.empty() && iequals(curvp.name().value(), _viewpointName)))
                    {
                        gotViewpoint = true;
                        selectedViewpoint = curvp;
                    }
                }
            }

        }
        if(gotViewpoint)
            earth_manip->setViewpoint(selectedViewpoint);
    }
    else
#endif // SGI_USE_OSGEARTH
        if (trackball_manip)
        {
            // set clear color to OSG default (violet-blue)
            view->getCamera()->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));

            osg::ref_ptr<osg::Node> root = view->getSceneData();
            osg::BoundingSphere bs = root->getBound();
            float radiusDistanceFactor = 2.5f;
            osg::Group * mainGroup = dynamic_cast<osg::Group*>(root.get());
            if (mainGroup)
            {
                osg::ref_ptr<osg::LOD> firstChildLOD = mainGroup->getNumChildren() ? dynamic_cast<osg::LOD*>(mainGroup->getChild(0)) : nullptr;
                if (firstChildLOD)
                {
                    bs._center = firstChildLOD->getCenter();
                    bs._radius = firstChildLOD->getRadius();
                    // make sure we are inside the first LOD radius
                    radiusDistanceFactor = 0.9f;
                }
            }
            trackball_manip->setCenter(bs.center());
            trackball_manip->setDistance(bs.radius() * radiusDistanceFactor);
        }
}

std::string
sgi_MapNodeHelper::usage() const
{
    std::stringstream ss;
    ss  << "  --osgdebug <level>            : set OSG_NOTIFY_LEVEL to specified level\n";

#ifdef SGI_USE_OSGEARTH
    ss  << _mapNodeHelper->usage()
        << "  --earthdebug <level>          : set OSGEARTG_NOTIFY_LEVEL to specified level\n"
        << "  --debug                       : set OSG_NOTIFY_LEVEL and OSGEARTG_NOTIFY_LEVEL to debug\n"
        << "  --autoclose <ms>              : set up timer to close the main window after the given time in milliseconds\n"
        << "  --viewpoint <name|num>        : jump to the given viewpoint\n"
        ;
#endif
    ss  << "  --nosgi                       : do not add SceneGraphInspector handle\n"
        << "  --hidesgi                     : do not show SceneGraphInspector after loading\n"
        << "  --keys                        : enable keyboard event logging\n"
        << "  --mouse                       : enable mouse event logging\n"
        << "  --track-mouse                 : adjust animation speed by mouse moves\n"
        ;

    return ss.str();
}

std::string sgi_MapNodeHelper::errorMessages() const
{
    return m_errorMessages.str();
}
#ifdef SGI_USE_OSGEARTH
namespace {
    static const char * vert_defaultMaterial =
        "#version " GLSL_VERSION_STR "\n"
        "vec4 vp_Color; \n"
        "void defaultMaterial(inout vec4 vert) { \n"
        "    vp_Color = vec4(1,1,1,1); \n"
        "}\n"
        ;
}
#endif // SGI_USE_OSGEARTH


osg::Group * sgi_MapNodeHelper::setupLight(osg::Group * root)
{
    unsigned lightNum = 0;
    osg::Group* lights = root;
#ifdef SGI_USE_OSGEARTH
    if (_useOELighting)
    {
        lights = new osg::Group();
        lights->setName("lights");

        osgEarth::Util::Ephemeris e;
        osgEarth::DateTime dt(2016, 8, 10, 14.0);
        osgEarth::Util::CelestialBody sun = e.getSunPosition(dt);
        osg::Vec3d world = sun.geocentric;

        osg::Light* sunLight = new osg::Light(lightNum);
        world.normalize();
        sunLight->setPosition(osg::Vec4d(world, 0.0));

        sunLight->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
        sunLight->setDiffuse(osg::Vec4(1.0f, 1.0f, 0.9f, 1.0f));

        osg::LightSource* sunLS = new osg::LightSource();
        sunLS->setLight(sunLight);
        osgEarth::GenerateGL3LightingUniforms generateUniforms;
        sunLS->accept(generateUniforms);
#ifdef OSG_GL3_FEATURES
        // remove GL_LIGHT0 from the stateSet to avoid GL errors about invalid enum
        sunLS->getStateSet()->removeMode(GL_LIGHT0 + lightNum);
#endif
        sunLS->getStateSet()->setDefine("OE_LIGHTING");

        osgEarth::Util::ShadowCaster* caster = osgEarth::findTopMostNodeOfType<osgEarth::Util::ShadowCaster>(root);
        if (caster)
        {
            OE_INFO << "Found a shadow caster!\n";
            caster->setLight(sunLight);
        }

        osg::Group * groupPhong = new osg::Group;
        groupPhong->setName("groupPhong");
        // Add phong lighting.
        osgEarth::PhongLightingEffect * phong = new osgEarth::PhongLightingEffect();
        phong->attach(groupPhong->getOrCreateStateSet());
        groupPhong->setUserData(phong);

        // Generate the necessary uniforms for the shaders.
        osgEarth::GenerateGL3LightingUniforms gen;
        groupPhong->accept(gen);

        osg::Group * groupDefaultMaterial = new osg::Group;
        groupDefaultMaterial->setName("defaultMaterial");
        // install a default material for everything in the map
        osg::Material* defaultMaterial = new osgEarth::MaterialGL3();
        defaultMaterial->setDiffuse(defaultMaterial->FRONT, osg::Vec4(1, 1, 1, 1));
        defaultMaterial->setAmbient(defaultMaterial->FRONT, osg::Vec4(1, 1, 1, 1));
        osg::StateSet * groupDefaultMaterialStateSet = groupDefaultMaterial->getOrCreateStateSet();
        groupDefaultMaterialStateSet->setAttributeAndModes(defaultMaterial, 1);
        osgEarth::MaterialCallback().operator()(defaultMaterial, nullptr);

        lights->addChild(groupDefaultMaterial);
        groupDefaultMaterial->addChild(sunLS);
        sunLS->addChild(groupPhong);
        groupPhong->addChild(root);

        osg::ref_ptr<osgEarth::StateSetCache> cache = new osgEarth::StateSetCache();
        osgEarth::Registry::shaderGenerator().run(root, cache.get());
    }
#endif
    return lights;
}

osg::Group*
sgi_MapNodeHelper::load(osg::ArgumentParser& args,
    osgViewer::View* view
#ifdef SGI_USE_OSGEARTH
    , osgEarth::Util::Controls::Container* userContainer
#endif
)
{
    if (args.read("--keys"))
        _addKeyDumper = true;
    if (args.read("--mouse"))
        _addMouseDumper = true;
    if (args.read("--track-mouse"))
        _movieTrackMouse = true;

#ifdef SGI_USE_OSGEARTH
    if (args.read("--no-oe-lighting"))
        _useOELighting = false;
#endif

    if (!args.read("--viewpoint", _viewpointNum))
    {
        _viewpointNum = -1;
        if (!args.read("--viewpoint", _viewpointName))
            _viewpointName.clear();
    }

    _usageMessage = args.getApplicationUsage();

    osgDB::Registry * registry = osgDB::Registry::instance();
    bool hasAtLeastOneNode = false;
    bool hasAtLeastOneObject = false;
    bool hasAtLeastOneImage = false;

    // a root node to hold everything:
    bool previousWasOption = false;
    // check if the args only contain the executable name and nothing else
    bool emptyArgs = args.argc() <= 1;
    osg::Group * root = new osg::Group;
    // load all files from the given args
    for (int i = 1; i < args.argc(); )
    {
        bool argRemoved = false;
        // skip all dash arguments (switches, flags, options, etc)
        if (!args.isOption(i))
        {
            if (!previousWasOption)
            {
                std::string arg = args[i];
                std::string protocol;

                const char * scheme_sep = strstr(args[i], "://");
                if (scheme_sep)
                {
                    std::string schema = std::string(args[i], scheme_sep - args[i]);
                    if (schema == "file")
                        arg = &args[i][7];
                    else
                    {
                        protocol = schema;
                    }
                }

                std::string lowercaseExt = osgDB::getLowerCaseFileExtension(arg);
                if (!lowercaseExt.empty())
                {
                    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = registry->getReaderWriterForExtension(lowercaseExt);
                    if (!readerWriter.valid())
                    {
                        readerWriter = registry->getReaderWriterForProtocolAndExtension(protocol, lowercaseExt);
                    }
                    if (readerWriter.valid())
                    {
                        m_files.push_back(arg);
                        osgDB::ReaderWriter::ReadResult rr;
                        osgDB::ReaderWriter::Features supportedFeatures = readerWriter->supportedFeatures();
                        if (!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_NODE))
                            rr = osgDB::readRefNodeFile(arg, registry->getOptions());
                        if (rr.success() && !rr.validObject())
                            rr = osgDB::ReaderWriter::ReadResult();
                        if (!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_IMAGE))
                            rr = osgDB::readRefImageFile(arg, registry->getOptions());
                        if (rr.success() && !rr.validObject())
                            rr = osgDB::ReaderWriter::ReadResult();
                        if (!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_HEIGHT_FIELD))
                            rr = osgDB::readRefHeightFieldFile(arg, registry->getOptions());
                        if (rr.success() && !rr.validObject())
                            rr = osgDB::ReaderWriter::ReadResult();
                        if (!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_OBJECT))
                            rr = osgDB::readRefObjectFile(arg, registry->getOptions());
                        if (rr.success() && !rr.validObject())
                            rr = osgDB::ReaderWriter::ReadResult();
                        if (rr.validObject())
                        {
                            if (rr.validNode())
                            {
                                hasAtLeastOneNode = true;
                                osg::ref_ptr<osg::Node> node = rr.takeNode();
                                sgi::osg_helpers::tagNodeForObjectTree(node.get(), arg);
                                root->addChild(node);
                            }
                            else if (rr.validImage())
                            {
                                hasAtLeastOneImage = true;
                                osg::ref_ptr<osg::Image> image = rr.takeImage();
                                osg::Node * node = sgi::osg_helpers::createNodeForImage(image.get());
                                sgi::osg_helpers::tagNodeForObjectTree(node, arg);
                                root->addChild(node);
                            }
                            else if (rr.validHeightField())
                            {
                                hasAtLeastOneImage = true;
                                osg::ref_ptr<osg::HeightField> hf = rr.takeHeightField();
                                osg::Node * node = new osg::ShapeDrawable(hf.get());

                                osg::UserDataContainer * container = node->getOrCreateUserDataContainer();
                                osg::ref_ptr<osg::Object> obj = rr.takeObject();
                                container->addUserObject(hf.get());

                                sgi::osg_helpers::tagNodeForObjectTree(node, arg);
                                root->addChild(node);
                            }
                            else
                            {
                                hasAtLeastOneObject = true;
                                osg::UserDataContainer * container = root->getOrCreateUserDataContainer();
                                osg::ref_ptr<osg::Object> obj = rr.takeObject();
                                //obj->setUserValue("sgi_tree_item", true);
                                container->addUserObject(obj);
                            }
                        }
                        else
                        {
                            m_errorMessages << "Unable to load file: " << arg << std::endl;
                            if (!rr.message().empty())
                                m_errorMessages << "Error: " << rr.message() << std::endl;
                            else if (rr.notHandled())
                                m_errorMessages << "Error: File format not handled" << std::endl;
                            else if (rr.notFound())
                                m_errorMessages << "Error: File not found" << std::endl;
                            else if (rr.notEnoughMemory())
                                m_errorMessages << "Error: Not enough memory" << std::endl;
                            else
                                m_errorMessages << "Error: Unknown" << std::endl;
                        }
                        args.remove(i);
                        argRemoved = true;
                    }
                    else
                    {
                        m_errorMessages << "No plugin found to load file: " << arg << std::endl;
                    }
                }
                else
                {
                    // no extension means, we found a parameter to another option.
                    // maybe the window size or something like this.
                }
            }
        }
        else
            previousWasOption = true;
        if (!argRemoved)
            i++;
    }

    if (!hasAtLeastOneNode && !hasAtLeastOneObject && !hasAtLeastOneImage)
    {
        if(emptyArgs)
            m_errorMessages << "No .earth, 3D model or image file/url specified in the command line. Empty command line arguments." << std::endl;
        else
            m_errorMessages << "No .earth, 3D model or image file/url specified in the command line." << std::endl;
        return nullptr;
    }

    // check if we only got one image and nothing else
    _onlyImages = (!hasAtLeastOneNode && !hasAtLeastOneObject && hasAtLeastOneImage);

#ifdef SGI_USE_OSGEARTH
    osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode(root);
    if (mapNode.valid())
    {
        // warn about not having an earth manip
        osgEarth::Util::EarthManipulator* manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(view->getCameraManipulator());
        if (!manipulator)
        {
            // install our default manipulator (do this before calling load)
            manipulator = new osgEarth::Util::EarthManipulator();
            view->setCameraManipulator(manipulator);
        }
    }
    else
#endif // SGI_USE_OSGEARTH
    {
#ifdef SGI_USE_OSGEARTH
        m_errorMessages << "Loaded scene graph does not contain a MapNode" << std::endl;
#endif // SGI_USE_OSGEARTH
        osgGA::TrackballManipulator* manipulator = dynamic_cast<osgGA::TrackballManipulator*>(view->getCameraManipulator());
        if (!manipulator)
        {
            osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

            keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
            keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
            keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
            keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator());
            keyswitchManipulator->addMatrixManipulator('5', "Orbit", new osgGA::OrbitManipulator());
            keyswitchManipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator());
            keyswitchManipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator());

            std::string pathfile;
            double animationSpeed = 1.0;
            while (args.read("--speed", animationSpeed)) {}
            char keyForAnimationPath = '8';
            while (args.read("-p", pathfile))
            {
                osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
                if (apm || !apm->valid())
                {
                    apm->setTimeScale(animationSpeed);

                    unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                    keyswitchManipulator->addMatrixManipulator(keyForAnimationPath, "Path", apm);
                    keyswitchManipulator->selectMatrixManipulator(num);
                    ++keyForAnimationPath;
                }
            }

            view->setCameraManipulator(keyswitchManipulator.get());
        }

        root = setupLight(root);
    }

    std::string device;
    while (args.read("--device", device))
    {
        osg::ref_ptr<osgGA::Device> dev = osgDB::readRefFile<osgGA::Device>(device);
        if (dev.valid())
        {
            view->addDevice(dev);
        }
    }

    root = setupRootGroup(root);

#ifdef SGI_USE_OSGEARTH
    if (mapNode.valid())
        _mapNodeHelper->parse(mapNode.get(), args, view, root, userContainer);

    // DO NOT call _mapNodeHelper->configureView because it creates the event handlers and
    // this results in duplicated event handlers
#if 0
    // configures the viewer with some stock goodies
    _mapNodeHelper->configureView(view);
#else
    // ... but still initialize the camera stateSet
    // default uniform values:
    osgEarth::GLUtils::setGlobalDefaults(view->getCamera()->getOrCreateStateSet());
#endif

#endif // SGI_USE_OSGEARTH

    setupEventHandlers(view, root);

    return root;
}

osg::Group * sgi_MapNodeHelper::setupRootGroup(osg::Group * root)
{
    osg::Group * ret;

    ret = root;
    return ret;
}

void sgi_MapNodeHelper::setupEventHandlers(osgViewer::View* view, osg::Group * root)
{

    // add the state manipulator
    view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));

    // add the thread model handler
    view->addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    view->addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    view->addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    view->addEventHandler(new osgViewer::HelpHandler(_usageMessage));

    // add the record camera path handler
    view->addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    view->addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    view->addEventHandler(new osgViewer::ScreenCaptureHandler);

    if (_addKeyDumper)
        view->addEventHandler(new KeyboardDumpHandler);
    if (_addMouseDumper)
        view->addEventHandler(new MouseDumpHandler);

    // pass the model to the MovieEventHandler so it can pick out ImageStream's to manipulate.
    MovieEventHandler* meh = new MovieEventHandler();
    meh->set(root);

    if (_movieTrackMouse) 
        meh->setTrackMouse(true);

    view->addEventHandler(meh);
}

QWidget * getWidgetForGraphicsWindow(osgViewer::GraphicsWindow * gw, QWidget * parent)
{
    return sgi::osg_helpers::getWidgetForGraphicsWindow(gw, parent);
}

bool createWidgetForGraphicsWindow(osg::GraphicsContext * ctx, QObject * parent)
{
    return sgi::osg_helpers::createWidgetForGraphicsWindow(ctx, parent);
}
