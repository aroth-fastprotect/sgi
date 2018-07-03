#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/SGIItemInternal>

#include "SGIItemLog4cplus"
#include "ObjectTreeLog4cplus.h"
#include "writeHTMLLog4cplus.h"
#include "getObjectInfoLog4cplus.h"
#include "ContextMenuLog4cplus.h"
#include "MenuActionLog4cplus.h"

using namespace sgi::log4cplus_plugin;

SGI_OBJECT_INFO_BEGIN(Log4cplusObjectBase)
    Log4cplusObjectLogger, Log4cplusObjectHierarchy, Log4cplusObjectAppender, Log4cplusObjectLayout
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(Log4cplusObjectLayout)
    Log4cplusObjectSimpleLayout, Log4cplusObjectPatternLayout
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(sgi::details::Referenced)
    ISceneGraphDialog
SGI_OBJECT_INFO_END()

namespace sgi {

namespace log4cplus_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(sgi::details::Referenced)

typedef generateItemImplT<generateItemAcceptImpl, SGIItemLog4cplus, SGIItemInternal > generateItemImpl;


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

class SGIPlugin_log4cplus_Implementation : public log4cplus_plugin::SGIPluginImpl
{
public:
    SGIPlugin_log4cplus_Implementation(SGIPluginHostInterface * hostInterface=nullptr)
        : log4cplus_plugin::SGIPluginImpl(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeAppenders);
    }
    SGIPlugin_log4cplus_Implementation(const SGIPlugin_log4cplus_Implementation & rhs)
        : log4cplus_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi_log4cplus, SGIPlugin_log4cplus_Implementation)
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(log4cplus)
