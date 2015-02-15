#include "stdafx.h"
#include "sgi_internal.h"

#include "sgi/Export"
#include "sgi/plugins/SGIPluginImpl.h"

#include "sgi/plugins/SGIHostItemOsg.h"
#include "sgi/plugins/SGIProxyItem.h"
#include "sgi/SGIItemInternal"
#include "SGIPlugin.h"
#include "sgi/SceneGraphDialog"
#include "sgi/InspectorHandler"
//#include "ContextMenu.h"

#include "sgi/helpers/rtti"
#include <osgGA/GUIEventHandler>

sgi::SGIPluginHostInterface * sgi::SGIPluginInterface::_hostInterface = NULL;

namespace sgi {
namespace internal_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)
GENERATE_IMPL_NO_ACCEPT(osg::Object)
GENERATE_IMPL_NO_ACCEPT(osg::NodeCallback)
GENERATE_IMPL_NO_ACCEPT(osgGA::GUIEventHandler)

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced,
                                  LOKI_TYPELIST(SGIPlugins,
                                                ISceneGraphDialog,
                                                IContextMenu,
                                                IObjectLoggerDialog,
                                                ReferencedInternalItemData,
                                                ReferencedInternalInfoData,
                                                osg::Object))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Object, LOKI_TYPELIST(SGIItemBase, osg::NodeCallback))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::NodeCallback, LOKI_TYPELIST(osgGA::GUIEventHandler))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgGA::GUIEventHandler, LOKI_TYPELIST(SceneGraphInspectorHandler))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(SGIItemBase, LOKI_TYPELIST(SGIProxyItemBase))

WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIPlugins)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalItemData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(ReferencedInternalInfoData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SGIProxyItemBase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(SceneGraphInspectorHandler)


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
                    os << "<tr><td>score</td><td>" << info.pluginScore << "</td></tr>";
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
            os << "<tr><td>pluginScore</td><td>" << pluginInfo->pluginScore << "</td></tr>" << std::endl;

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
                    os << "&nbsp;<b>" << helpers::demangle_html(item->typeName()) << "</b>&nbsp;<i>" << itemPluginInfo->pluginName << ':' << itemPluginInfo->pluginScore << "</i></li>";
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

bool writePrettyHTMLImpl<SceneGraphInspectorHandler>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    SceneGraphInspectorHandler * object = getObject<SceneGraphInspectorHandler,SGIItemInternal,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>info</td><td>" << (void*)object->info() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
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
    return true;
}

typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemInternal),
                                        call_function_for_object_type,
                                        generateItemImpl,
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
        SGIITEMTYPE_NAME(SGIItemTypeStateSetEffective);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetModeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetAttributeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetTextureModeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetTextureAttributeLists);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetUniformList);
        SGIITEMTYPE_NAME(SGIItemTypeParentalNodePath);
        SGIITEMTYPE_NAME(SGIItemTypeDrawables);
        SGIITEMTYPE_NAME(SGIItemTypeDrawable);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableColors);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableColorIndicies);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableNormals);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableNormalIndicies);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableVertices);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableVertexIndicies);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableTexCoords);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableTexCoordsList);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableTexCoordIndicies);
        SGIITEMTYPE_NAME(SGIItemTypeDrawablePrimitiveSetList);
        SGIITEMTYPE_NAME(SGIItemTypeImageLayers);
        SGIITEMTYPE_NAME(SGIItemTypeElevationLayers);
        SGIITEMTYPE_NAME(SGIItemTypeModelLayers);
        SGIITEMTYPE_NAME(SGIItemTypeMaskLayers);
        SGIITEMTYPE_NAME(SGIItemTypeImageLayer);
        SGIITEMTYPE_NAME(SGIItemTypeElevationLayer);
        SGIITEMTYPE_NAME(SGIItemTypeModelLayer);
        SGIITEMTYPE_NAME(SGIItemTypeMaskLayer);
        SGIITEMTYPE_NAME(SGIItemTypeTerrain);
        SGIITEMTYPE_NAME(SGIItemTypeMap);
        SGIITEMTYPE_NAME(SGIItemTypeMapNode);
        SGIITEMTYPE_NAME(SGIItemTypeStatistics);
        SGIITEMTYPE_NAME(SGIItemTypeCallbacks);
        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextCameras);
        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextOperations);
        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextGLExtensions);
        SGIITEMTYPE_NAME(SGIItemTypeModel);
        SGIITEMTYPE_NAME(SGIItemTypeModelRaw);
        SGIITEMTYPE_NAME(SGIItemTypeModelType);
        SGIITEMTYPE_NAME(SGIItemTypeModelObject);
        SGIITEMTYPE_NAME(SGIItemTypeModelObjectProperty);
        SGIITEMTYPE_NAME(SGIItemTypeLocationTrack);
        SGIITEMTYPE_NAME(SGIItemTypeLocationFutureTrack);
        SGIITEMTYPE_NAME(SGIItemTypeDigitalSensorDevices);
        SGIITEMTYPE_NAME(SGIItemTypeRelayDevices);
        SGIITEMTYPE_NAME(SGIItemTypeCameras);
        SGIITEMTYPE_NAME(SGIItemTypeCamerasAsync);
        SGIITEMTYPE_NAME(SGIItemTypePlacemarks);
        SGIITEMTYPE_NAME(SGIItemTypeGlobalPlacemarks);
        SGIITEMTYPE_NAME(SGIItemTypeGroupPlacemarks);
        SGIITEMTYPE_NAME(SGIItemTypeUserPlacemarks);
        SGIITEMTYPE_NAME(SGIItemTypeTemporaryPlacemarks);
        SGIITEMTYPE_NAME(SGIItemTypeBeings);
        SGIITEMTYPE_NAME(SGIItemTypeAlarms);
        SGIITEMTYPE_NAME(SGIItemTypePointOfInterests);
        SGIITEMTYPE_NAME(SGIItemTypeLayers);
        SGIITEMTYPE_NAME(SGIItemTypeVicinities);
        SGIITEMTYPE_NAME(SGIItemTypeBuildings);
        SGIITEMTYPE_NAME(SGIItemTypeFloors);
        SGIITEMTYPE_NAME(SGIItemTypeRooms);
        SGIITEMTYPE_NAME(SGIItemTypeSky);
        SGIITEMTYPE_NAME(SGIItemTypeView);
        SGIITEMTYPE_NAME(SGIItemTypeImage);
        SGIITEMTYPE_NAME(SGIItemTypeShaders);
        SGIITEMTYPE_NAME(SGIItemTypeShaderSource);
        SGIITEMTYPE_NAME(SGIItemTypeShaderCodeInjectionMap);
        SGIITEMTYPE_NAME(SGIItemTypeVirtualProgramShaderMap);
        SGIITEMTYPE_NAME(SGIItemTypeVirtualProgramFunctions);
        SGIITEMTYPE_NAME(SGIItemTypePopulated);
        SGIITEMTYPE_NAME(SGIItemTypeFixLocations);
        SGIITEMTYPE_NAME(SGIItemTypeVisibleLocations);
        SGIITEMTYPE_NAME(SGIItemTypeString);
        SGIITEMTYPE_NAME(SGIItemTypeWatcherEntries);
        SGIITEMTYPE_NAME(SGIItemTypeWatcherNodes);
        SGIITEMTYPE_NAME(SGIItemTypePickerHits);
        SGIITEMTYPE_NAME(SGIItemTypePickerHit);
        SGIITEMTYPE_NAME(SGIItemTypeOptions);
        SGIITEMTYPE_NAME(SGIItemTypeCachedNodes);
        SGIITEMTYPE_NAME(SGIItemTypeSettings);
        SGIITEMTYPE_NAME(SGIItemTypeImageLayerOptions);
        SGIITEMTYPE_NAME(SGIItemTypeElevationLayerOptions);
        SGIITEMTYPE_NAME(SGIItemTypeModelLayerOptions);
        SGIITEMTYPE_NAME(SGIItemTypeMaskLayerOptions);
        SGIITEMTYPE_NAME(SGIItemTypeDataExtents);
        SGIITEMTYPE_NAME(SGIItemTypeSlaves);
        SGIITEMTYPE_NAME(SGIItemTypeConfig);
        SGIITEMTYPE_NAME(SGIItemTypeTour);
        SGIITEMTYPE_NAME(SGIItemTypeObjectLogger);
        SGIITEMTYPE_NAME(SGIItemTypeArrayData);
        SGIITEMTYPE_NAME(SGIItemTypeEventHandlers);
        SGIITEMTYPE_NAME(SGIItemTypeDevices);
        SGIITEMTYPE_NAME(SGIItemTypeLocationTypes);
        SGIITEMTYPE_NAME(SGIItemTypeTrackingDevices);
        SGIITEMTYPE_NAME(SGIItemTypePolygonTypes);
        SGIITEMTYPE_NAME(SGIItemTypeControlPorts);
        SGIITEMTYPE_NAME(SGIItemTypeDataMaps);
        SGIITEMTYPE_NAME(SGIItemTypeWidgets);
        SGIITEMTYPE_NAME(SGIItemTypeActivePagedLODs);
        SGIITEMTYPE_NAME(SGIItemTypeThreads);
        SGIITEMTYPE_NAME(SGIItemTypeTileCache);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheLRU);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheMap);
        SGIITEMTYPE_NAME(SGIItemTypeUpdatableNodeBaseRequired);
        SGIITEMTYPE_NAME(SGIItemTypeUpdatableNodeBaseNotify);
        SGIITEMTYPE_NAME(SGIItemTypeZones);
        SGIITEMTYPE_NAME(SGIItemTypeParentCullState);
        SGIITEMTYPE_NAME(SGIItemTypeStateAttibutes);
        SGIITEMTYPE_NAME(SGIItemTypeStateSets);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseScenes);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseViews);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseCameras);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseContexts);
        SGIITEMTYPE_NAME(SGIItemTypeLeaves);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerShaderMap);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerProgramFunctions);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoStateSetStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoRenderBinStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoCameraStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoAppliedProgramSet);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoState);
        SGIITEMTYPE_NAME(SGIItemTypeSymbols);
        SGIITEMTYPE_NAME(SGIItemTypeReadersWriters);
        SGIITEMTYPE_NAME(SGIItemTypeImageProcessors);
        SGIITEMTYPE_NAME(SGIItemTypeDatabaseRevisions);
        SGIITEMTYPE_NAME(SGIItemTypeBufferDatas);
        SGIITEMTYPE_NAME(SGIItemTypePendingChild);
        SGIITEMTYPE_NAME(SGIItemTypeTasks);
        SGIITEMTYPE_NAME(SGIItemTypeMethods);
        SGIITEMTYPE_NAME(SGIItemTypeCameraFrustum);
        SGIITEMTYPE_NAME(SGIItemTypeCameraVideoProjection);
        SGIITEMTYPE_NAME(SGIItemTypeCameraPolygon);
        SGIITEMTYPE_NAME(SGIItemTypeChannels);
        SGIITEMTYPE_NAME(SGIItemTypeAnimations);
        SGIITEMTYPE_NAME(SGIItemTypeSerializers);
        SGIITEMTYPE_NAME(SGIItemTypeWrappers);
        SGIITEMTYPE_NAME(SGIItemTypeCompressors);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerFileRequests);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerHttpRequests);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerDataToCompile);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerDataToMerge);
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
