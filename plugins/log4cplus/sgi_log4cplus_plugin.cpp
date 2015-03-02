#include "stdafx.h"
#include <stdio.h>
#include <iostream>

#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemLog4cplus.h>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>

#include "SGIItemLog4cplus"
#include "ObjectTreeLog4cplus.h"
#include "writeHTMLLog4cplus.h"
#include "getObjectInfoLog4cplus.h"
#include "ContextMenuLog4cplus.h"
#include "MenuActionLog4cplus.h"
#include "sgi_log4cplus_plugin.h"

namespace sgi {

namespace log4cplus_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(Log4cplusObjectBase, LOKI_TYPELIST(Log4cplusObjectLogger, Log4cplusObjectHierarchy, Log4cplusObjectAppender, Log4cplusObjectLayout))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(Log4cplusObjectLayout, LOKI_TYPELIST(Log4cplusObjectSimpleLayout, Log4cplusObjectPatternLayout))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(ISceneGraphDialog))

typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemLog4cplus, SGIItemInternal),
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
                                        actionHandlerImpl
                                        >
    SGIPlugin_log4cplus_Implementation_base;

} // namespace log4cplus_plugin

class SGIPlugin_log4cplus_Implementation : public log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base
{
public:
    SGIPlugin_log4cplus_Implementation(SGIPluginHostInterface * hostInterface=NULL)
        : log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeAppenders);
    }
    SGIPlugin_log4cplus_Implementation(const SGIPlugin_log4cplus_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : log4cplus_plugin::SGIPlugin_log4cplus_Implementation_base(rhs, copyop)
    {
    }

    META_Object(sgi_log4cplus, SGIPlugin_log4cplus_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(log4cplus)
