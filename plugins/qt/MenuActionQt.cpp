#include "stdafx.h"
#include "MenuActionQt.h"
#include "SGIItemQt"

#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ImagePreviewDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/helpers/qt>
#include <sgi/helpers/qt_widgets>

#include "SettingsDialogQt.h"
#include "ContextMenuQt.h"

#include <iostream>
#include <QMetaProperty>

namespace sgi {
namespace qt_plugin {

ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDumpObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionDumpObjectTree)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetGrab)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetSetVisibility)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetSetEnabled)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetSetAcceptDrops)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetSetAutoFillBackground)
ACTION_HANDLER_IMPL_REGISTER(MenuActionWidgetHighlight)
ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectMethodInvoke)
ACTION_HANDLER_IMPL_REGISTER(MenuActionImagePreview)

using namespace sgi::qt_helpers;

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    if(callback)
        callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
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

bool actionHandlerImpl<MenuActionWidgetGrab>::execute()
{
    QWidget * object = getObject<QWidget,SGIItemQt>();
    QPixmap * pixmap = new QPixmap(object->grab());
    SGIHostItemQtPaintDevice pixmapHostItem(pixmap);

    SGIItemBasePtr pixmapItem;
    _hostInterface->generateItem(pixmapItem, &pixmapHostItem);

    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogImagePreview, menu()->parentWidget(), hostCallback());
    ret = _hostInterface->openSettingsDialog(dialog, pixmapItem, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
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

namespace {
	const sgi::Image * convertImage(QImage * image)
	{
		if (!image)
			return NULL;
		sgi::Image::ImageFormat imageFormat;
		switch (image->format())
		{
		case QImage::Format_Mono: imageFormat = sgi::Image::ImageFormatMono; break;
		case QImage::Format_MonoLSB: imageFormat = sgi::Image::ImageFormatMonoLSB; break;
		case QImage::Format_Indexed8: imageFormat = sgi::Image::ImageFormatIndexed8; break;
		case QImage::Format_RGB32: imageFormat = sgi::Image::ImageFormatRGB32; break;
		case QImage::Format_ARGB32: imageFormat = sgi::Image::ImageFormatARGB32; break;
		case QImage::Format_ARGB32_Premultiplied: imageFormat = sgi::Image::ImageFormatARGB32_Premultiplied; break;
		case QImage::Format_RGB888: imageFormat = sgi::Image::ImageFormatRGB32; break;
		case QImage::Format_RGB16:
		case QImage::Format_ARGB8565_Premultiplied:
		case QImage::Format_RGB666:
		case QImage::Format_ARGB6666_Premultiplied:
		case QImage::Format_Invalid:
		case QImage::Format_RGB555:
		case QImage::Format_ARGB8555_Premultiplied:
		case QImage::Format_RGB444:
		case QImage::Format_ARGB4444_Premultiplied:
		case QImage::Format_RGBX8888:
		case QImage::Format_RGBA8888:
		case QImage::Format_RGBA8888_Premultiplied:
		case QImage::Format_BGR30:
		case QImage::Format_A2BGR30_Premultiplied:
		case QImage::Format_RGB30:
		case QImage::Format_A2RGB30_Premultiplied:
			imageFormat = sgi::Image::ImageFormatInvalid; 
			break;
		}
		sgi::Image::Origin origin = sgi::Image::OriginTopLeft;
		sgi::Image * ret = new sgi::Image(imageFormat, origin,
			image->bits(), image->byteCount(),
			image->width(), image->height(), 1, image->bytesPerLine(),
			image);
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

bool actionHandlerImpl<MenuActionImagePreview>::execute()
{
    IImagePreviewDialogPtr dialog = _hostInterface->showImagePreviewDialog(menu()->parentWidget(), _item.get(), NULL);
	
	if (dialog.valid())
	{
		SGIItemQtPaintDevice * qpaintdevItem = dynamic_cast<SGIItemQtPaintDevice *>(_item.get());
		SGIItemQtIcon * qiconItem = dynamic_cast<SGIItemQtIcon *>(_item.get());
		if (qpaintdevItem)
		{
			QImage * qimage = dynamic_cast<QImage *>(qpaintdevItem->object());
			QPixmap * qpixmap = dynamic_cast<QPixmap *>(qpaintdevItem->object());
			if(qimage)
				dialog->setObject(_item.get(), convertImage(qimage), std::string(), NULL);
			else if(qpixmap)
				dialog->setObject(_item.get(), convertImage(qpixmap), std::string(), NULL);
		}
		if (qiconItem)
		{
			QIcon * qicon = dynamic_cast<QIcon *>(qiconItem->object());
			dialog->setObject(_item.get(), convertImage(qicon), std::string(), NULL);
		}

		dialog->show();
	}
    return true;
}

} // namespace qt_plugin
} // namespace sgi
