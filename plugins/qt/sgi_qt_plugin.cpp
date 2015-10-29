#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include "SGIItemQt"

#include <QtCore/QThread>
#include <QCoreApplication>
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

#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLPaintDevice>

#include <QGLWidget>
#include <QGLFramebufferObject>
#include <QGLPixelBuffer>

#include "writeHTMLQt.h"
#include "getObjectInfoQt.h"
#include "ObjectTreeQt.h"
#include "ContextMenuQt.h"
#include "MenuActionQt.h"
#include "SettingsDialogQt.h"

namespace sgi {

namespace qt_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QObject, LOKI_TYPELIST(QWindow, QWidget, QThread, QCoreApplication));
//SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QWindow, LOKI_TYPELIST(QPaintDeviceWindow, QQuickWindow));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QWindow, LOKI_TYPELIST(QPaintDeviceWindow, QWidgetWindow));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QPaintDeviceWindow, LOKI_TYPELIST(QOpenGLWindow, QRasterWindow));

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QSurface, LOKI_TYPELIST(QWindow, QOffscreenSurface));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QCoreApplication, LOKI_TYPELIST(QApplication));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QWidget, LOKI_TYPELIST(QDialog, QGLWidget));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QDialog, LOKI_TYPELIST(QFileDialog, QMessageBox, QInputDialog, QProgressDialog, QColorDialog));

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QMetaObject, ::Loki::NullType);
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QPaintDevice, LOKI_TYPELIST(QGLFramebufferObject, QGLPixelBuffer, QImage, QPicture, QPixmap,
                                                              QOpenGLPaintDevice, QPagedPaintDevice, QPaintDeviceWindow,
                                                              QWidget))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QIcon, ::Loki::NullType);

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(ISceneGraphDialog))


typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemQt, SGIItemQtMeta, SGIItemQtPaintDevice, SGIItemQtSurface, SGIItemQtIcon),
                                        call_function_for_object_type,
                                        generateItemImpl,
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
    }
    SGIPlugin_qt_Implementation(const SGIPlugin_qt_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : qt_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_qt, SGIPlugin_qt_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(qt)
