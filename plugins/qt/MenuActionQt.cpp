#include "stdafx.h"
#include "MenuActionQt.h"
#include "qt_string_helpers"
#include "SGIItemQt"

#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>

#include "SettingsDialogQt.h"
#include "ContextMenuQt.h"

#include <iostream>
#include <QMetaProperty>

namespace sgi {
namespace qt_plugin {

ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDumpObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDumpObjectTree)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectMethodInvoke)
ACTION_HANDLER_IMPL_REGISTER(MenuActionImagePreview)

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IContextMenuInfo * info = menuInfo();
    if(info)
        info->showSceneGraphDialog(_item->rootBase());
    else
    {
        ISceneGraphDialog * dlg = _hostInterface->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
        if(dlg)
            dlg->show();
    }
    return true;
}

namespace {
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, QtMsgType msgtype)
    {
        switch(msgtype)
        {
        case QtDebugMsg: os << "DEBUG"; break;
        case QtWarningMsg: os << "WARN"; break;
        case QtCriticalMsg: os << "CRIT"; break;
        case QtFatalMsg: os << "FATAL"; break;
        default: os << (int)msgtype; break;
        }
        return os;
    }

    class CaptureQtMessageHandler
    {
    public:
        CaptureQtMessageHandler()
        {
            _currentCaptureHandler = this;
#if QT_VERSION >= 0x050000
            _oldHandler = qInstallMessageHandler(CaptureQtMessageHandler::handler);
#else
            _oldHandler = qInstallMsgHandler(CaptureQtMessageHandler::handler);
#endif
        }
        ~CaptureQtMessageHandler()
        {
            _currentCaptureHandler = NULL;
            if(_oldHandler)
            {
#if QT_VERSION >= 0x050000
                qInstallMessageHandler(_oldHandler);
#else
                qInstallMsgHandler(_oldHandler);
#endif
            }
        }
        std::string messageText() const
        {
            return _stream.str();
        }

    private:
#if QT_VERSION >= 0x050000
        static void handler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
        static void handler(QtMsgType type, const char *msg)
#endif
        {
            if(_currentCaptureHandler)
            {
#if QT_VERSION >= 0x050000
                _currentCaptureHandler->_stream << type << ": " << (const char*)msg.toLocal8Bit().constData() << std::endl;
#else
                _currentCaptureHandler->_stream << type << ": " << msg << std::endl;
#endif
            }
            if(_oldHandler)
            {
                // call the original handler to get the original designed behavior
                // from Qt. this restores the assert handling
#if QT_VERSION >= 0x050000
                _oldHandler(type, context, msg);
#else
                _oldHandler(type, msg);
#endif
            }
        }
    private:
        static CaptureQtMessageHandler* _currentCaptureHandler;
#if QT_VERSION >= 0x050000
        static QtMessageHandler     _oldHandler;
#else
        static QtMsgHandler         _oldHandler;
#endif
        std::stringstream           _stream;
    };
    CaptureQtMessageHandler* CaptureQtMessageHandler::_currentCaptureHandler = NULL;
#if QT_VERSION >= 0x050000
    QtMessageHandler CaptureQtMessageHandler::_oldHandler = NULL;
#else
    QtMsgHandler CaptureQtMessageHandler::_oldHandler = NULL;
#endif
}

bool actionHandlerImpl<MenuActionDumpObjectInfo>::execute()
{
    QObject* object = getObject<QObject,SGIItemQt>();
    CaptureQtMessageHandler handler;
    object->dumpObjectInfo();
    std::cout << handler.messageText();
    return true;
}

bool actionHandlerImpl<MenuActionDumpObjectTree>::execute()
{
    QObject* object = getObject<QObject,SGIItemQt>();
    CaptureQtMessageHandler handler;
    object->dumpObjectTree();
    std::cout << handler.messageText();
    return true;
}

bool actionHandlerImpl<MenuActionObjectMethodInvoke>::execute()
{
    QObject* object = getObject<QObject,SGIItemQt>();
    ReferencedDataMetaMethod * userData = static_cast<ReferencedDataMetaMethod *>(menuAction()->userData());
    QMetaMethod & method = userData->data();
    method.invoke(object);
    return true;
}

bool actionHandlerImpl<MenuActionImagePreview>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogImagePreview, menu()->parentWidget());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
}

} // namespace qt_plugin
} // namespace sgi
