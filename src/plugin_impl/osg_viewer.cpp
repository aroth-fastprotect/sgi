
#ifdef _WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#elif defined(__linux__)
#include <osgViewer/api/X11/GraphicsWindowX11>
// undefine Status from X11/ICELIB
#undef Bool
#undef CursorShape
#undef None
#undef Status
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Expose
#endif

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#ifdef SGI_USE_OSGQT
#include <osgQt/GraphicsWindowQt>
#include <osgQt/GraphicsWindowQt5>
#endif

#include <QKeyEvent>
#include <QWindow>
#include <QWidget>
#ifdef _WIN32
#include <QCoreApplication>
#endif

#include <sgi/helpers/osg>

namespace sgi {
    namespace osg_helpers {

#ifdef _WIN32
class QtKeyboardMap
{

public:
    QtKeyboardMap()
    {
        mKeyMap[Qt::Key_Escape] = osgGA::GUIEventAdapter::KEY_Escape;
        mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Home] = osgGA::GUIEventAdapter::KEY_Home;
        mKeyMap[Qt::Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMap[Qt::Key_End] = osgGA::GUIEventAdapter::KEY_End;
        mKeyMap[Qt::Key_Return] = osgGA::GUIEventAdapter::KEY_Return;
        mKeyMap[Qt::Key_PageUp] = osgGA::GUIEventAdapter::KEY_Page_Up;
        mKeyMap[Qt::Key_PageDown] = osgGA::GUIEventAdapter::KEY_Page_Down;
        mKeyMap[Qt::Key_Left] = osgGA::GUIEventAdapter::KEY_Left;
        mKeyMap[Qt::Key_Right] = osgGA::GUIEventAdapter::KEY_Right;
        mKeyMap[Qt::Key_Up] = osgGA::GUIEventAdapter::KEY_Up;
        mKeyMap[Qt::Key_Down] = osgGA::GUIEventAdapter::KEY_Down;
        mKeyMap[Qt::Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
        mKeyMap[Qt::Key_Tab] = osgGA::GUIEventAdapter::KEY_Tab;
        mKeyMap[Qt::Key_Space] = osgGA::GUIEventAdapter::KEY_Space;
        mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Period] = osgGA::GUIEventAdapter::KEY_Period;
        mKeyMap[Qt::Key_Alt] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Shift] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Control] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Meta] = osgGA::GUIEventAdapter::KEY_Meta_L;

        mKeyMap[Qt::Key_Super_L] = osgGA::GUIEventAdapter::KEY_Super_L;
        mKeyMap[Qt::Key_Super_R] = osgGA::GUIEventAdapter::KEY_Super_R;
        mKeyMap[Qt::Key_Hyper_L] = osgGA::GUIEventAdapter::KEY_Hyper_L;
        mKeyMap[Qt::Key_Hyper_R] = osgGA::GUIEventAdapter::KEY_Hyper_R;

        mKeyMap[Qt::Key_CapsLock] = osgGA::GUIEventAdapter::KEY_Caps_Lock;
        mKeyMap[Qt::Key_NumLock] = osgGA::GUIEventAdapter::KEY_Num_Lock;
        mKeyMap[Qt::Key_ScrollLock] = osgGA::GUIEventAdapter::KEY_Scroll_Lock;

        mKeyMap[Qt::Key_hyphen] = '-';
        mKeyMap[Qt::Key_Equal] = '=';

        mKeyMap[Qt::Key_division] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMap[Qt::Key_multiply] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMap[Qt::Key_Minus] = '-';
        mKeyMap[Qt::Key_Plus] = '+';
        mKeyMap[Qt::Key_Insert] = osgGA::GUIEventAdapter::KEY_Insert;

        mKeyMapKeypad[Qt::Key_Slash] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMapKeypad[Qt::Key_Asterisk] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMapKeypad[Qt::Key_Minus] = osgGA::GUIEventAdapter::KEY_KP_Subtract;
        mKeyMapKeypad[Qt::Key_Plus] = osgGA::GUIEventAdapter::KEY_KP_Add;
        mKeyMapKeypad[Qt::Key_Period] = osgGA::GUIEventAdapter::KEY_Period;


        mKeyMapKeypad[Qt::Key_Home] = osgGA::GUIEventAdapter::KEY_KP_Home;
        mKeyMapKeypad[Qt::Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMapKeypad[Qt::Key_End] = osgGA::GUIEventAdapter::KEY_KP_End;
        mKeyMapKeypad[Qt::Key_PageUp] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
        mKeyMapKeypad[Qt::Key_PageDown] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
        mKeyMapKeypad[Qt::Key_Left] = osgGA::GUIEventAdapter::KEY_KP_Left;
        mKeyMapKeypad[Qt::Key_Right] = osgGA::GUIEventAdapter::KEY_KP_Right;
        mKeyMapKeypad[Qt::Key_Up] = osgGA::GUIEventAdapter::KEY_KP_Up;
        mKeyMapKeypad[Qt::Key_Down] = osgGA::GUIEventAdapter::KEY_KP_Down;
        mKeyMapKeypad[Qt::Key_Tab] = osgGA::GUIEventAdapter::KEY_KP_Tab;
        mKeyMapKeypad[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_KP_Delete;
        mKeyMapKeypad[Qt::Key_Insert] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        mKeyMapKeypad[Qt::Key_Clear] = osgGA::GUIEventAdapter::KEY_KP_Space;

    }

    ~QtKeyboardMap()
    {
    }

    int remapKey(QKeyEvent* event)
    {
        const int key = event->key();
        bool isKeypad = (event->modifiers() & Qt::KeypadModifier);

        const KeyMap & map = (isKeypad) ? mKeyMapKeypad : mKeyMap;
        KeyMap::const_iterator itr = map.find(key);
        if (itr == map.end())
        {
            if (key >= Qt::Key_0 && key <= Qt::Key_9)
            {
                if (isKeypad)
                    return osgGA::GUIEventAdapter::KEY_KP_0 + (key - Qt::Key_0);
                else
                    return osgGA::GUIEventAdapter::KEY_0 + (key - Qt::Key_0);
            }
            else if (key >= Qt::Key_A && key <= Qt::Key_Z)
            {
                return osgGA::GUIEventAdapter::KEY_A + (key - Qt::Key_A);
            }
            else if (key >= Qt::Key_F1 && key <= Qt::Key_F35)
            {
                if (isKeypad)
                    return osgGA::GUIEventAdapter::KEY_KP_F1 + (key - Qt::Key_F1);
                else
                    return osgGA::GUIEventAdapter::KEY_F1 + (key - Qt::Key_F1);
            }
            else
            {
                if (isKeypad)
                    return key;
                else
                    return int(*(event->text().toLocal8Bit().data()));
            }
        }
        else
            return itr->second;
    }

private:
    typedef std::map<unsigned int, int> KeyMap;
    KeyMap mKeyMap;
    KeyMap mKeyMapKeypad;
};

static QtKeyboardMap s_QtKeyboardMap;

class EventFilterWin32 : public QObject
{
    osgViewer::GraphicsWindow * _gw;
    QWidget * _widget;


    void setKeyboardModifiers(QInputEvent* event)
    {
        int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier);
        unsigned int mask = 0;
        if (modkey & Qt::ShiftModifier) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
        if (modkey & Qt::ControlModifier) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
        if (modkey & Qt::AltModifier) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
        if (modkey & Qt::MetaModifier) mask |= osgGA::GUIEventAdapter::MODKEY_META;
        _gw->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
    }

public:
    EventFilterWin32(osgViewer::GraphicsWindow * gw, QWidget * parent)
        : QObject(parent)
        , _gw(gw)
        , _widget(parent)
    {
    }
    bool eventFilter(QObject *obj, QEvent *event)
    {
        switch (event->type())
        {
        case QEvent::KeyPress:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            //_widget->event(keyEvent);
            setKeyboardModifiers(keyEvent);
            int value = s_QtKeyboardMap.remapKey(keyEvent);
            _gw->getEventQueue()->keyPress(value);
        }
        break;
        case QEvent::KeyRelease:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            //_widget->event(keyEvent);
            setKeyboardModifiers(keyEvent);
            int value = s_QtKeyboardMap.remapKey(keyEvent);
            _gw->getEventQueue()->keyRelease(value);
        }
        break;
        }
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
};
#endif // _WIN32

QWidget * getWidgetForGraphicsWindow(osgViewer::GraphicsWindow * gw, QWidget * parent)
{
    QWidget * ret = nullptr;
#ifdef SGI_USE_OSGQT
#ifdef OSGQT_ENABLE_QGLWIDGET
    osgQt::GraphicsWindowQt* gwq = dynamic_cast<osgQt::GraphicsWindowQt*>(gw);
    if (gwq)
        ret = gwq->getGLWidget();
    else 
#endif // OSGQT_ENABLE_QGLWIDGET
	if (osgQt::GraphicsWindowQt5* gwq = dynamic_cast<osgQt::GraphicsWindowQt5*>(gw))
    {
        if (gwq)
            ret = gwq->getOrCreateGLWidget();
    }
#endif
    if (!ret)
    {
#ifdef _WIN32
        osgViewer::GraphicsWindowWin32* gwin32 = dynamic_cast<osgViewer::GraphicsWindowWin32*>(gw);
        if (gwin32)
        {
            HWND hwnd = gwin32->getHWND();
            QWindow * wnd = QWindow::fromWinId((WId)hwnd);
            wnd->setFlags(Qt::ForeignWindow | Qt::MSWindowsOwnDC);
            QRect rect = wnd->geometry();
            ret = QWidget::createWindowContainer(wnd, parent);
            ret->setAttribute(Qt::WA_NativeWindow);
            ret->setFocusPolicy(Qt::WheelFocus);
            ret->setGeometry(rect);

            gwin32->setUserValue("sgi_ctx_widget", (double)(qulonglong)(void*)ret);
            QCoreApplication::instance()->installEventFilter(new EventFilterWin32(gwin32, ret));
        }
#elif defined(__linux__)
        osgViewer::GraphicsWindowX11* gwx11 = dynamic_cast<osgViewer::GraphicsWindowX11*>(gw);
        if (gwx11)
        {
            const Window & xwnd = gwx11->getWindow();
            const Window & xparent = gwx11->getParent();
            QWindow * wnd = QWindow::fromWinId((WId)xwnd);
            QWindow * wnd_parent = QWindow::fromWinId((WId)xparent);
            wnd->setFlags(Qt::ForeignWindow);
            wnd->setParent(wnd_parent);
            ret = QWidget::createWindowContainer(wnd, parent);
            ret->setAttribute(Qt::WA_NativeWindow);
            ret->setFocusPolicy(Qt::StrongFocus);
            gwx11->setUserValue("sgi_ctx_widget", (double)(qulonglong)(void*)ret);
        }
#endif
    }
    return ret;
}

bool createWidgetForGraphicsWindow(osg::GraphicsContext * ctx, QObject * parent)
{
    bool ret = false;
    osgViewer::GraphicsWindow * gw = dynamic_cast<osgViewer::GraphicsWindow*>(ctx);
    if (gw)
    {
        QWidget * widget = getWidgetForGraphicsWindow(gw, nullptr);
        if (widget)
        {
            widget->show();
            ret = (widget != nullptr);
        }
    }
    return ret;
}

    } // namespace osg_helpers
} // namespace sgi
