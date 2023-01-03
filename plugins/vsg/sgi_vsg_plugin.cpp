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
#include "ObjectTreeVsg.h"
#include "writeHTMLVsg.h"
#include "getObjectInfoVsg.h"
#include "ContextMenuVsg.h"
#include "MenuActionVsg.h"

#include <vsg/core/Data.h>

SGI_OBJECT_INFO_BEGIN(vsg::Object)
    vsg::Data
SGI_OBJECT_INFO_END()

namespace sgi {

namespace vsg_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(sgi::details::Referenced)

typedef generateItemImplT<generateItemAcceptImpl, SGIItemVsg, SGIItemInternal > generateItemImpl;


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
