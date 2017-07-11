#include "stdafx.h"
#include "MenuActionQt.h"
#include "SGIItemQt"

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ImagePreviewDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/helpers/qt>
#include <sgi/helpers/qt_widgets>

#include "SettingsDialogQt.h"
#include "ContextMenuQt.h"

#include <iostream>
#include <cassert>
#include <QMetaProperty>

namespace sgi {
namespace qt_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDumpObjectInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionDumpObjectTree)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetGrab)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetSetVisibility)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetSetEnabled)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetSetAcceptDrops)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetSetAutoFillBackground)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionWidgetHighlight)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectMethodInvoke)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectModifyProperty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImagePreview)

using namespace sgi::qt_helpers;

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
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
                _currentCaptureHandler->_stream << type << ": " << (const char*)msg.toUtf8().constData() << std::endl;
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

    const sgi::Image * convertImage(QImage * image)
    {
        if (!image)
            return NULL;
        sgi::Image * ret = new sgi::Image(image);
        return ret;
    }
    const sgi::Image * convertImage(QPixmap * image)
    {
        if (!image)
            return NULL;
        QImage qimg = image->toImage();
        return convertImage(&qimg);
    }
    const sgi::Image * convertImage(QIcon * image)
    {
        if (!image)
            return NULL;
        QList<QSize> sizes = image->availableSizes();
        if (sizes.empty())
            return NULL;
        QPixmap pixmap = image->pixmap(sizes.front());
        return convertImage(&pixmap);
    }
}

bool actionHandlerImpl<MenuActionDumpObjectInfo>::execute()
{
    QObject* object = getObject<QObject,SGIItemQt>();
    CaptureQtMessageHandler handler;
    object->dumpObjectInfo();
    std::string text = handler.messageText();
    _hostInterface->inputDialogText(menu()->parentWidget(), text, "Object info", "Object info", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item.get());
    return true;
}

bool actionHandlerImpl<MenuActionDumpObjectTree>::execute()
{
    QObject* object = getObject<QObject,SGIItemQt>();
    CaptureQtMessageHandler handler;
    object->dumpObjectTree();
    std::string text = handler.messageText();
    _hostInterface->inputDialogText(menu()->parentWidget(), text, "Object tree", "Object tree", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item.get());
    return true;
}

bool actionHandlerImpl<MenuActionWidgetGrab>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    QPixmap * pixmap = new QPixmap(object->grab());
    SGIHostItemQtPaintDevice pixmapHostItem(pixmap);

    SGIItemBasePtr pixmapItem;
    _hostInterface->generateItem(pixmapItem, &pixmapHostItem);

    IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), pixmapItem.get());

    if(dialog.valid())
    {
        dialog->setObject(pixmapItem.get(), hostCallback());
        dialog->show();
    }
    return true;
}

bool actionHandlerImpl<MenuActionWidgetSetVisibility>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    object->setVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionWidgetSetEnabled>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    object->setEnabled(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionWidgetSetAcceptDrops>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    object->setAcceptDrops(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionWidgetSetAutoFillBackground>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    object->setAutoFillBackground(menuAction()->state());
    return true;
}


bool actionHandlerImpl<MenuActionWidgetHighlight>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    new qt_helpers::HighlightWidget(object);
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

bool actionHandlerImpl<MenuActionObjectModifyProperty>::execute()
{
    QObject* object = getObject<QObject, SGIItemQt>();
    ReferencedDataString * userData = static_cast<ReferencedDataString *>(menuAction()->userData());
    const std::string & propertyName = userData->data();
    QVariant propertyValue = object->property(propertyName.c_str());

    switch(propertyValue.type())
    {
    case QVariant::Color:
        {
            QColor qcolor = propertyValue.value<QColor>();
            sgi::Color color = qtColor(qcolor);
            bool gotInput = _hostInterface->inputDialogColor(menuAction()->menu()->parentWidget(),
                color, propertyName, "Set property " + propertyName,
                _item);
            if(gotInput)
            {
                qcolor = qtColor(color);
                propertyValue = QVariant::fromValue(qcolor);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::Bool:
        {
            bool propertyValueBool = propertyValue.value<bool>();
            std::string propertyValueString = propertyValueBool ? "true" : "false";
            bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(),
                propertyValueString, propertyName, "Set property " + propertyName,
                SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
            if (gotInput)
            {
                if (propertyValueString == "true" || propertyValueString == "on" || propertyValueString == "yes" || propertyValueString == "1")
                    propertyValueBool = true;
                else if (propertyValueString == "false" || propertyValueString == "off" || propertyValueString == "no" || propertyValueString == "0")
                    propertyValueBool = false;
                propertyValue = QVariant::fromValue(propertyValueBool);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::Int:
        {
            int propertyValueInt = propertyValue.value<int>();
            bool gotInput = _hostInterface->inputDialogInteger(menuAction()->menu()->parentWidget(),
                propertyValueInt, propertyName, "Set property " + propertyName, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1, _item);
            if (gotInput)
            {
                propertyValue = QVariant::fromValue(propertyValueInt);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::UInt:
        {
            uint propertyValueInt = propertyValue.value<uint>();
            bool gotInput = _hostInterface->inputDialogInteger(menuAction()->menu()->parentWidget(),
                (int&)propertyValueInt, propertyName, "Set property " + propertyName, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1, _item);
            if (gotInput)
            {
                propertyValue = QVariant::fromValue(propertyValueInt);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::LongLong:
        {
            qlonglong propertyValueInt = propertyValue.value<qlonglong>();
            bool gotInput = _hostInterface->inputDialogInteger64(menuAction()->menu()->parentWidget(),
                (int64_t&)propertyValueInt, propertyName, "Set property " + propertyName, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), 1, _item);
            if (gotInput)
            {
                propertyValue = QVariant::fromValue(propertyValueInt);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::ULongLong:
        {
            qulonglong propertyValueInt = propertyValue.value<qulonglong>();
            bool gotInput = _hostInterface->inputDialogInteger64(menuAction()->menu()->parentWidget(),
                (int64_t&)propertyValueInt, propertyName, "Set property " + propertyName, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), 1, _item);
            if (gotInput)
            {
                propertyValue = QVariant::fromValue(propertyValueInt);
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::String:
        {
            std::string propertyValueString = toUtf8(propertyValue.toString());

            bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(),
                                                            propertyValueString, propertyName, "Set property " + propertyName,
                                                            SGIPluginHostInterface::InputDialogStringEncodingUTF8, _item);
            if(gotInput)
            {
                propertyValue.setValue(fromUtf8(propertyValueString));
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    case QVariant::Pixmap:
        {
            IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());
            if (dialog.valid())
            {
                QPixmap img = propertyValue.value<QPixmap>();
                dialog->setObject(_item.get(), convertImage(&img), std::string(), hostCallback());
                dialog->show();
            }
        }
        break;
    case QVariant::Image:
        {
            IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());
            if (dialog.valid())
            {
                QImage img = propertyValue.value<QImage>();
                dialog->setObject(_item.get(), convertImage(&img), std::string(), hostCallback());
                dialog->show();
            }
        }
        break;
    case QVariant::Icon:
        {
            IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());
            if (dialog.valid())
            {
                QIcon img = propertyValue.value<QIcon>();
                dialog->setObject(_item.get(), convertImage(&img), std::string(), hostCallback());
                dialog->show();
            }
        }
        break;
    default:
        {
            std::stringstream ss;
            ss << propertyValue;
            std::string propertyValueString = ss.str();

            bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(),
                                                            propertyValueString, propertyName, "Set property " + propertyName,
                                                            SGIPluginHostInterface::InputDialogStringEncodingUTF8, _item);
            if(gotInput)
            {
                propertyValue.setValue(fromUtf8(propertyValueString));
                object->setProperty(propertyName.c_str(), propertyValue);
            }
        }
        break;
    }
    return true;
}

bool actionHandlerImpl<MenuActionImagePreview>::execute()
{
    IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), _item.get());
	if (dialog.valid())
	{
		SGIItemQtPaintDevice * qpaintdevItem = dynamic_cast<SGIItemQtPaintDevice *>(_item.get());
		SGIItemQtIcon * qiconItem = dynamic_cast<SGIItemQtIcon *>(_item.get());
		if (qpaintdevItem)
		{
			QImage * qimage = dynamic_cast<QImage *>(qpaintdevItem->object());
			QPixmap * qpixmap = dynamic_cast<QPixmap *>(qpaintdevItem->object());
			if(qimage)
				dialog->setObject(_item.get(), convertImage(qimage), std::string(), hostCallback());
			else if(qpixmap)
				dialog->setObject(_item.get(), convertImage(qpixmap), std::string(), hostCallback());
		}
		if (qiconItem)
		{
			QIcon * qicon = dynamic_cast<QIcon *>(qiconItem->object());
			dialog->setObject(_item.get(), convertImage(qicon), std::string(), hostCallback());
		}

		dialog->show();
	}
    return true;
}

} // namespace qt_plugin
} // namespace sgi
