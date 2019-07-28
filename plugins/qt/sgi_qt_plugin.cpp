#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include "SGIItemQt"

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif

#include <QtCore/QThread>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QColorDialog>
#include <QPicture>
#include <QWindow>
#include <QPaintDeviceWindow>
#include <QSurface>
#include <QOffscreenSurface>
#include <QPagedPaintDevice>
#include <QRasterWindow>
#include <QSystemTrayIcon>

#ifdef __APPLE__
typedef void * GLDEBUGPROC;
#endif
#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#ifdef WITH_QTOPENGL
#include <QGLWidget>
#include <QGLFramebufferObject>
#include <QGLPixelBuffer>
#endif

#include "writeHTMLQt.h"
#include "getObjectInfoQt.h"
#include "ObjectTreeQt.h"
#include "ContextMenuQt.h"
#include "MenuActionQt.h"
#include "SettingsDialogQt.h"

SGI_OBJECT_INFO_BEGIN(QObject)
    QWindow, QWidget, QThread,
    QCoreApplication,
    QSystemTrayIcon,
    QOpenGLContext,
    QOpenGLShaderProgram,
    QOpenGLShader,
    QOpenGLVertexArrayObject
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QWindow)
    QPaintDeviceWindow
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QPaintDeviceWindow)
    QOpenGLWindow, QRasterWindow
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QSurface)
    QWindow, QOffscreenSurface
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QCoreApplication)
    QGuiApplication
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QGuiApplication)
    QApplication
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QWidget)
    QDialog,
#ifdef WITH_QTOPENGL
    QGLWidget,
#endif
    QOpenGLWidget
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QPaintDevice)
    QImage, QPicture, QPixmap,
    QOpenGLPaintDevice, QPagedPaintDevice, QPaintDeviceWindow,
#ifdef WITH_QTOPENGL
    QGLFramebufferObject, QGLPixelBuffer,
#endif
    QWidget
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QDialog)
    QFileDialog, QMessageBox, QInputDialog, QProgressDialog, QColorDialog
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(QMetaObject)
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QIcon)
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(sgi::details::Referenced)
    ISceneGraphDialog
SGI_OBJECT_INFO_END()

namespace sgi {

namespace qt_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(sgi::details::Referenced)


typedef generateItemImplT<generateItemAcceptImpl, SGIItemInternal, SGIItemQt, SGIItemQtMeta, SGIItemQtPaintDevice, SGIItemQtSurface, SGIItemQtIcon > generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectNameImpl,
                                        getObjectTypeImpl,
                                        getObjectPathImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        defaultPluginWriteObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl,
                                        defaultPluginGuiAdapterParentWidgetImpl,
                                        defaultPluginGuiAdapterSetViewImpl,
                                        defaultPluginGetOrCreateLoggerImpl,
                                        convertToImageConvertImpl
                                        >
    SGIPluginImpl;

} // namespace qt_plugin

class SGIPlugin_qt_Implementation : public qt_plugin::SGIPluginImpl
{
public:
    SGIPlugin_qt_Implementation(SGIPluginHostInterface * hostInterface=nullptr)
        : qt_plugin::SGIPluginImpl(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeWidgets);
        SGIITEMTYPE_NAME(SGIItemTypeMethods);
        SGIITEMTYPE_NAME(SGIItemTypeSurfaceFormat);
        SGIITEMTYPE_NAME(SGIItemTypeContext);
        SGIITEMTYPE_NAME(SGIItemTypeContextExtensions);
        SGIITEMTYPE_NAME(SGIItemTypeSurface);
        SGIITEMTYPE_NAME(SGIItemTypeProperties);
        SGIITEMTYPE_NAME(SGIItemTypeSystemEnvironment);
        SGIITEMTYPE_NAME(SGIItemTypeShaderProgramShaders);
        SGIITEMTYPE_NAME(SGIItemTypeShaderSourceCode);
        SGIITEMTYPE_NAME(SGIItemTypeShaderLog);
        SGIITEMTYPE_NAME(SGIItemTypePalette);
        SGIITEMTYPE_NAME(SGIItemTypeTheme);
    }
    SGIPlugin_qt_Implementation(const SGIPlugin_qt_Implementation & rhs)
        : qt_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi_qt, SGIPlugin_qt_Implementation)
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(qt)
