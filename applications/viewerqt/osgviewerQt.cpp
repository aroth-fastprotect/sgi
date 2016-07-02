#include "osgviewerQt.h"
#include "osgviewerQt.moc"
#include <QTimer>
#include <QFileInfo>
#include <QApplication>
#include <QMessageBox>
#include <QHBoxLayout>

#include <osg/ValueObject>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgGA/Device>

#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>
#include <osgUtil/Optimizer>

#include <osgQt/GraphicsWindowQt>

#include <osgEarth/Notify>
#include <osgEarth/MapNode>
#include <osgEarth/MapFrame>
#include <osgEarth/Registry>
#include <osgEarth/TerrainEngineNode>

#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>

#include <iostream>
#include <iomanip>
#include <osg/io_utils>

#include <sgi/helpers/osg_helper_nodes>

namespace std {

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::NotifySeverity & t)
{
    switch(t)
    {
    case osg::ALWAYS: os << "ALWAYS"; break;
    case osg::FATAL: os << "FATAL"; break;
    case osg::WARN: os << "WARN"; break;
    case osg::NOTICE: os << "NOTICE"; break;
    case osg::INFO: os << "INFO"; break;
    case osg::DEBUG_INFO: os << "DEBUG_INFO"; break;
    case osg::DEBUG_FP: os << "DEBUG_FP"; break;
    default: os << (int)t; break;
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
    switch(t)
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
    default: os << (int)t; break;
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
    if(str.empty())
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
    switch(k)
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
            if(k >= osgGA::GUIEventAdapter::KEY_0 && k <= osgGA::GUIEventAdapter::KEY_9)
            {
                os << "Key_" << (k -osgGA::GUIEventAdapter::KEY_0);
            }
            else if(k >= osgGA::GUIEventAdapter::KEY_A && k <= osgGA::GUIEventAdapter::KEY_Z)
            {
                os << "Key_" << (char)('A' + (k -osgGA::GUIEventAdapter::KEY_A));
            }
            else if(k >= osgGA::GUIEventAdapter::KEY_KP_0 && k <= osgGA::GUIEventAdapter::KEY_KP_9)
            {
                os << "Key_KP" << (k - osgGA::GUIEventAdapter::KEY_KP_0);
            }
            else if(k >= osgGA::GUIEventAdapter::KEY_F1 && k <= osgGA::GUIEventAdapter::KEY_F34)
            {
                os << "Key_" << 'F' << (k - osgGA::GUIEventAdapter::KEY_F1 + 1);
            }
            else
                os << "0x" << std::hex << (int)k << std::dec;
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
    if(str.empty())
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
    switch(ea.getEventType())
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
            << ";mask=" <<  (osgGA::GUIEventAdapter::ModKeyMask)ea.getModKeyMask()
            << ";numPt=" << ea.getNumPointerData()
            << ";x=" << ea.getX() << ";y=" << ea.getY() << ";yOrient=" << ea.getMouseYOrientation()
            << ";xMin=" << ea.getXmin() << ";xMax=" << ea.getXmax()
            << ";yMin=" << ea.getYmin() << ";yMax=" << ea.getYmax();
        if(ea.getNumPointerData())
            os << ";lastPd=" << *ea.getPointerData(ea.getNumPointerData() - 1);
        break;
    case osgGA::GUIEventAdapter::KEYDOWN:
    case osgGA::GUIEventAdapter::KEYUP:
        os << ";key=" << (osgGA::GUIEventAdapter::KeySymbol)ea.getKey()
            << ";mask=" <<  (osgGA::GUIEventAdapter::ModKeyMask)ea.getModKeyMask();
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

    if(str == "always")
        ret = osg::ALWAYS;
    else if(str == "fatal")
        ret = osg::FATAL;
    else if(str == "warn")
        ret = osg::WARN;
    else if(str == "notice")
        ret = osg::NOTICE;
    else if(str == "info")
        ret = osg::INFO;
    else if(str == "debug")
        ret = osg::DEBUG_INFO;
    else if(str == "full")
        ret = osg::DEBUG_FP;
    return ret;
}

namespace {

    static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
    {
        if (texture_format == GL_BGRA) {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return ((src_pixel << 24) & 0xff000000)
                    | ((src_pixel >> 24) & 0x000000ff)
                    | ((src_pixel << 8) & 0x00ff0000)
                    | ((src_pixel >> 8) & 0x0000ff00);
            }
            else {
                return src_pixel;
            }
        }
        else {  // GL_RGBA
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
            }
            else {
                return ((src_pixel << 16) & 0xff0000)
                    | ((src_pixel >> 16) & 0xff)
                    | (src_pixel & 0xff00ff00);
            }
        }
    }

    QRgb qt_gl_convertToGLFormat(QRgb src_pixel, GLenum texture_format)
    {
        return qt_gl_convertToGLFormatHelper(src_pixel, texture_format);
    }

    static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format)
    {
        Q_ASSERT(dst.depth() == 32);
        Q_ASSERT(img.depth() == 32);

        if (dst.size() != img.size()) {
            int target_width = dst.width();
            int target_height = dst.height();
            qreal sx = target_width / qreal(img.width());
            qreal sy = target_height / qreal(img.height());

            quint32 *dest = (quint32 *)dst.scanLine(0); // NB! avoid detach here
            uchar *srcPixels = (uchar *)img.scanLine(img.height() - 1);
            int sbpl = img.bytesPerLine();
            int dbpl = dst.bytesPerLine();

            int ix = int(0x00010000 / sx);
            int iy = int(0x00010000 / sy);

            quint32 basex = int(0.5 * ix);
            quint32 srcy = int(0.5 * iy);

            // scale, swizzle and mirror in one loop
            while (target_height--) {
                const uint *src = (const quint32 *)(srcPixels - (srcy >> 16) * sbpl);
                int srcx = basex;
                for (int x = 0; x < target_width; ++x) {
                    dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                    srcx += ix;
                }
                dest = (quint32 *)(((uchar *)dest) + dbpl);
                srcy += iy;
            }
        }
        else {
            const int width = img.width();
            const int height = img.height();
            const uint *p = (const uint*)img.scanLine(img.height() - 1);
            uint *q = (uint*)dst.scanLine(0);

            if (texture_format == GL_BGRA) {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    // mirror + swizzle
                    for (int i = 0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = ((*p << 24) & 0xff000000)
                                | ((*p >> 24) & 0x000000ff)
                                | ((*p << 8) & 0x00ff0000)
                                | ((*p >> 8) & 0x0000ff00);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                }
                else {
                    p = (const uint*)img.scanLine(0);
                    for (int i = 0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                            p++;
                            q++;
                        }
                        //p -= 2 * width;
                    }
                    /*
                    const uint bytesPerLine = img.bytesPerLine();
                    for (int i = 0; i < height; ++i) {
                        memcpy(q, p, bytesPerLine);
                        q += width;
                        p -= width;
                    }
                    */
                }
            }
            else {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    for (int i = 0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = (*p << 8) | ((*p >> 24) & 0xff);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                }
                else {
                    for (int i = 0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                }
            }
        }
    }

    QImage convertToGLFormat(const QImage& img)
    {
        QImage res(img.size(), QImage::Format_ARGB32);
        convertToGLFormatHelper(res, img.convertToFormat(QImage::Format_ARGB32), GL_BGRA);
        return res;
    }
}

bool osgImageToQImage(const osg::Image * image, QImage * qimage)
{
    bool ret = false;
    QImage::Format format;
    switch (image->getPixelFormat())
    {
    case GL_RGB:
        format = QImage::Format_RGB888;
        break;
    case GL_RGBA:
        //format = QImage::Format_ARGB32;
        format = QImage::Format_ARGB32_Premultiplied;
        break;
    case GL_LUMINANCE:
        format = QImage::Format_Indexed8;
        break;
    default:
        format = QImage::Format_Invalid;
        break;
    }

    if (format != QImage::Format_Invalid)
    {
        int bytesPerLine = image->getRowSizeInBytes();
        int width = image->s();
        int height = image->t();
        *qimage = QImage(image->data(), width, height, bytesPerLine, format);
        *qimage = convertToGLFormat(*qimage);
        //ret = ret.mirrored(false, true);
        ret = true;
    }
    return ret;
}

class KeyboardDumpHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardDumpHandler() {}
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) override
    {
        switch(ea.getEventType())
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
        case osgGA::GUIEventAdapter::KEYUP:
            std::cout << "ea=" << ea << " aa=" << aa << std::endl;
            break;
        default: break;
        }
        return osgGA::GUIEventHandler::handle(ea, aa, obj, nv);
    }
};

class MouseDumpHandler : public osgGA::GUIEventHandler
{
public:
    MouseDumpHandler() {}

    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) override
    {
        switch(ea.getEventType())
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
};

bool iequals(const std::string& a, const std::string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

TEVMapNodeHelper::TEVMapNodeHelper()
{
    _mapNodeHelper = new osgEarth::Util::MapNodeHelper;
}

TEVMapNodeHelper::~TEVMapNodeHelper()
{
    delete _mapNodeHelper;
}


void TEVMapNodeHelper::setupInitialPosition( osgViewer::View* view, int viewpointNum, const std::string & viewpointName ) const
{
    osgGA::CameraManipulator * manip = view->getCameraManipulator();

    osgEarth::Util::EarthManipulator* earth_manip = dynamic_cast<osgEarth::Util::EarthManipulator*>(manip);
    osgGA::TrackballManipulator* trackball_manip = dynamic_cast<osgGA::TrackballManipulator*>(manip);

    if(earth_manip)
    {
        osgEarth::Viewpoint selectedViewpoint;
        bool gotViewpoint = false;

        osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode(view->getSceneData());
        if(mapNode.valid())
        {
            // look for external data in the map node:
            const osgEarth::Config& externals = mapNode->externalConfig();
            osgEarth::Config        viewpointsConf  = externals.child("viewpoints");

            // backwards-compatibility: read viewpoints at the top level:
            const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
            for( osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i )
                viewpointsConf.add( *i );

            const osgEarth::ConfigSet& children = viewpointsConf.children();
            if ( children.size() > 0 )
            {
                int currentViewpointNum = 0;
                for( osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i, ++currentViewpointNum )
                {
                    osgEarth::Viewpoint curvp(*i);
                    if(currentViewpointNum == viewpointNum || (!viewpointName.empty() && iequals(curvp.name().value(), viewpointName)))
                    {
                        gotViewpoint = true;
                        selectedViewpoint = curvp;
                    }
                }
            }

        }
        earth_manip->setViewpoint(selectedViewpoint);
    }
    else if(trackball_manip)
    {
        // set clear color to OSG default (violett-blue)
        view->getCamera()->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));

        osg::ref_ptr<osg::Node> root = view->getSceneData();
        osg::BoundingSphere bs = root->getBound();
        float radiusDistanceFactor = 2.5f;
        osg::Group * mainGroup = dynamic_cast<osg::Group*>(root.get());
        if(mainGroup)
        {
            osg::ref_ptr<osg::LOD> firstChildLOD = mainGroup->getNumChildren()?dynamic_cast<osg::LOD*>(mainGroup->getChild(0)):NULL;
            if(firstChildLOD)
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
TEVMapNodeHelper::usage() const
{
    std::string msg = osgEarth::Stringify()
        << "    --nosgi              : do not add SceneGraphInspector\n"
        << "    --hidesgi            : do not show SceneGraphInspector\n"
        << "    --nokeys             : do not add keyboard dump handler\n"
        << "    --nomouse            : do not add mouse dump handler\n"
        << "    --osgdebug <level>   : set OSG_NOTIFY_LEVEL to specified level\n"
        << "    --earthdebug <level> : set OSGEARTG_NOTIFY_LEVEL to specified level\n"
        << "    --debug              : set OSG_NOTIFY_LEVEL and OSGEARTG_NOTIFY_LEVEL to debug\n"
        << "    --autoclose <ms>     : set up timer to close the main window after the given time in milliseconds\n"
        << "    --viewpoint <name|num>  : jump to the given viewpoint\n"
        ;
    return _mapNodeHelper->usage() + msg;
}

std::string TEVMapNodeHelper::errorMessages() const
{
    return m_errorMessages.str();
}

osg::Group*
TEVMapNodeHelper::load(osg::ArgumentParser& args,
                    osgViewer::View* view,
                    osgEarth::Util::Controls::Container* userContainer)
{
    osgDB::Registry * registry = osgDB::Registry::instance();
    bool hasAtLeastOneNode = false;
    bool hasAtLeastOneObject = false;

    // a root node to hold everything:
    bool previousWasOption = false;
    osg::Group * root = new osg::Group;
    // load all files from the given args
    for( int i=1; i<args.argc(); )
    {
        bool argRemoved = false;
        // skip all dash arguments (switches, flags, options, etc)
        if(!args.isOption(i))
        {
            if(!previousWasOption)
            {
                std::string arg = args[i];
                std::string protocol;

                const char * scheme_sep = strstr(args[i], "://");
                if(scheme_sep)
                {
                    std::string schema = std::string(args[i], scheme_sep - args[i]);
                    if(schema == "file")
                        arg = &args[i][7];
                    else
                    {
                        protocol = schema;
                    }
                }

                std::string lowercaseExt = osgDB::getLowerCaseFileExtension(arg);
                if(!lowercaseExt.empty())
                {
                    osg::ref_ptr<osgDB::ReaderWriter> readerWriter = registry->getReaderWriterForExtension(lowercaseExt);
                    if(!readerWriter.valid())
                    {
                        readerWriter = registry->getReaderWriterForProtocolAndExtension(protocol, lowercaseExt);
                    }
                    if(readerWriter.valid())
                    {
                        m_files.push_back(arg);
                        osgDB::ReaderWriter::ReadResult rr;
                        osgDB::ReaderWriter::Features supportedFeatures = readerWriter->supportedFeatures();
                        if(!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_NODE))
                            rr = osgDB::readRefNodeFile(arg,registry->getOptions());
                        if(!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_IMAGE))
                            rr = osgDB::readRefImageFile(arg,registry->getOptions());
                        if(!rr.success() && (supportedFeatures & osgDB::ReaderWriter::FEATURE_READ_HEIGHT_FIELD))
                            rr = osgDB::readRefHeightFieldFile(arg,registry->getOptions());
                        if(rr.validObject())
                        {
                            hasAtLeastOneObject = true;
                            if (rr.validNode())
                            {
                                hasAtLeastOneNode = true;
                                osg::ref_ptr<osg::Node> node = rr.takeNode();
                                sgi::osg_helpers::tagNodeForObjectTree(node.get(), arg);
                                root->addChild(node);
                            }
                            else if(rr.validImage())
                            {
                                hasAtLeastOneNode = true;
                                osg::ref_ptr<osg::Image> image = rr.takeImage();
                                osg::Node * node = sgi::osg_helpers::createNodeForImage(image.get());
                                sgi::osg_helpers::tagNodeForObjectTree(node, arg);
                                root->addChild(node);
                            }
                            else
                            {
                                osg::UserDataContainer * container = root->getOrCreateUserDataContainer();
                                osg::ref_ptr<osg::Object> obj = rr.takeObject();
                                //obj->setUserValue("sgi_tree_item", true);
                                container->addUserObject(obj);
                            }
                        }
                        else
                        {
                            m_errorMessages << "Unable to load file: " << arg << std::endl;
                            if(!rr.message().empty())
                                m_errorMessages << "Error: " << rr.message() << std::endl;
                            else if(rr.notHandled())
                                m_errorMessages << "Error: File format not handled" << std::endl;
                            else if(rr.notFound())
                                m_errorMessages << "Error: File not found" << std::endl;
                            else if(rr.notEnoughMemory())
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
        if(!argRemoved)
            i++;
    }

    if ( !hasAtLeastOneNode && !hasAtLeastOneObject )
    {
        m_errorMessages << "No .earth file or 3D model file specified in the command line." << std::endl;
        return 0L;
    }

    osg::ref_ptr<osgEarth::MapNode> mapNode = osgEarth::MapNode::findMapNode(root);
    if(mapNode.valid())
    {
        // warn about not having an earth manip
        osgEarth::Util::EarthManipulator* manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(view->getCameraManipulator());
        if(!manipulator)
        {
            // install our default manipulator (do this before calling load)
            manipulator = new osgEarth::Util::EarthManipulator();
            view->setCameraManipulator( manipulator );
        }
    }
    else
    {
        m_errorMessages << "Loaded scene graph does not contain a MapNode" << std::endl;
        // warn about not having an earth manip
        osgGA::TrackballManipulator* manipulator = dynamic_cast<osgGA::TrackballManipulator*>(view->getCameraManipulator());
        if(!manipulator)
        {
            // install our default manipulator (do this before calling load)
            manipulator = new osgGA::TrackballManipulator();
            view->setCameraManipulator( manipulator );
        }
    }

    root = setupRootGroup(root);

    if(mapNode.valid())
        _mapNodeHelper->parse( mapNode.get(), args, view, root, userContainer);

    // configures the viewer with some stock goodies
    _mapNodeHelper->configureView( view );

    return root;
}

osg::Group * TEVMapNodeHelper::setupRootGroup(osg::Group * root)
{
    osg::Group * ret;

    ret = root;
    return ret;
}

void setupWidgetAutoCloseTimer(QWidget * widget, int milliseconds)
{
    QTimer * closeTimer = new QTimer();
    QObject::connect(closeTimer, SIGNAL(timeout()), widget, SLOT(close()));
    closeTimer->setSingleShot(true);
    closeTimer->start(milliseconds);
}

ViewerWidget::ViewerWidget(osg::ArgumentParser & arguments, QWidget * parent)
    : QMainWindow(parent)
    , osgViewer::CompositeViewer(arguments)
    , _timer(new QTimer(this))
{
    int screenNum = -1;
    while (arguments.read("--screen",screenNum)) {}

    int x = -1, y = -1, width = -1, height = -1;
    while (arguments.read("--window",x,y,width,height)) {}

#if QT_VERSION >= 0x050000
    // Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
#endif

    // disable the default setting of viewer.done() by pressing Escape.
    setKeyEventSetsDone(0);

    _mainGW = createGraphicsWindow(0,0,QMainWindow::width(),QMainWindow::height());
    setCentralWidget(_mainGW->getGLWidget());

    _view = new osgViewer::View;
    addView( _view );

    osg::Camera* camera = _view->getCamera();
    camera->setGraphicsContext( _mainGW );

    const osg::GraphicsContext::Traits* traits = _mainGW->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    if(x >= 0 && y >= 0 && width >= 0 && height >= 0)
    {
        setGeometry(x, y, width, height);
    }

    connect( _timer, SIGNAL(timeout()), this, SLOT(onTimer()) );
    _timer->start( 10 );
}

ViewerWidget::~ViewerWidget()
{
}

osgViewer::View * ViewerWidget::view()
{
    return _view.get();
}

bool ViewerWidget::createCamera()
{
    return true;
}

void ViewerWidget::setData(osg::Node * node)
{
    _view->setSceneData(node);
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    return new osgQt::GraphicsWindowQt(traits.get());
}

void ViewerWidget::onTimer()
{
    frame();
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{
    QMainWindow::paintEvent(event);
    frame();
}


int
main(int argc, char** argv)
{
    int ret = 0;
#ifndef _WIN32
    // QApplication constructor shall call X11InitThreads to get OpenGL working with
    // multi-threading.
    QApplication::setAttribute (Qt::AA_X11InitThreads);
#endif // _WIN32

    QApplication app( argc, argv );
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    bool allocConsole = false;

    osg::ArgumentParser arguments(&argc,argv);
    if ( arguments.read("--nostencil") )
        osg::DisplaySettings::instance()->setMinimumNumStencilBits(0);
    if ( arguments.read("--console") )
        allocConsole = true;

    if(allocConsole)
    {
#ifdef _WIN32
        ::AllocConsole();
        freopen("CONIN$", "r",stdin);
        freopen("CONOUT$", "w",stdout);
        freopen("CONOUT$", "w",stderr);
#endif // _WIN32
    }

    std::string osgnotifylevel;
    std::string osgearthnotifylevel;
    if (arguments.read("--debug") )
    {
        osgnotifylevel = "debug";
        osgearthnotifylevel = "debug";
    }
    else
    {
        if(!arguments.read("--osgdebug", osgnotifylevel))
            osgnotifylevel.clear();
        if ( !arguments.read("--earthdebug", osgearthnotifylevel) )
            osgearthnotifylevel.clear();
    }
    if ( !osgnotifylevel.empty() )
    {
        osg::NotifySeverity level = severityFromString(osgnotifylevel);
        if(level >= 0)
            osg::setNotifyLevel(level);
    }

    if ( !osgearthnotifylevel.empty() )
    {
        osg::NotifySeverity level = severityFromString(osgearthnotifylevel);
        if(level >= 0)
            osgEarth::setNotifyLevel(level);
    }

    ViewerWidget * myMainWindow = new ViewerWidget(arguments);

    bool addSceneGraphInspector = true;
    bool showSceneGraphInspector = true;
    bool addKeyDumper = false;
    bool addMouseDumper = false;
    bool fullscreen = false;
    bool orderIndependantTransparency = false;
    int autoCloseTime = -1;
    int viewpointNum = -1;
    std::string viewpointName;
    std::string effectName;
    int numThreads = -1;
    int numHttpThreads = -1;

    if ( arguments.read("--nosgi") )
        addSceneGraphInspector = false;
    if ( arguments.read("--hidesgi") )
        showSceneGraphInspector = false;
    if (arguments.read("--oit"))
        orderIndependantTransparency = true;

    if ( arguments.read("--keys") )
        addKeyDumper = true;
    if ( arguments.read("--mouse") )
        addMouseDumper = true;
    if ( !arguments.read("--autoclose", autoCloseTime) )
        autoCloseTime = -1;
    if ( !arguments.read("--viewpoint", viewpointNum) )
    {
        viewpointNum = -1;
        if ( !arguments.read("--viewpoint", viewpointName) )
            viewpointName.clear();
    }
    if ( !arguments.read("--effect", effectName) )
        effectName.clear();
    if ( !arguments.read("--numThreads", numThreads) )
        numThreads = -1;
    if ( !arguments.read("--numHttpThreads", numHttpThreads) )
        numHttpThreads = -1;

    std::string imagefilename;
    while (arguments.read("--image", imagefilename))
    {
        osg::ref_ptr<osg::Image> img = osgDB::readImageFile(imagefilename);
        if (img.valid())
        {
            QFileInfo fi(QString::fromStdString(imagefilename));
            QString base = fi.baseName();
            QImage qimg;
            osgImageToQImage(img.get(), &qimg);
            qimg.save(QString("/tmp/qt_%1.png").arg(base));
        }
    }

    /*
    std::cout << "showSceneGraphInspector=" << showSceneGraphInspector << std::endl;
    std::cout << "addKeyDumper=" << addKeyDumper << std::endl;
    std::cout << "addMouseDumper=" << addMouseDumper << std::endl;
    std::cout << "autoclose=" << autoCloseTime << std::endl;
    std::cout << "viewpointNum=" << viewpointNum << std::endl;
    std::cout << "viewpointName=" << viewpointName << std::endl;
    */

    osgViewer::View * view = myMainWindow->view();

    //Tell the database pager to not modify the unref settings
    view->getDatabasePager()->setUnrefImageDataAfterApplyPolicy( false, false );

    myMainWindow->createCamera();

    // configure the near/far so we don't clip things that are up close
    view->getCamera()->setNearFarRatio(0.00002);
    //view->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    TEVMapNodeHelper helper;
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags
    osg::Node* node = helper.load( arguments, view );
    if ( node )
    {
		osg::ref_ptr<osg::Group> root = new osg::Group;
		root->addChild(node);

        if(addSceneGraphInspector)
        {
            osg::ref_ptr<osgDB::Options> opts = osgDB::Registry::instance()->getOptions();
            if(opts)
                opts = opts->cloneOptions();
            else
                opts = new osgDB::Options;
            opts->setPluginStringData("showSceneGraphDialog", showSceneGraphInspector ? "1" : "0");
            osg::ref_ptr<osg::Node> sgi_loader = osgDB::readRefNodeFile(".sgi_loader", opts);
            if(sgi_loader.valid())
                root->addChild(sgi_loader);
            else
                QMessageBox::information(myMainWindow, QCoreApplication::applicationFilePath(), "Failed to load SGI");
        }

		myMainWindow->setData(root);

        if(fullscreen)
            myMainWindow->showFullScreen();
        else
        {
            myMainWindow->setGeometry(200, 200, 800, 600);
            myMainWindow->show();
        }
        if(autoCloseTime >= 0)
            setupWidgetAutoCloseTimer(myMainWindow, autoCloseTime);
        helper.setupInitialPosition(view, viewpointNum, viewpointName);

        if(addKeyDumper)
            view->addEventHandler(new KeyboardDumpHandler);
        if(addMouseDumper)
            view->addEventHandler(new MouseDumpHandler);

        QString filelist;
        for (const auto & f : helper.files())
        {
            if (!filelist.isEmpty())
                filelist += QChar(',');
            filelist += QString::fromStdString(f);
        }

        myMainWindow->setWindowTitle("tev - " + filelist);

        ret = app.exec();
        //viewer->removeView(view);
        delete myMainWindow;
    }
    else
    {
        QString msg;
        msg = QString::fromStdString(helper.errorMessages());
        msg += "\r\nCmdline: ";
        for(int i = 0; i < argc; i++)
        {
            if(i > 0)
                msg += ' ';
            msg += QString::fromLocal8Bit(argv[i]);
        }
        msg += "\r\n";
        msg += QString::fromStdString(TEVMapNodeHelper().usage());
#ifdef _WIN32
        // Only Windows we have to show a message box with the usage information, because
        // on Windoof a GUI application does not have a console output.
        if(allocConsole)
            std::cerr << msg << std::endl;
        QMessageBox::information(NULL, QCoreApplication::applicationFilePath(), msg);
#else
        std::cerr << msg.toStdString() << std::endl;
#endif
        ret = 1;
    }
    if(allocConsole)
    {
#ifdef _WIN32
        FreeConsole();
#endif // _WIN32
    }
    return ret;
}
