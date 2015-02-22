#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/SGIItemInternal>
#include <sgi/SceneGraphDialog>
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

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QObject, LOKI_TYPELIST(QWidget, QThread, QCoreApplication));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QCoreApplication, LOKI_TYPELIST(QApplication));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QWidget, LOKI_TYPELIST(QDialog, QGLWidget));
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QDialog, LOKI_TYPELIST(QFileDialog, QMessageBox, QInputDialog, QProgressDialog, QColorDialog));

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QMetaObject, ::Loki::NullType);
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(QPaintDevice, LOKI_TYPELIST(QGLFramebufferObject, QGLPixelBuffer, QImage, QPicture, QPixmap, QWidget))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(ISceneGraphDialog))


typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemQt, SGIItemQtMeta, SGIItemQtPaintDevice),
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
    }
    SGIPlugin_qt_Implementation(const SGIPlugin_qt_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : qt_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_qt, SGIPlugin_qt_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(qt)
