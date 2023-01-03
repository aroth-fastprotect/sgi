#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemVsg.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/SGIItemInternal>

#include "SGIItemVsg"
#ifdef SGI_USE_VSGQT
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIItemQt>
#endif

#include "ObjectTreeVsg.h"
#include "writeHTMLVsg.h"
#include "getObjectInfoVsg.h"
#include "ContextMenuVsg.h"
#include "MenuActionVsg.h"

#include <type_traits>

#include <vsg/core/Data.h>
#include <vsg/io/Options.h>
#include <vsg/io/ObjectFactory.h>
#ifdef SGI_USE_VSGQT
#include <vsg/ui/PointerEvent.h>
#include <vsgQt/ViewerWindow.h>
#endif

SGI_OBJECT_INFO_BEGIN(vsg::Object)
vsg::Data, vsg::Options, vsg::ObjectFactory
SGI_OBJECT_INFO_END()

namespace sgi {

namespace vsg_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(sgi::details::Referenced)

#ifdef SGI_USE_VSGQT
GENERATE_IMPL_NO_ACCEPT(QObject);
GENERATE_IMPL_NO_ACCEPT(QWidget);
GENERATE_IMPL_NO_ACCEPT(QWindow);
#ifdef OSGQT_USE_QOPENGLWIDGET
GENERATE_IMPL_NO_ACCEPT(QOpenGLWidget);
#else
#ifdef OSGQT_ENABLE_QGLWIDGET
GENERATE_IMPL_NO_ACCEPT(QGLWidget);
#endif
#endif

typedef generateItemImplT<generateItemAcceptImpl, SGIItemVsg, SGIItemQt, SGIItemInternal> generateItemImpl;
#else
typedef generateItemImplT<generateItemAcceptImpl, SGIItemVsg, SGIItemInternal > generateItemImpl;
#endif

typedef SGIPluginImplementationT< generateItemImpl,
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
                                        actionHandlerImpl
                                        >
    SGIPluginImpl;

} // namespace log4cplus_plugin

class SGIPlugin_vsg_Implementation : public vsg_plugin::SGIPluginImpl
{
public:
    SGIPlugin_vsg_Implementation(SGIPluginHostInterface * hostInterface=nullptr)
        : vsg_plugin::SGIPluginImpl(hostInterface)
    {
        //SGIITEMTYPE_NAME(SGIItemTypeAppenders);
    }
    SGIPlugin_vsg_Implementation(const SGIPlugin_vsg_Implementation & rhs)
        : vsg_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi_vsg, SGIPlugin_vsg_Implementation)
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(vsg)
