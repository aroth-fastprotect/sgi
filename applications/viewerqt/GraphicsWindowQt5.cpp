// Copyright (C) 2017 James Turner
// derived from OSG GraphicsWindowQt by Wang Rui
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <osg/DeleteHandler>
#include <osg/Version>

#include <osg/GL>
#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include "GraphicsWindowQt5.hxx"
#include <osgViewer/ViewerBase>
#include <QInputEvent>

#include <QDebug>
#include <QThread>

#include <QScreen>
#include <QGuiApplication>
#include <QAbstractEventDispatcher>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QTimer>

#include <QWidget>

#include <sstream>

#define GL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define GL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

using namespace flightgear;

namespace Qt {
    enum MissingQtKeys {
        Key_MissingQtKeys_Base = Key_unknown - 0x00100000,
        Key_Shift_L,
        Key_Shift_R,
        Key_Control_L,
        Key_Control_R,
        Key_Alt_L,
        Key_Alt_R,
    };
}


class QtKeyboardMap
{

public:
    QtKeyboardMap()
    {
        mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
        mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
        mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
        mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
        mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
        mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
        mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
        mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
        mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
        mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
        mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
        mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Period     ] = osgGA::GUIEventAdapter::KEY_Period;
        mKeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Meta     ] = osgGA::GUIEventAdapter::KEY_Meta_L;

        mKeyMap[Qt::Key_Super_L     ] = osgGA::GUIEventAdapter::KEY_Super_L;
        mKeyMap[Qt::Key_Super_R     ] = osgGA::GUIEventAdapter::KEY_Super_R;
        mKeyMap[Qt::Key_Hyper_L     ] = osgGA::GUIEventAdapter::KEY_Hyper_L;
        mKeyMap[Qt::Key_Hyper_R     ] = osgGA::GUIEventAdapter::KEY_Hyper_R;

        mKeyMap[Qt::Key_CapsLock       ] = osgGA::GUIEventAdapter::KEY_Caps_Lock;
        mKeyMap[Qt::Key_NumLock        ] = osgGA::GUIEventAdapter::KEY_Num_Lock;
        mKeyMap[Qt::Key_ScrollLock     ] = osgGA::GUIEventAdapter::KEY_Scroll_Lock;

        mKeyMap[Qt::Key_hyphen         ] = '-';
        mKeyMap[Qt::Key_Equal          ] = '=';

        mKeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMap[Qt::Key_Minus         ] = '-';
        mKeyMap[Qt::Key_Plus          ] = '+';
        mKeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_Insert;

        mKeyMap[Qt::Key_Shift_L] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Shift_R] = osgGA::GUIEventAdapter::KEY_Shift_R;
        mKeyMap[Qt::Key_Control_L] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Control_R] = osgGA::GUIEventAdapter::KEY_Control_R;
        mKeyMap[Qt::Key_Alt_L] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Alt_R] = osgGA::GUIEventAdapter::KEY_Alt_R;

        mKeyMapKeypad[Qt::Key_Slash         ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMapKeypad[Qt::Key_Asterisk      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMapKeypad[Qt::Key_Minus         ] = osgGA::GUIEventAdapter::KEY_KP_Subtract;
        mKeyMapKeypad[Qt::Key_Plus          ] = osgGA::GUIEventAdapter::KEY_KP_Add;
        mKeyMapKeypad[Qt::Key_Period        ] = osgGA::GUIEventAdapter::KEY_Period;
        

        mKeyMapKeypad[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_KP_Home;
        mKeyMapKeypad[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMapKeypad[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_KP_End;
        mKeyMapKeypad[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
        mKeyMapKeypad[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
        mKeyMapKeypad[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_KP_Left;
        mKeyMapKeypad[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_KP_Right;
        mKeyMapKeypad[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_KP_Up;
        mKeyMapKeypad[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_KP_Down;
        mKeyMapKeypad[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_KP_Tab;
        mKeyMapKeypad[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
        mKeyMapKeypad[Qt::Key_Insert     ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        mKeyMapKeypad[Qt::Key_Clear      ] = osgGA::GUIEventAdapter::KEY_KP_Space;
        
    }

    ~QtKeyboardMap()
    {
    }

    int remapKey(QKeyEvent* event)
    {
        int key = event->key();
        bool isKeypad = (event->modifiers() & Qt::KeypadModifier);
#ifdef _WIN32
        switch (key)
        {
        case Qt::Key_Shift:
        case Qt::Key_Alt:
        case Qt::Key_Control:
        {
            switch (event->nativeScanCode())
            {
            case 0x2a: key = Qt::Key_Shift_L; break;
            case 0x36: key = Qt::Key_Shift_R; break;
            case 0x1d: key = Qt::Key_Control_L; break;
            case 0x11d: key = Qt::Key_Control_R; break;
            case 0x38: key = Qt::Key_Alt_L; break;
            case 0x138: key = Qt::Key_Alt_R; break;
            }
        }
        break;
        }
#endif

        const KeyMap & map = (isKeypad)?mKeyMapKeypad:mKeyMap;
        KeyMap::const_iterator itr = map.find(key);
        if (itr == map.end())
        {
            if(key >= Qt::Key_0 && key <= Qt::Key_9)
            {
                if(isKeypad)
                    return osgGA::GUIEventAdapter::KEY_KP_0 + (key - Qt::Key_0);
                else
                    return osgGA::GUIEventAdapter::KEY_0 + (key - Qt::Key_0);
            }
            else if(key >= Qt::Key_A && key <= Qt::Key_Z)
            {
                return osgGA::GUIEventAdapter::KEY_A + (key - Qt::Key_A);
            }
            else if(key >= Qt::Key_F1 && key <= Qt::Key_F35)
            {
                if(isKeypad)
                    return osgGA::GUIEventAdapter::KEY_KP_F1 + (key - Qt::Key_F1);
                else
                    return osgGA::GUIEventAdapter::KEY_F1 + (key - Qt::Key_F1);
            }
            else
            {
                if(isKeypad)
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



GLWindow::GLWindow()
    : QWindow()
    , _graphicsThreadActive(false)
{
    _devicePixelRatio = 1.0;
    connect(this, &QWindow::screenChanged, this, &GLWindow::onScreenChanged);
    onScreenChanged();
}

void GLWindow::onScreenChanged()
{
    qWarning() << Q_FUNC_INFO << "screen changed";
    _devicePixelRatio = screen()->devicePixelRatio();

#if 0
    if (_isPrimaryWindow) {
        // allow PUI and Canvas to be scaled
        fgSetDouble("/sim/rendering/gui-pixel-ratio", _devicePixelRatio);
    }
#endif

    syncGeometryWithOSG();
}

void GLWindow::syncGeometryWithOSG()
{
    const int w = width();
    const int h = height();

    int scaled_width = static_cast<int>(w *_devicePixelRatio);
    int scaled_height = static_cast<int>(h*_devicePixelRatio);

    if (_gw) {
        _gw->resized( x(), y(), scaled_width,  scaled_height);
        _gw->getEventQueue()->windowResize( x(), y(), scaled_width, scaled_height );
        _gw->requestRedraw();
        _gw->_updateContextNeeded = true;
    }
}

GLWindow::~GLWindow()
{
    if (_gw) {
        _gw->_window = nullptr;
        _gw = nullptr;
    }
}

#if QT_VERSION < 0x050500
void GLWindow::requestUpdate()
{
    // mimic Qt 5.5's requestUpdate method
    QTimer::singleShot(0, this, SLOT(processUpdateEvent()));
}
#endif

bool GLWindow::event( QEvent* event )
{
    if (event->type() == QEvent::WindowStateChange) {
        // keep full-screen state in sync
        const bool isFullscreen = (windowState() == Qt::WindowFullScreen);
#if 0
        if (_isPrimaryWindow) {
            fgSetBool("/sim/startup/fullscreen", isFullscreen);
        }
#endif
    }
    else if (event->type() == QEvent::UpdateRequest)
    {
        processUpdateEvent();
    }
    else if (event->type() == QEvent::Close) {
        // spin an 'are you sure'? dialog here
        // need to decide immediately unfortunately.
        _gw->getEventQueue()->closeWindow();
    }

    // perform regular event handling
    return QWindow::event( event );
}

void GLWindow::processUpdateEvent()
{
    osg::ref_ptr<osgViewer::ViewerBase> v;
    if (_gw->_viewer.lock(v)) {
        v->frame();
    }

    // see discussion of QWindow::requestUpdate to see
    // why this is good behavior
    if (_gw->_continousUpdate) {
        requestUpdate();
    }

}

#ifdef _WIN32

enum WindowsNativeModifiers {
    ShiftLeft = 0x00000001,
    ControlLeft = 0x00000002,
    AltLeft = 0x00000004,
    MetaLeft = 0x00000008,
    ShiftRight = 0x00000010,
    ControlRight = 0x00000020,
    AltRight = 0x00000040,
    MetaRight = 0x00000080,
    CapsLock = 0x00000100,
    NumLock = 0x00000200,
    ScrollLock = 0x00000400,
    ExtendedKey = 0x01000000,

    // Convenience mappings
    ShiftAny = 0x00000011,
    ControlAny = 0x00000022,
    AltAny = 0x00000044,
    MetaAny = 0x00000088,
    LockAny = 0x00000700
};
#endif

void GLWindow::setKeyboardModifiers(QKeyEvent* event)
{
    unsigned int mask = 0;
#ifdef _WIN32
    quint32 modkey = event->nativeModifiers();
    if (modkey & ShiftLeft) mask |= osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT;
    if (modkey & ShiftRight) mask |= osgGA::GUIEventAdapter::MODKEY_RIGHT_SHIFT;
    if (modkey & ControlLeft) mask |= osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL;
    if (modkey & ControlRight) mask |= osgGA::GUIEventAdapter::MODKEY_RIGHT_CTRL;
    if (modkey & AltLeft) mask |= osgGA::GUIEventAdapter::MODKEY_LEFT_ALT;
    if (modkey & AltRight) mask |= osgGA::GUIEventAdapter::MODKEY_RIGHT_ALT;
    if (modkey & MetaLeft) mask |= osgGA::GUIEventAdapter::MODKEY_LEFT_META;
    if (modkey & MetaRight) mask |= osgGA::GUIEventAdapter::MODKEY_RIGHT_META;
#else
    int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier);
    if (modkey & Qt::ShiftModifier) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if (modkey & Qt::ControlModifier) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if (modkey & Qt::AltModifier) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if (modkey & Qt::MetaModifier) mask |= osgGA::GUIEventAdapter::MODKEY_META;
#endif
    _gw->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

void GLWindow::setKeyboardModifiers( QInputEvent* event )
{
    int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier);
    unsigned int mask = 0;
    if ( modkey & Qt::ShiftModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
    if ( modkey & Qt::ControlModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
    if ( modkey & Qt::AltModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
    if ( modkey & Qt::MetaModifier ) mask |= osgGA::GUIEventAdapter::MODKEY_META;
    _gw->getEventQueue()->getCurrentEventState()->setModKeyMask( mask );
}

void GLWindow::resizeEvent( QResizeEvent* event )
{
    QWindow::resizeEvent(event);
    syncGeometryWithOSG();
}

void GLWindow::moveEvent( QMoveEvent* event )
{
    QWindow::moveEvent(event);
    syncGeometryWithOSG();
}

void GLWindow::keyPressEvent( QKeyEvent* event )
{
    setKeyboardModifiers(event);
    int value = s_QtKeyboardMap.remapKey( event );
    _gw->getEventQueue()->keyPress( value );

    // this passes the event to the regular Qt key event processing,
    // among others, it closes popup windows on ESC and forwards the event to the parent widgets
    if( _forwardKeyEvents )
        inherited::keyPressEvent( event );
}

void GLWindow::keyReleaseEvent( QKeyEvent* event )
{
    if (event->isAutoRepeat()) {
        event->ignore();
    } else {
        setKeyboardModifiers(event);
        int value = s_QtKeyboardMap.remapKey( event );
        _gw->getEventQueue()->keyRelease( value );
    }

    // this passes the event to the regular Qt key event processing,
    // among others, it closes popup windows on ESC and forwards the event to the parent widgets
    if( _forwardKeyEvents )
        inherited::keyReleaseEvent( event );
}

void GLWindow::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    setKeyboardModifiers(event);
    _gw->getEventQueue()->mouseButtonPress( event->x()*_devicePixelRatio, event->y()*_devicePixelRatio, button );
}

void GLWindow::mouseReleaseEvent( QMouseEvent* event )
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    setKeyboardModifiers(event);
    _gw->getEventQueue()->mouseButtonRelease( event->x()*_devicePixelRatio, event->y()*_devicePixelRatio, button );
}

void GLWindow::mouseDoubleClickEvent( QMouseEvent* event )
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    setKeyboardModifiers(event);
    _gw->getEventQueue()->mouseDoubleButtonPress( event->x()*_devicePixelRatio, event->y()*_devicePixelRatio, button );
}

void GLWindow::mouseMoveEvent( QMouseEvent* event )
{
    setKeyboardModifiers(event);
    _gw->getEventQueue()->mouseMotion( event->x()*_devicePixelRatio, event->y()*_devicePixelRatio );
}

void GLWindow::wheelEvent( QWheelEvent* event )
{
    setKeyboardModifiers(event);
    _gw->getEventQueue()->mouseScroll(
        event->orientation() == Qt::Vertical ?
            (event->delta()>0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN) :
            (event->delta()>0 ? osgGA::GUIEventAdapter::SCROLL_LEFT : osgGA::GUIEventAdapter::SCROLL_RIGHT) );
}

void GLWindow::moveGLContextToThread(QThread * targetThread)
{
    QOpenGLContext* ctx = _gw->_context.get();
    if (ctx)
    {
        if (this->thread() == targetThread)
        {
            _graphicsThreadActive = false;
        }
        else
        {
            _graphicsThreadActive = true;
        }
        ctx->moveToThread(targetThread);
    }
}

GraphicsThreadQt5::GraphicsThreadQt5(GraphicsWindowQt5 * gw)
    : osg::GraphicsThread()
    , _gw(gw)
{
    qWarning() << Q_FUNC_INFO;
}

void GraphicsThreadQt5::prepareStart()
{
    GLWindow * window = _gw->getGLWindow();
    qWarning() << Q_FUNC_INFO;
    window->moveGLContextToThread(static_cast<QThread*>(getImplementation()));
}

void GraphicsThreadQt5::run()
{
    Thread::setCurrentThreadName("GFXQt5");

    // QOpenGLContext already belongs to this thread
    GLWindow * window = _gw->getGLWindow();
    GraphicsThread::run();
    // moving the QOpenGLContext instance back to the thread of
    // the widget (which is the original owning thread)
    window->moveGLContextToThread(window->thread());
}

GraphicsWindowQt5::GraphicsWindowQt5(osg::GraphicsContext::Traits* traits)
{
    _traits = traits;
    init(0);
}

GraphicsWindowQt5::~GraphicsWindowQt5()
{
    OSG_INFO << "destroying GraphicsWindowQt5" << std::endl;
    close();
}

bool GraphicsWindowQt5::init( Qt::WindowFlags f )
{
    // update _widget and parent by WindowData
    WindowData* windowData = _traits.get() ? dynamic_cast<WindowData*>(_traits->inheritedWindowData.get()) : 0;
    Q_ASSERT(!_window);
    _ownsWidget = true;

    // WindowFlags
    Qt::WindowFlags flags = f | Qt::Window | Qt::CustomizeWindowHint;
    if ( _traits->windowDecoration ) {
        flags |= Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint;
        flags |= Qt::WindowFullscreenButtonHint;

        // TODO - check if this is desirable or not on Windows+Linux
        //flags |= Qt::MaximizeUsingFullscreenGeometryHint;
    }

    // create window
    _window.reset(new GLWindow);
    _window->setFlags(flags);
    _window->setSurfaceType(QSurface::OpenGLSurface);
    _window->setFormat(traits2qSurfaceFormat(_traits.get()));
    _window->create();
    _window->setTitle( _traits->windowName.c_str() );

    // to get OS-dependant default positioning of the window (which is desirable),
    // we must take care to only set the position if explicitly requested.
    // hence we set X & Y to these marker values by default.
    // And hence only set position if both are valid.
    if ((_traits->x != std::numeric_limits<int>::max()) && (_traits->y != std::numeric_limits<int>::max())) {
        _window->setPosition( _traits->x, _traits->y );
    }

    QSize sz(_traits->width, _traits->height);
    if ( !_traits->supportsResize ) {
      _window->setMinimumSize( sz );
      _window->setMaximumSize( sz );
    } else {
      _window->resize( sz );
    }

    if(windowData)
    {
        if (windowData->createFullscreen) {
            // this doesn't seem to actually work, so we
            // check the flag again in realizeImplementation()
            _window->setWindowState(Qt::WindowFullScreen);
        }

        _window->_isPrimaryWindow = windowData->isPrimaryWindow;
    }
#if 0
    if (_window->_isPrimaryWindow) {
        fgSetDouble("/sim/rendering/gui-pixel-ratio", _window->_devicePixelRatio);
    }
#endif

    _window->setGraphicsWindow( this );
    useCursor( _traits->useCursor );

    // initialize State
    setState( new osg::State );
    getState()->setGraphicsContext(this);
    getState()->setContextID( osg::GraphicsContext::createNewContextID() );

#if (OPENSCENEGRAPH_MAJOR_VERSION == 3) && (OPENSCENEGRAPH_MINOR_VERSION >= 4)
    // make sure the event queue has the correct window rectangle size and input range
    getEventQueue()->syncWindowRectangleWithGraphicsContext();
#endif

    return true;
}

QSurfaceFormat GraphicsWindowQt5::traits2qSurfaceFormat( const osg::GraphicsContext::Traits* traits )
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);

    format.setAlphaBufferSize( traits->alpha );
    format.setRedBufferSize( traits->red );
    format.setGreenBufferSize( traits->green );
    format.setBlueBufferSize( traits->blue );
    format.setDepthBufferSize( traits->depth );
    format.setStencilBufferSize( traits->stencil );
  //  format.setSampleBuffers( traits->sampleBuffers );
    format.setSamples( traits->samples );

    format.setAlphaBufferSize( traits->alpha>0 );
    format.setDepthBufferSize( traits->depth );

    format.setSwapBehavior( traits->doubleBuffer ?
        QSurfaceFormat::DoubleBuffer :
        QSurfaceFormat::DefaultSwapBehavior);
    format.setSwapInterval( traits->vsync ? 1 : 0 );
    format.setStereo( traits->quadBufferStereo ? 1 : 0 );

    unsigned major, minor;
    traits->getContextVersion(major, minor);
    format.setVersion((int)major, (int)minor);

    QSurfaceFormat::OpenGLContextProfile profile = QSurfaceFormat::NoProfile;
    if (traits->glContextProfileMask & GL_CONTEXT_CORE_PROFILE_BIT_ARB)
    {
        profile = QSurfaceFormat::CoreProfile;
        format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    }
    else if (traits->glContextProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB)
    {
        profile = QSurfaceFormat::CompatibilityProfile;
        format.setOption(QSurfaceFormat::DeprecatedFunctions, true);
    }

    format.setProfile(profile);

    /*
    if (traits->glContextFlags & GL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB)
        format.setOption(QGL::DebugContext);
        */

    if (traits->glContextFlags & GL_CONTEXT_DEBUG_BIT_ARB)
        format.setOption(QSurfaceFormat::DebugContext);


    return format;
}

void GraphicsWindowQt5::qSurfaceFormat2traits( const QSurfaceFormat& format, osg::GraphicsContext::Traits* traits )
{
    traits->red = format.redBufferSize();
    traits->green = format.greenBufferSize();
    traits->blue = format.blueBufferSize();
    traits->alpha = format.alphaBufferSize();
    traits->depth = format.depthBufferSize();
    traits->stencil = format.stencilBufferSize();
    traits->samples = format.samples();

    traits->quadBufferStereo = format.stereo();
    traits->doubleBuffer = (format.swapBehavior() == QSurfaceFormat::DoubleBuffer);
    traits->vsync = format.swapInterval() >= 1;

    std::stringstream ss;
    ss << format.majorVersion() << '.' << format.minorVersion();
    traits->glContextVersion = ss.str();

    switch(format.profile())
    {
    default:
    case QSurfaceFormat::NoProfile:
        traits->glContextProfileMask = 0;
        break;
    case QSurfaceFormat::CoreProfile:
        traits->glContextProfileMask = GL_CONTEXT_CORE_PROFILE_BIT_ARB;
        break;
    case QSurfaceFormat::CompatibilityProfile:
        traits->glContextProfileMask = GL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        break;
    }
}

osg::GraphicsContext::Traits* GraphicsWindowQt5::createTraits( const QWindow* window )
{
    osg::GraphicsContext::Traits *traits = new osg::GraphicsContext::Traits;

    qSurfaceFormat2traits( window->format(), traits );

    QRect r = window->geometry();
    traits->x = r.x();
    traits->y = r.y();
    traits->width = r.width();
    traits->height = r.height();

    traits->windowName = window->title().toLocal8Bit().data();
    Qt::WindowFlags f = window->flags();
    traits->windowDecoration = ( f & Qt::WindowTitleHint ) &&
                            ( f & Qt::WindowMinMaxButtonsHint ) &&
                            ( f & Qt::WindowSystemMenuHint );

#if 0
    QSizePolicy sp = window->sizePolicy();
    traits->supportsResize = sp.horizontalPolicy() != QSizePolicy::Fixed ||
                            sp.verticalPolicy() != QSizePolicy::Fixed;
#else
    traits->supportsResize = true;
#endif

    return traits;
}

bool GraphicsWindowQt5::setWindowRectangleImplementation( int x, int y, int width, int height )
{
    if (!_window)
        return false;
#if 0
    qDebug() << "setWRI window geometry to " << x << "," << y <<
      " w=" << width << " h=" << height;
#endif
    _window->setGeometry( x, y, width, height );
    return true;
}

void GraphicsWindowQt5::getWindowRectangle( int& x, int& y, int& width, int& height )
{
    if ( _window )
    {
        const QRect& geom = _window->geometry();
        x = geom.x();
        y = geom.y();
        width = geom.width();
        height = geom.height();
    }
}

bool GraphicsWindowQt5::setWindowDecorationImplementation( bool windowDecoration )
{
    Qt::WindowFlags flags = Qt::Window|Qt::CustomizeWindowHint;//|Qt::WindowStaysOnTopHint;
    if ( windowDecoration )
        flags |= Qt::WindowTitleHint|Qt::WindowMinMaxButtonsHint|Qt::WindowSystemMenuHint;
    _traits->windowDecoration = windowDecoration;

    if ( _window )
    {
        _window->setFlags( flags );
        return true;
    }

    return false;
}

bool GraphicsWindowQt5::getWindowDecoration() const
{
    return _traits->windowDecoration;
}

void GraphicsWindowQt5::grabFocus()
{
    if ( _window )
        _window->requestActivate();
}

void GraphicsWindowQt5::grabFocusIfPointerInWindow()
{
  #if 0
    if ( _widget->underMouse() )
        _widget->setFocus( Qt::ActiveWindowFocusReason );
    #endif
}

void GraphicsWindowQt5::raiseWindow()
{
    if ( _window )
        _window->raise();
}

void GraphicsWindowQt5::setWindowName( const std::string& name )
{
    if ( _window )
        _window->setTitle( QString::fromUtf8(name.c_str()) );
}

std::string GraphicsWindowQt5::getWindowName()
{
    return _window ? _window->title().toStdString() : "";
}

void GraphicsWindowQt5::useCursor( bool cursorOn )
{
    if ( _window )
    {
        _traits->useCursor = cursorOn;
        if ( !cursorOn ) _window->setCursor( Qt::BlankCursor );
        else _window->setCursor( _currentCursor );
    }
}

void GraphicsWindowQt5::setCursor( MouseCursor cursor )
{
    if ( cursor==InheritCursor && _window )
    {
        _window->unsetCursor();
    }

    switch ( cursor )
    {
    case NoCursor: _currentCursor = Qt::BlankCursor; break;
    case RightArrowCursor: case LeftArrowCursor: _currentCursor = Qt::ArrowCursor; break;
    case InfoCursor: _currentCursor = Qt::SizeAllCursor; break;
    case DestroyCursor: _currentCursor = Qt::ForbiddenCursor; break;
    case HelpCursor: _currentCursor = Qt::WhatsThisCursor; break;
    case CycleCursor: _currentCursor = Qt::ForbiddenCursor; break;
    case SprayCursor: _currentCursor = Qt::SizeAllCursor; break;
    case WaitCursor: _currentCursor = Qt::WaitCursor; break;
    case TextCursor: _currentCursor = Qt::IBeamCursor; break;
    case CrosshairCursor: _currentCursor = Qt::CrossCursor; break;
    case HandCursor: _currentCursor = Qt::OpenHandCursor; break;
    case UpDownCursor: _currentCursor = Qt::SizeVerCursor; break;
    case LeftRightCursor: _currentCursor = Qt::SizeHorCursor; break;
    case TopSideCursor: case BottomSideCursor: _currentCursor = Qt::UpArrowCursor; break;
    case LeftSideCursor: case RightSideCursor: _currentCursor = Qt::SizeHorCursor; break;
    case TopLeftCorner: _currentCursor = Qt::SizeBDiagCursor; break;
    case TopRightCorner: _currentCursor = Qt::SizeFDiagCursor; break;
    case BottomRightCorner: _currentCursor = Qt::SizeBDiagCursor; break;
    case BottomLeftCorner: _currentCursor = Qt::SizeFDiagCursor; break;
    default: break;
    };
    if ( _window ) _window->setCursor( _currentCursor );
}

bool GraphicsWindowQt5::valid() const
{
    return _window.get() != nullptr;
}

void GraphicsWindowQt5::createGraphicsThread()
{

    qWarning() << Q_FUNC_INFO << _graphicsThread;
    //return osgViewer::GraphicsWindow::createGraphicsThread();
    if (!_graphicsThread)
    {
        setGraphicsThread(new GraphicsThreadQt5(this));
    }
    // do not call osgViewer::GraphicsWindow::createGraphicsThread()
    // here, because this would create a default graphic thread
    // which does not handle the gfx context moving and results in
    // assertions
}

bool GraphicsWindowQt5::realizeImplementation()
{
    WindowData* windowData = _traits.get() ? dynamic_cast<WindowData*>(_traits->inheritedWindowData.get()) : nullptr;
    if (windowData && windowData->createFullscreen) {
        _window->showFullScreen();
        // work-around for resize events being discarded by EventQueue::setStartTick
        _sendResizeOnEventCheck = true;
    } else {
        _window->show();
    }

#if (OPENSCENEGRAPH_MAJOR_VERSION == 3) && (OPENSCENEGRAPH_MINOR_VERSION >= 4)
    // make sure the event queue has the correct window rectangle size and input range
    getEventQueue()->syncWindowRectangleWithGraphicsContext();
#endif

    getEventQueue()->getCurrentEventState()->setMouseYOrientation(osgGA::GUIEventAdapter::Y_INCREASING_DOWNWARDS);

    _realized = true;
    return true;
}

bool GraphicsWindowQt5::isRealizedImplementation() const
{
    return _realized;
}

void GraphicsWindowQt5::closeImplementation()
{
    if ( _window ) {
        _window->close();
        _window.reset();
    }

    _context.reset();
    _realized = false;
}

void GraphicsWindowQt5::runOperations()
{
    //qWarning() << Q_FUNC_INFO << _updateContextNeeded << QOpenGLContext::currentContext() << _context.get();

    // While in graphics thread this is last chance to do something useful before
    // graphics thread will execute its operations.
    if (_updateContextNeeded || (QOpenGLContext::currentContext() != _context.get())) {
        makeCurrent();
        _updateContextNeeded = false;
    }

    _window->beforeRendering();

    GraphicsWindow::runOperations();

    _window->afterRendering();
}

bool GraphicsWindowQt5::makeCurrentImplementation()
{
    if (!_context) {
        if ( _traits->sharedContext.valid() ) {
            qWarning() << Q_FUNC_INFO << "share contexts not supported";
        }

        _context.reset(new QOpenGLContext());
        _context->setFormat(_window->format());
        bool result = _context->create();
        if (!result)
        {
          OSG_WARN << "GraphicsWindowQt5: Can't create QOpenGLContext'" << std::endl;
          return false;
        }

        _context->makeCurrent(_window.get());

        this->setDefaultFboId(_context->defaultFramebufferObject());

        // allow derived classes to do work now the context is initialized
        contextInitalised();
    }

    if (_context && (QThread::currentThread() != _context->thread())) {
      qWarning() << "attempt to make context current on wrong thread";
      return false;
    }

    _context->makeCurrent(_window.get());
    return true;
}

bool GraphicsWindowQt5::releaseContextImplementation()
{
    _context->doneCurrent();
    return true;
}

void GraphicsWindowQt5::swapBuffersImplementation()
{
    if (!_window->isExposed())
        return;

    _context->swapBuffers(_window.get());
}

void GraphicsWindowQt5::requestWarpPointer( float x, float y )
{
    if ( _window )
        QCursor::setPos( _window->mapToGlobal(QPoint((int)x,(int)y)) );

    getEventQueue()->mouseWarped(x,y);        
}

bool GraphicsWindowQt5::checkEvents()
{
    if (_sendResizeOnEventCheck) {
        _sendResizeOnEventCheck = false;
        _window->syncGeometryWithOSG();
    }
#if 0
// todo - only if not running inside QApplication::exec; can we check this?
    QCoreApplication::processEvents(QEventLoop::AllEvents);
#endif
    return true;
}

void GraphicsWindowQt5::setViewer(osgViewer::ViewerBase* viewer)
{
     osg::ref_ptr<osgViewer::ViewerBase> previous(_viewer);
    _viewer = viewer;
    viewerChanged(previous.get());
}

void GraphicsWindowQt5::viewerChanged(osgViewer::ViewerBase*)
{
    // nothing
}

void GraphicsWindowQt5::requestRedraw()
{
  _window->requestUpdate();
}

void GraphicsWindowQt5::requestContinuousUpdate(bool needed)
{
    _continousUpdate = needed;
    GraphicsWindow::requestContinuousUpdate(needed);
    if (_continousUpdate) {
        _window->requestUpdate();
    }
}

void GraphicsWindowQt5::setFullscreen(bool isFullscreen)
{
    if (isFullscreen) {
        _window->showFullScreen();
    } else {
        // FIXME should restore previous state?
        _window->showNormal();
    }
}

QWidget* GraphicsWindowQt5::getOrCreateGLWidget()
{
    if (!_widget.get() && _window.get())
        _widget.reset(QWidget::createWindowContainer(_window.get()));
    return _widget.get();
}


class Qt5WindowingSystem : public osg::GraphicsContext::WindowingSystemInterface
{
public:

    Qt5WindowingSystem()
    {
    }

    ~Qt5WindowingSystem()
    {
        if (osg::Referenced::getDeleteHandler())
        {
            osg::Referenced::getDeleteHandler()->setNumFramesToRetainObjects(0);
            osg::Referenced::getDeleteHandler()->flushAll();
        }
    }

    // Access the Qt windowing system through this singleton class.
    static Qt5WindowingSystem* getInterface()
    {
        static Qt5WindowingSystem* qtInterface = new Qt5WindowingSystem;
        return qtInterface;
    }

    // Return the number of screens present in the system
    virtual unsigned int getNumScreens( const osg::GraphicsContext::ScreenIdentifier& /*si*/ )
    {
        return QGuiApplication::screens().size();
    }

    // Return the resolution of specified screen
    // (0,0) is returned if screen is unknown
    virtual void getScreenSettings( const osg::GraphicsContext::ScreenIdentifier& si,
      osg::GraphicsContext::ScreenSettings& resolution )
    {
      QScreen* screen = qScreenFromSI(si);
      if (!screen) {
          qWarning() << Q_FUNC_INFO << "no screen for identifier" << QString::fromStdString(si.displayName());
          return;
      }

      resolution.width = screen->size().width();
      resolution.height = screen->size().height();
      resolution.colorDepth = screen->depth();
      resolution.refreshRate = screen->refreshRate();
    }

    // Set the resolution for given screen
    virtual bool setScreenSettings( const osg::GraphicsContext::ScreenIdentifier& /*si*/, const osg::GraphicsContext::ScreenSettings & /*resolution*/ )
    {
        OSG_WARN << "osgQt: setScreenSettings() not implemented yet." << std::endl;
        return false;
    }

    // Enumerates available resolutions
    virtual void enumerateScreenSettings( const osg::GraphicsContext::ScreenIdentifier& si,
      osg::GraphicsContext::ScreenSettingsList & resolutions )
    {
      QScreen* screen = qScreenFromSI(si);
      if (!screen) {
          qWarning() << Q_FUNC_INFO << "no screen for identifier" << QString::fromStdString(si.displayName());
          return;
      }

      resolutions.clear();
      osg::GraphicsContext::ScreenSettings ss;
      ss.width = screen->size().width();
      ss.height = screen->size().height();
      ss.colorDepth = screen->depth();
      ss.refreshRate = screen->refreshRate();
      resolutions.push_back(ss);
    }

    // Create a graphics context with given traits
    virtual osg::GraphicsContext* createGraphicsContext( osg::GraphicsContext::Traits* traits )
    {
        if (traits->pbuffer)
        {
            OSG_WARN << "osgQt: createGraphicsContext - pbuffer not implemented yet." << std::endl;
            return NULL;
        }
        else
        {
            osg::ref_ptr< GraphicsWindowQt5 > window = new GraphicsWindowQt5( traits );
            if (window->valid()) {
              return window.release();
            }
            else {
              qWarning() << "window is not valid";
              return NULL;
            }
        }
    }

private:
    QScreen* qScreenFromSI(const osg::GraphicsContext::ScreenIdentifier& si)
    {
        QList<QScreen*> screens = QGuiApplication::screens();
        if (si.screenNum >= 0 && si.screenNum < screens.size()) {
            return screens.at(si.screenNum);
        }

        return QGuiApplication::primaryScreen();
    }

    // No implementation for these
    Qt5WindowingSystem( const Qt5WindowingSystem& );
    Qt5WindowingSystem& operator=( const Qt5WindowingSystem& );
};

namespace flightgear
{

void initQtWindowingSystem()
{
#if OSG_VERSION_LESS_THAN(3,5,2)
    osg::GraphicsContext::setWindowingSystemInterface(Qt5WindowingSystem::getInterface());
#endif
}

} // of namespace flightgear
