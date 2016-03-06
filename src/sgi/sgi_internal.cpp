#include "stdafx.h"
#include "sgi_internal.h"

#include "sgi/Export"
#include "sgi/plugins/SGIPluginImpl.h"

#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>
#include "SGIPlugin.h"
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ObjectLoggerDialog>
#include <sgi/plugins/SettingsDialog>

#include "sgi/helpers/rtti"

sgi::SGIPluginHostInterface * sgi::SGIPluginInterface::_hostInterface = NULL;

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced,
                                  LOKI_TYPELIST(sgi::SGIPlugins,
                                                sgi::ISceneGraphDialog,
                                                sgi::IContextMenu,
                                                sgi::IObjectLoggerDialog,
                                                sgi::ISettingsDialog,
                                                sgi::ISettingsDialogInfo,
                                                sgi::ReferencedInternalItemData,
                                                sgi::ReferencedInternalInfoData,
                                                osg::Object))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Object, LOKI_TYPELIST(sgi::SGIItemBase))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(sgi::SGIItemBase, LOKI_TYPELIST(sgi::SGIProxyItemBase))

namespace sgi {
namespace internal_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)
GENERATE_IMPL_NO_ACCEPT(osg::Object)


WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIPlugins)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)


bool writePrettyHTMLImpl<SGIPlugins>::process(std::basic_ostream<char>& os)
{
    SGIPlugins * object = getObject<SGIPlugins,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'' width='100%'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>libraryName</td><td>" << sgiGetLibraryName() << "</td></tr>" << std::endl;
            os << "<tr><td>version</td><td>" << sgiGetVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>.so version</td><td>" << sgiGetSOVersion() << "</td></tr>" << std::endl;

            os << "<tr><td>plugins</td><td>";
            SGIPlugins::PluginInfoList plugins;
            if(object->getPlugins(plugins))
            {
                for(SGIPlugins::PluginInfoList::const_iterator it = plugins.begin(); it != plugins.end(); it++)
                {
                    const SGIPlugins::PluginInfo & info = *it;
                    os << "<table border=\'1\' align=\'left\' width='100%'>";
                    os << "<tr><td>name</td><td>" << info.pluginName << "</td></tr>";
                    os << "<tr><td>filename</td><td>" << info.pluginFilename << "</td></tr>";
                    os << "<tr><td>score</td><td>" << info.pluginScore() << "</td></tr>";
                    os << "<tr><td>WritePrettyHTML</td><td>" << (void*)info.writePrettyHTMLInterface << "</td></tr>";
                    os << "<tr><td>ObjectInfo</td><td>" << (void*)info.objectInfoInterface << "</td></tr>";
                    os << "<tr><td>ObjectTree</td><td>" << (void*)info.objectTreeInterface << "</td></tr>";
                    os << "<tr><td>ObjectLogger</td><td>" << (void*)info.objectLoggerInterface << "</td></tr>";
                    os << "<tr><td>ContextMenu</td><td>" << (void*)info.contextMenuInterface << "</td></tr>";
                    os << "<tr><td>SettingsDialog</td><td>" << (void*)info.settingsDialogInterface << "</td></tr>";
                    os << "<tr><td>GUIAdapter</td><td>" << (void*)info.guiAdapterInterface << "</td></tr>";
                    os << "</table>";
                }
            }
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<ReferencedInternalItemData>::process(std::basic_ostream<char>& os)
{
    ReferencedInternalItemData * object = getObject<ReferencedInternalItemData,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const SGIItemBasePtr & data = object->data().item;
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>this</td><td>" << (void*)data.get() << "</td></tr>" << std::endl;
            os << "<tr><td>type</td><td>" << helpers::getRTTITypename_html(data.get()) << "</td></tr>" << std::endl;

            os << "<tr><td>itemType</td><td>" << enumValueToString(data->type()) << "</td></tr>" << std::endl;
            os << "<tr><td>flags</td><td>0x" << std::hex << data->flags() << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>score</td><td>" << data->score() << "</td></tr>" << std::endl;
            os << "<tr><td>typeName</td><td>" << helpers::demangle_html(data->typeName()) << "</td></tr>" << std::endl;
            os << "<tr><td>number</td><td>" << data->number() << "</td></tr>" << std::endl;
            os << "<tr><td>userData</td><td>" << (void*)data->userData<osg::Referenced>() << "</td></tr>" << std::endl;
            os << "<tr><td>root</td><td>" << (void*)data->rootBase() << "</td></tr>" << std::endl;
            os << "<tr><td>prev</td><td>" << (void*)data->previousBase() << "</td></tr>" << std::endl;
            os << "<tr><td>next</td><td>" << (void*)data->nextBase() << "</td></tr>" << std::endl;

            const SGIPlugins::PluginInfo * pluginInfo = (const SGIPlugins::PluginInfo *)data->pluginInfo();
            os << "<tr><td>plugin</td><td>" << pluginInfo->pluginName << "</td></tr>" << std::endl;
            os << "<tr><td>pluginScore</td><td>" << pluginInfo->pluginScore() << "</td></tr>" << std::endl;

            os << "<tr><td>item chain</td><td><ul>";
            SGIItemType initialType = data->type();
            SGIItemBase * item;
            {
                item = data->rootBase();
                while(item)
                {
                    const SGIPlugins::PluginInfo * itemPluginInfo = (const SGIPlugins::PluginInfo *)item->pluginInfo();
                    os << "<li>#" << item->score() << "&nbsp;";
                    if(initialType != item->type())
                        os << enumValueToString(item->type()) << "&nbsp;";
                    if(item == data.get())
                        os << "(this)";
                    else
                        os << (void*)item;
                    os << "&nbsp;<b>" << helpers::demangle_html(item->typeName()) << "</b>&nbsp;<i>" << itemPluginInfo->pluginName << ':' << itemPluginInfo->pluginScore() << "</i></li>";
                    item = item->nextBase();
                }
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<ReferencedInternalInfoData>::process(std::basic_ostream<char>& os)
{
    ReferencedInternalInfoData * object = getObject<ReferencedInternalInfoData,SGIItemInternal>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            InternalInfoData::CommandId command = object->data().command;
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>command</td><td>" << command << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        break;
    }
    return true;
}

bool writePrettyHTMLImpl<SGIProxyItemBase>::process(std::basic_ostream<char>& os)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret;
    SGIItemBase * realObject = object->realItem(true);
    if(realObject)
        ret = _hostInterface->writePrettyHTML(os, realObject);
    else
    {
        switch(itemType())
        {
        case SGIItemTypeObject:
            {
                if(_table)
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                ret = callNextHandler(os);

                os << "<tr><td>proxyName</td><td>" << object->name() << "</td></tr>" << std::endl;
                os << "<tr><td>proxyDisplayName</td><td>" << object->displayName() << "</td></tr>" << std::endl;
                os << "<tr><td>proxyTypeName</td><td>" << object->typeName() << "</td></tr>" << std::endl;

                if(_table)
                    os << "</table>" << std::endl;
            }
            break;
        default:
            ret = callNextHandler(os);
            break;
        }
    }
    return ret;
}

GET_OBJECT_NAME_IMPL_TEMPLATE()
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_NAME_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)

std::string getObjectNameImpl<ReferencedInternalItemData>::process()
{
    return "ReferencedInternalItemData";
}

std::string getObjectNameImpl<ReferencedInternalInfoData>::process()
{
    return "ReferencedInternalInfoData";
}

std::string getObjectNameImpl<SGIPlugins>::process()
{
    return "SGIPlugins";
}

std::string getObjectNameImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object->realItem(false);
    std::string ret;
    if(realObject)
    {
        if(!_hostInterface->getObjectName(ret, realObject))
            ret = object->name();
    }
    else
        ret = object->name();
    return ret;
}

GET_OBJECT_TYPE_IMPL_TEMPLATE()
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_TYPE_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)

std::string getObjectTypeImpl<ReferencedInternalItemData>::process()
{
    return "sgi::ReferencedInternalItemData";
}

std::string getObjectTypeImpl<ReferencedInternalInfoData>::process()
{
    return "sgi::ReferencedInternalInfoData";
}

std::string getObjectTypeImpl<SGIPlugins>::process()
{
    return "sgi::SGIPlugins";
}

std::string getObjectTypeImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object->realItem(false);
    std::string ret;
    if(realObject)
    {
        if(!_hostInterface->getObjectTypename(ret, realObject))
            ret = object->typeName();
    }
    else
        ret = object->typeName();
    return ret;
}

GET_OBJECT_DISPLAYNAME_IMPL_TEMPLATE()
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(sgi::SGIPlugins)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
GET_OBJECT_DISPLAYNAME_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)

std::string getObjectDisplayNameImpl<ReferencedInternalItemData>::process()
{
    return "ReferencedInternalItemData";
}

std::string getObjectDisplayNameImpl<ReferencedInternalInfoData>::process()
{
    return "ReferencedInternalInfoData";
}

std::string getObjectDisplayNameImpl<SGIPlugins>::process()
{
    return "SGIPlugins";
}

std::string getObjectDisplayNameImpl<SGIProxyItemBase>::process()
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    SGIItemBase * realObject = object->realItem(false);
    std::string ret;
    if(realObject)
    {
        if(!_hostInterface->getObjectDisplayName(ret, realObject))
            ret = object->displayName();
    }
    else
        ret = object->displayName();
    return ret;
}

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

bool objectTreeBuildImpl<ReferencedInternalItemData>::build(IObjectTreeItem * treeItem)
{
    ReferencedInternalItemData * object = getObject<ReferencedInternalItemData,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const SGIItemBasePtr & data = object->data().item;
            if(data->nextBase())
            {
                SGIHostItemOsg next(new ReferencedInternalItemData(data->nextBase()));
                if(next.hasObject())
                    treeItem->addChild("Next", &next);
            }
            if(data->previousBase())
            {
                SGIHostItemOsg previous(new ReferencedInternalItemData(data->previousBase()));
                if(previous.hasObject())
                    treeItem->addChild("Previous", &previous);
            }
            SGIItemBasePtr rootBase = data->rootBase();
            if(rootBase.valid() && rootBase.get() != data.get())
            {
                SGIHostItemOsg root(new ReferencedInternalItemData(rootBase.get()));
                if(root.hasObject())
                    treeItem->addChild("Root", &root);
            }
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<ReferencedInternalInfoData>::build(IObjectTreeItem * treeItem)
{
    ReferencedInternalInfoData * object = getObject<ReferencedInternalInfoData,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<SGIProxyItemBase>::build(IObjectTreeItem * treeItem)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret;
    SGIItemBase * realObject = object->realItem(true);
    if(realObject)
        ret = _hostInterface->objectTreeBuildTree(treeItem, realObject);
    else
        ret = callNextHandler(treeItem);
    return ret;
}

bool objectTreeBuildImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = getObject<ISceneGraphDialog,SGIItemInternal>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            callNextHandler(treeItem);

            SGIHostItemQt dialog(object->getDialog());
            if(dialog.hasObject())
                treeItem->addChild("Dialog", &dialog);
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)

bool contextMenuPopulateImpl<SGIProxyItemBase>::populate(IContextMenuItem * menuItem)
{
    SGIProxyItemBase * object = getObject<SGIProxyItemBase,SGIItemInternal>();
    bool ret;
    SGIItemBase * realObject = object->realItem(true);
    if(realObject)
        ret = _hostInterface->contextMenuPopulate(menuItem, realObject);
    else
        ret = callNextHandler(menuItem);
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

    // no need to use a proxy node here, because the plugin instance is already created anyway
    SGIHostItemInternal hostItem(SGIPlugins::instance());
    treeItem->addChild(std::string(), &hostItem);

    treeItem->addChild(std::string(), _item);
    return true;
}

typedef generateItemImplT<generateItemAcceptImpl, SGIItemInternal> generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectNameImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectPathImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectInfoStringImpl,
                                        defaultPluginGetObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        defaultPluginWriteObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        defaultPluginActionHandlerImpl,
                                        defaultPluginSettingsDialogCreateImpl,
                                        defaultPluginGuiAdapterSetViewImpl
                                        >
    SGIPluginImpl;

} // namespace internal_plugin

class SGIPlugin_internal::Plugin : public internal_plugin::SGIPluginImpl
{
public:
    static Plugin * s_instance;
    Plugin(SGIPluginHostInterface * hostInterface=NULL)
        : internal_plugin::SGIPluginImpl(hostInterface)
    {
        s_instance = this;
        SGIITEMTYPE_NAME(SGIItemTypeInvalid);
        SGIITEMTYPE_NAME(SGIItemTypeDummy);
        SGIITEMTYPE_NAME(SGIItemTypeTreeRoot);
        SGIITEMTYPE_NAME(SGIItemTypeObject);
        SGIITEMTYPE_NAME(SGIItemTypeChilds);
        SGIITEMTYPE_NAME(SGIItemTypeParents);
        SGIITEMTYPE_NAME(SGIItemTypeObservers);
        SGIITEMTYPE_NAME(SGIItemTypeStatistics);
        SGIITEMTYPE_NAME(SGIItemTypeCallbacks);
        SGIITEMTYPE_NAME(SGIItemTypeSettings);
        SGIITEMTYPE_NAME(SGIItemTypeObjectLogger);
    }
    Plugin(const Plugin & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : internal_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi, Plugin);
};

SGIPlugin_internal::Plugin * SGIPlugin_internal::Plugin::s_instance = NULL;
const char * SGIPlugin_internal::PluginName = "_sgi_internal";

SGIPluginInterface * SGIPlugin_internal::create(SGIPluginHostInterface * hostInterface)
{
    return new Plugin(hostInterface);
}

} // namespace sgi
