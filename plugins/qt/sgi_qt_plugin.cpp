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
#include <QtWidgets/private/qwidgetwindow_p.h>
#include <QPaintDeviceWindow>
#include <QSurface>
#include <QOffscreenSurface>
#include <QPagedPaintDevice>
#include <QRasterWindow>

#ifdef __APPLE__
typedef void * GLDEBUGPROC;
#endif
#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLPaintDevice>

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
    QOpenGLContext
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(QWindow)
    QPaintDeviceWindow, QWidgetWindow
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

SGI_OBJECT_INFO_BEGIN(osg::Referenced)
    ISceneGraphDialog
SGI_OBJECT_INFO_END()

namespace sgi {

namespace qt_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)


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
                                        settingsDialogCreateImpl
                                        >
    SGIPluginImpl;

} // namespace qt_plugin

class SGIPlugin_qt_Implementation : public qt_plugin::SGIPluginImpl
{
public:
    SGIPlugin_qt_Implementation(SGIPluginHostInterface * hostInterface=NULL)
        : qt_plugin::SGIPluginImpl(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeWidgets);
        SGIITEMTYPE_NAME(SGIItemTypeMethods);
        SGIITEMTYPE_NAME(SGIItemTypeSurfaceFormat);
        SGIITEMTYPE_NAME(SGIItemTypeContext);
        SGIITEMTYPE_NAME(SGIItemTypeContextExtensions);
        SGIITEMTYPE_NAME(SGIItemTypeSurface);
        SGIITEMTYPE_NAME(SGIItemTypeProperties);
    }
    SGIPlugin_qt_Implementation(const SGIPlugin_qt_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : qt_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_qt, SGIPlugin_qt_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(qt)
