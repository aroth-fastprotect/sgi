#include "stdafx.h"
#include "ObjectTreeOSGEarth.h"
#include "string_helpers.h"
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include "../osg/SGIItemOsg"
#include "SGIItemOsgEarth"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/helpers/string>
#include <sgi/helpers/osg>

#include <osgEarth/Version>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/Registry>
#include <osgEarth/LevelDBFactory>

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#include <osgEarthUtil/Controls>
#include <osgEarth/Capabilities>
#include <osgEarth/TaskService>
#include <osgEarth/StateSetCache>
#include <osgEarth/OverlayDecorator>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/model_feature_stencil/FeatureStencilModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>

#include "osgearth_accessor.h"
#include "ElevationQueryReferenced"
#include "geo_helpers.h"

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::MapNode)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Map)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Registry)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::StateSetCache)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::SkyNode)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::Control)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::Container)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::ControlCanvas)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::ControlNodeBin)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::ControlNode)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::LabelControl)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Util::Controls::ImageControl)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Layer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::TerrainLayer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ImageLayer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ElevationLayer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ModelLayer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::MaskLayer)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Terrain)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::TerrainEngineNode)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::TileSource)
OBJECT_TREE_BUILD_IMPL_REGISTER(TileSourceInfo)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::TileBlacklist)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ModelSource)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::MaskSource)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::LevelDBDatabase)
OBJECT_TREE_BUILD_IMPL_REGISTER(ElevationQueryReferenced)
OBJECT_TREE_BUILD_IMPL_REGISTER(TileKeyReferenced)
OBJECT_TREE_BUILD_IMPL_REGISTER(TileSourceTileKey)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::VirtualProgram)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Cache)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::CacheBin)

OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Features::FeatureProfile)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Features::FeatureSource)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Features::FeatureModelSource)

OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Config)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ConfigOptions)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::ModelLayerOptions)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Features::FeatureModelSourceOptions)
OBJECT_TREE_BUILD_IMPL_REGISTER(osgEarth::Drivers::FeatureGeomModelOptions)

using namespace osg_helpers;

bool objectTreeBuildImpl<osgEarth::Map>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    osgEarth::MapNode * mapNode = _item->userData<osgEarth::MapNode>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            osgEarth::MapCallbackList callbacks;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            object->getMapCallbacks(callbacks);
#endif
            if(!callbacks.empty())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            SGIHostItemOsg profile(object->getProfile());
            if(profile.hasObject())
                treeItem->addChild("Profile", &profile);

            SGIHostItemOsg profileNoVDatum(object->getProfileNoVDatum());
            if(profileNoVDatum.hasObject())
                treeItem->addChild("ProfileNoVDatum", &profileNoVDatum);

            SGIHostItemOsg cache(object->getCache());
            if(cache.hasObject())
                treeItem->addChild("Cache", &cache);

            SGIHostItemOsg globalOpts(object->getGlobalOptions());
            if(globalOpts.hasObject())
                treeItem->addChild("GlobalOptions", &globalOpts);

            SGIHostItemOsg dbOpts(object->getDBOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("DBOptions", &dbOpts);

            SGIHostItemOsgEarthConfigOptions initialOptions(object->getInitialMapOptions());
            treeItem->addChild("Initial Options", &initialOptions);

            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getMapOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);

            unsigned numImage = object->getNumImageLayers();
            if(numImage)
                treeItem->addChild(helpers::str_plus_count("Image layers", numImage), cloneItem<SGIItemOsg>(SGIItemTypeImageLayers));
            unsigned numElev = object->getNumElevationLayers();
            if(numElev)
                treeItem->addChild(helpers::str_plus_count("Elevation layers", numElev), cloneItem<SGIItemOsg>(SGIItemTypeElevationLayers));
            unsigned numModel = object->getNumModelLayers();
            if(numModel)
                treeItem->addChild(helpers::str_plus_count("Model layers", numModel), cloneItem<SGIItemOsg>(SGIItemTypeModelLayers));
            unsigned numMask = 0;
            {
                osgEarth::MaskLayerVector maskLayers;
                object->getTerrainMaskLayers(maskLayers);
                numMask = maskLayers.size();
            }
            if(numMask)
                treeItem->addChild(helpers::str_plus_count("Mask layers", numMask), cloneItem<SGIItemOsg>(SGIItemTypeMaskLayers));
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            osgEarth::MapCallbackList callbacks;
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
            object->getMapCallbacks(callbacks);
#endif
            for(osgEarth::MapCallbackList::const_iterator it = callbacks.begin(); it != callbacks.end(); it++)
            {
                SGIHostItemOsg callback(*it);
                if(callback.hasObject())
                    treeItem->addChild(std::string(), &callback);
            }
        }
        break;
    case SGIItemTypeElevationLayers:
        {
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ELEVATION_LAYERS);
            const osgEarth::ElevationLayerVector & elevationLayers = mapframe.elevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = elevationLayers.begin(); it != elevationLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ElevationLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeImageLayers:
        {
            osgEarth::MapFrame mapframe(object, osgEarth::Map::IMAGE_LAYERS);
            const osgEarth::ImageLayerVector & imageLayers = mapframe.imageLayers();
            for(osgEarth::ImageLayerVector::const_iterator it = imageLayers.begin(); it != imageLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ImageLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeModelLayers:
        {
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MODEL_LAYERS);
            const osgEarth::ModelLayerVector & modelLayers = mapframe.modelLayers();
            for(osgEarth::ModelLayerVector::const_iterator it = modelLayers.begin(); it != modelLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ModelLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    case SGIItemTypeMaskLayers:
        {
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MASK_LAYERS);
            const osgEarth::MaskLayerVector & maskLayers = mapframe.terrainMaskLayers();
            for(osgEarth::MaskLayerVector::const_iterator it = maskLayers.begin(); it != maskLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::MaskLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::MapNode>::build(IObjectTreeItem * treeItem)
{
    osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg map(object->getMap(), object);
            if(map.hasObject())
                treeItem->addChild("Map", &map);

            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getMapNodeOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);

			MapNodeAccess * access = (MapNodeAccess*)object;
			const auto & extensions = object->getExtensions();
			if(!extensions.empty())
				treeItem->addChild("Extensions", cloneItem<SGIItemOsg>(SGIItemTypeExtensions));

			SGIHostItemOsg terrain(access->terrain());
            if(terrain.hasObject())
                treeItem->addChild("Terrain", &terrain);
            SGIHostItemOsg terrainEngine(access->terrainEngineNode());
            if(terrainEngine.hasObject())
                treeItem->addChild("TerrainEngine", &terrainEngine);
			SGIHostItemOsg terrainEngineContainer(access->terrainEngineContainer());
			if (terrainEngineContainer.hasObject())
				treeItem->addChild("TerrainEngineContainer", &terrainEngineContainer);
            SGIHostItemOsg modelLayerGroup(object->getModelLayerGroup());
            if(modelLayerGroup.hasObject())
                treeItem->addChild("ModelLayerGroup", &modelLayerGroup);
            SGIHostItemOsg overlayDecorator(object->getOverlayDecorator());
            if(overlayDecorator.hasObject())
                treeItem->addChild("OverlayDecorator", &overlayDecorator);
        }
        break;
	case SGIItemTypeExtensions:
		{
			const auto & extensions = object->getExtensions();
			for (const auto & ext : extensions)
			{
				SGIHostItemOsg item(ext);
				treeItem->addChild(std::string(), &item);
			}
		}
		break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Registry>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Registry * object = static_cast<osgEarth::Registry*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getURIReadCallback() )
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

            SGIHostItemOsg cache(object->getCache());
            if(cache.hasObject())
                treeItem->addChild("Cache", &cache);

			treeItem->addChild("Blacklist", cloneItem<SGIItemOsg>(SGIItemTypeBlacklist));

            SGIHostItemOsg globalGeodeticProfile(object->getGlobalGeodeticProfile());
            if(globalGeodeticProfile.hasObject())
                treeItem->addChild("Global geodetic profile", &globalGeodeticProfile);

            SGIHostItemOsg globalMercatorProfile(object->getGlobalMercatorProfile());
            if(globalMercatorProfile.hasObject())
                treeItem->addChild("Global mercator profile", &globalMercatorProfile);

            SGIHostItemOsg sphericalMercatorProfile(object->getSphericalMercatorProfile());
            if(sphericalMercatorProfile.hasObject())
                treeItem->addChild("Spherical mercator profile", &sphericalMercatorProfile);

            SGIHostItemOsg cubeProfile(object->getCubeProfile());
            if(cubeProfile.hasObject())
                treeItem->addChild("Cube profile", &cubeProfile);

            SGIHostItemOsg defaultFont(object->getDefaultFont());
            if(defaultFont.hasObject())
                treeItem->addChild("Default Font", &defaultFont);

//             SGIHostItemOsg shaderFactory(object->getShaderFactory());
//             if(shaderFactory.hasObject())
//                 treeItem->addChild("ShaderFactory", &shaderFactory);

            SGIHostItemOsg stateSetCache(object->getStateSetCache());
            if(stateSetCache.hasObject())
                treeItem->addChild("StateSetCache", &stateSetCache);

            SGIHostItemOsg taskServiceManager(object->getTaskServiceManager());
            if(taskServiceManager.hasObject())
                treeItem->addChild("TaskServiceManager", &taskServiceManager);

            SGIHostItemOsg defaultOptions(object->getDefaultOptions());
            if(defaultOptions.hasObject())
                treeItem->addChild("Default options", &defaultOptions);

            SGIHostItemOsg capabilities(&object->getCapabilities());
            if(capabilities.hasObject())
                treeItem->addChild("Capabilities", &capabilities);

			{
                osgEarth::LevelDBDatabasePairList databases;
                osgEarth::LevelDBFactory::getDatabases(databases, false);
                if (!databases.empty())
					treeItem->addChild(helpers::str_plus_count("LevelDB", databases.size()), cloneItem<SGIItemOsg>(SGIItemTypeDatabases));
			}
		}
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            SGIHostItemOsg uriReadFileCallback(object->getURIReadCallback());
            if(uriReadFileCallback.hasObject())
                treeItem->addChild("URIReadCallback", &uriReadFileCallback);

            ret = true;
        }
        break;
	case SGIItemTypeDatabases:
		{
            osgEarth::LevelDBDatabasePairList databases;
            osgEarth::LevelDBFactory::getDatabases(databases, false);
            for (const osgEarth::LevelDBDatabasePair & pair : databases)
            {
                SGIHostItemOsg item(pair.second.get());
                if (item.hasObject())
                    treeItem->addChild(pair.first, &item);
                else
                    treeItem->addChild(pair.first, (sgi::SGIItemBase*)NULL);
            }
			ret = true;
		}
		break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

namespace {
    class StateSetCacheTree : public osgEarth::StateSetCache
    {
    public:
        typedef StateSetSet SSSet;
        typedef StateAttributeSet SASet;
        void getStateAttributeCache(SASet & set) const
        {
            osgEarth::Threading::ScopedMutexLock lock( _mutex );
            set = _stateAttributeCache;
        }
        void getStateSetCache(SSSet & set) const
        {
            osgEarth::Threading::ScopedMutexLock lock( _mutex );
            set = _stateSetCache;
        }
    };
}

bool objectTreeBuildImpl<osgEarth::StateSetCache>::build(IObjectTreeItem * treeItem)
{
    osgEarth::StateSetCache * object = static_cast<osgEarth::StateSetCache*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const StateSetCacheTree * access = (const StateSetCacheTree*)object;
            {
                StateSetCacheTree::SASet saset;
                access->getStateAttributeCache(saset);
                treeItem->addChild(helpers::str_plus_count("StateAttributes", saset.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateAttibutes));
            }
            {
                StateSetCacheTree::SSSet ssset;
                access->getStateSetCache(ssset);
                treeItem->addChild(helpers::str_plus_count("StateSets", ssset.size()), cloneItem<SGIItemOsg>(SGIItemTypeStateSets));
            }
        }
        break;
    case SGIItemTypeStateAttibutes:
        {
            const StateSetCacheTree * access = (const StateSetCacheTree*)object;
            StateSetCacheTree::SASet saset;
            access->getStateAttributeCache(saset);
            
            for(StateSetCacheTree::SASet::const_iterator it = saset.begin(); it != saset.end(); it++)
            {
                SGIHostItemOsg item(*it);
                treeItem->addChild(std::string(), &item);
            }
            ret = true;
        }
        break;
    case SGIItemTypeStateSets:
        {
            const StateSetCacheTree * access = (const StateSetCacheTree*)object;
            StateSetCacheTree::SSSet ssset;
            access->getStateSetCache(ssset);
            
            for(StateSetCacheTree::SSSet::const_iterator it = ssset.begin(); it != ssset.end(); it++)
            {
                SGIHostItemOsg item(*it);
                treeItem->addChild(std::string(), &item);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::SkyNode>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::SkyNode * object = static_cast<osgEarth::Util::SkyNode*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            osg::Camera * camera = findFirstParentOfType<osg::Camera>(object);
            osg::View * view = (camera)?camera->getView():NULL;

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            SGIHostItemOsg ephemeris(object->getEphemeris());
            if(ephemeris.hasObject())
                treeItem->addChild("Ephemeris", &ephemeris);
            SGIHostItemOsg light(object->getSunLight());
            if(light.hasObject())
                treeItem->addChild("Light", &light);
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::Control>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::Control * object = static_cast<osgEarth::Util::Controls::Control*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {


            ControlAccess * access = (ControlAccess *)object;

            SGIHostItemOsg geode(access->geode());
            if(geode.hasObject())
                treeItem->addChild("Geode", &geode);

            const osgEarth::Util::Controls::ControlEventHandlerList & eventHandlers = access->eventHandlers();
            if(!eventHandlers.empty())
                treeItem->addChild(helpers::str_plus_count("Event Handlers", eventHandlers.size()), cloneItem<SGIItemOsg>(SGIItemTypeEventHandlers));
        }
        break;
    case SGIItemTypeEventHandlers:
        {
            ControlAccess * access = (ControlAccess *)object;
            const osgEarth::Util::Controls::ControlEventHandlerList & eventHandlers = access->eventHandlers();
            for(osgEarth::Util::Controls::ControlEventHandlerList::const_iterator it = eventHandlers.begin(); it != eventHandlers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::Util::Controls::ControlEventHandler> evthandler = *it;
                SGIHostItemOsg handler(evthandler.get());
                treeItem->addChild(std::string(), &handler);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::Container>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::Container * object = static_cast<osgEarth::Util::Controls::Container*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {

        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
bool objectTreeBuildImpl<osgEarth::Util::Controls::ControlCanvas>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::ControlCanvas * object = static_cast<osgEarth::Util::Controls::ControlCanvas*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            ControlCanvasAccess* access = (ControlCanvasAccess*)object;
            const osgEarth::Util::Controls::ControlContext & controlContext = access->controlContext();

            SGIHostItemOsg view(controlContext._view);
            if(view.hasObject())
                treeItem->addChild("View", &view);

            SGIHostItemOsg viewport(controlContext._vp);
            if(viewport.hasObject())
                treeItem->addChild("Viewport", &viewport);

            SGIHostItemOsg controlNodeBin(access->controlNodeBin());
            if(controlNodeBin.hasObject())
                treeItem->addChild("ControlNodeBin", &controlNodeBin);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::ControlNodeBin>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::ControlNodeBin * object = getObject<osgEarth::Util::Controls::ControlNodeBin, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::ControlNode>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::ControlNode * object = getObject<osgEarth::Util::Controls::ControlNode, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::LabelControl>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::LabelControl * object = getObject<osgEarth::Util::Controls::LabelControl, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg font(object->font());
            if(font.hasObject())
                treeItem->addChild("Font", &font);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            SGIHostItemOsg drawable(object->drawable());
            if(drawable.hasObject())
                treeItem->addChild("Drawable", &drawable);
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Util::Controls::ImageControl>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Util::Controls::ImageControl * object = getObject<osgEarth::Util::Controls::ImageControl, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg image(object->getImage());
            if(image.hasObject())
                treeItem->addChild("Image", &image);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Layer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Layer * object = static_cast<osgEarth::Layer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {


        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::TerrainLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            TerrainLayerAccessor * access = (TerrainLayerAccessor*)object;
            
            const osgEarth::Profile * profile = access->profileNoInit();
            const osgEarth::Profile * targetProfileHint = access->targetProfileHintNoInit();

            SGIHostItemOsg profileItem(profile);
            if(profile)
                treeItem->addChild("Profile", &profileItem);
            SGIHostItemOsg targetProfileItem(targetProfileHint);
            if(targetProfileHint && targetProfileHint != profile)
                treeItem->addChild("Target profile", &targetProfileItem);

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            SGIHostItemOsg tileSource(access->tileSourceNoInit());
#else
			SGIHostItemOsg tileSource((osg::Referenced*)NULL);
			if (access->tileSourceInitAttempted())
				tileSource = object->getTileSource();
#endif
            if(tileSource.hasObject())
                treeItem->addChild("Tile source", &tileSource);
            SGIHostItemOsg dbOptions(access->dbOptions());
            if(dbOptions.hasObject())
                treeItem->addChild("Database options", &dbOptions);

            SGIHostItemOsg cache(object->getCache());
            if(cache.hasObject())
                treeItem->addChild("Cache", &cache);

            if(profile)
            {
                SGIHostItemOsg cacheBinNative(object->getCacheBin(profile));
                if(cacheBinNative.hasObject())
                    treeItem->addChild("CacheBin (native)", &cacheBinNative);
            }
            if(targetProfileHint)
            {
                SGIHostItemOsg cacheBinTarget(object->getCacheBin(targetProfileHint));
                if(cacheBinTarget.hasObject())
                    treeItem->addChild("CacheBin (target)", &cacheBinTarget);
            }

            SGIHostItemOsgEarthConfigOptions initialOptions(object->getInitialOptions());
            treeItem->addChild("Initial Options", &initialOptions);

            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getTerrainLayerRuntimeOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ImageLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ElevationLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ElevationLayer * object = static_cast<osgEarth::ElevationLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ModelLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    osgEarth::MapNode * mapNode = _item->userData<osgEarth::MapNode>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg modelSource(object->getModelSource());
            if(modelSource.hasObject())
                treeItem->addChild("Model source", &modelSource);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            SGIHostItemOsg maskSource(object->getMaskSource());
            if(maskSource.hasObject())
                treeItem->addChild("Mask source", &maskSource);
#endif

            SGIHostItemOsg modelNode(mapNode?mapNode->getModelLayerNode(object):NULL);
            if(modelNode.hasObject())
                treeItem->addChild("Model node", &modelNode);

            SGIHostItemOsgEarthConfigOptions initialOptions(object->getModelLayerOptions());
            treeItem->addChild("Initial Options", &initialOptions);

            // there are runtime options, but they are not accessible
#if 0
            SGIHostItemOsgEarthConfigOptions runtimeOptions(&object->getTerrainLayerRuntimeOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::MaskLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::MaskLayer * object = static_cast<osgEarth::MaskLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg maskSource(object->getMaskSource());
            if(maskSource.hasObject())
                treeItem->addChild("Mask source", &maskSource);

            // there are initial and runtime options, but they are not accessible
#if 0
            SGIHostItemOsgEarthConfigOptions initialOptions(&object->getModelLayerOptions());
            treeItem->addChild("Initial Options", &initialOptions);

            SGIHostItemOsgEarthConfigOptions runtimeOptions(&object->getTerrainLayerRuntimeOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Terrain>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Terrain * object = static_cast<osgEarth::Terrain*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {


            SGIHostItemOsg graph(object->getGraph());
            if(graph.hasObject())
                treeItem->addChild("Graph", &graph);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::TerrainEngineNode>::build(IObjectTreeItem * treeItem)
{
	osgEarth::TerrainEngineNode * object = static_cast<osgEarth::TerrainEngineNode*>(item<SGIItemOsg>()->object());
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			SGIHostItemOsg map(object->getMap());
			if (map.hasObject())
				treeItem->addChild("Map", &map);

			SGIHostItemOsg terrain(object->getTerrain());
			if (terrain.hasObject())
				treeItem->addChild("Terrain", &terrain);

			SGIHostItemOsg resources(object->getResources());
			if (resources.hasObject())
				treeItem->addChild("Resources", &resources);

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
			SGIHostItemOsg terrainstateset(object->getTerrainStateSet());
			if (terrainstateset.hasObject())
				treeItem->addChild("TerrainStateSet", &terrainstateset);

			SGIHostItemOsg payloadstateset(object->getPayloadStateSet());
			if (payloadstateset.hasObject())
				treeItem->addChild("PayloadStateSet", &payloadstateset);
#endif
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgEarth::TileSource>::build(IObjectTreeItem * treeItem)
{
    osgEarth::TileSource * object = dynamic_cast<osgEarth::TileSource*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg profile(object->getProfile());
            if(profile.hasObject())
                treeItem->addChild("Profile", &profile);
            SGIHostItemOsg blacklist(object->getBlacklist());
            if(blacklist.hasObject())
                treeItem->addChild("Blacklist", &blacklist);

            const osgEarth::TileSourceOptions & tileSourceOpts = object->getOptions();
            SGIHostItemOsgEarthConfigOptions runtimeOptions(tileSourceOpts);
            treeItem->addChild("Runtime Options", &runtimeOptions);

            const osgEarth::DataExtentList& dataExtents = object->getDataExtents();
            if(!dataExtents.empty())
                treeItem->addChild(helpers::str_plus_count("Data extents", dataExtents.size()), cloneItem<SGIItemOsg>(SGIItemTypeDataExtents, ~0u));

            osg::ref_ptr<TileSourceInfo> tsi = new TileSourceInfo(object);
            SGIHostItemOsg tsiitem(tsi.get());
            treeItem->addChild("Info", &tsiitem);
        }
        break;
    case SGIItemTypeDataExtents:
        {
            if(_item->number() == ~0u)
            {
                const osgEarth::DataExtentList& dataExtents = object->getDataExtents();
                unsigned num = 0;
                for(osgEarth::DataExtentList::const_iterator it = dataExtents.begin(); it != dataExtents.end(); ++it, ++num)
                {
                    const osgEarth::DataExtent & extent = *it;
                    treeItem->addChild(extent.toString(), cloneItem<SGIItemOsg>(SGIItemTypeDataExtents, num));
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<TileSourceInfo>::build(IObjectTreeItem * treeItem)
{
    TileSourceInfo * object = getObject<TileSourceInfo, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild(helpers::str_plus_count("Changesets", object->changesets().size()), cloneItem<SGIItemOsg>(SGIItemTypeChangeset, ~0u));
            treeItem->addChild("Info", cloneItem<SGIItemOsg>(SGIItemTypeInfo, ~0u));
        }
        break;
    case SGIItemTypeChangeset:
        {
            if(_item->number() == ~0u)
            {
                const osgEarth::ConfigSet & changesets = object->changesets();
                unsigned num = 0;
                for(osgEarth::ConfigSet::const_iterator it = changesets.begin(); it != changesets.end(); ++it, ++num)
                {
                    const osgEarth::Config & cfg = *it;
                    std::string name = (cfg.key().empty())?helpers::str_plus_count("Changeset", num):cfg.key();
                    treeItem->addChild(name, cloneItem<SGIItemOsg>(SGIItemTypeChangeset, num));
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeInfo:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::TileBlacklist>::build(IObjectTreeItem * treeItem)
{
    osgEarth::TileBlacklist * object = dynamic_cast<osgEarth::TileBlacklist*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {


        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ModelSource>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ModelSource * object = getObject<osgEarth::ModelSource,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            const osgEarth::DataExtentList& dataExtents = object->getDataExtents();
            if(!dataExtents.empty())
                treeItem->addChild(helpers::str_plus_count("Data extents", dataExtents.size()), cloneItem<SGIItemOsg>(SGIItemTypeDataExtents, ~0u));

            const osgEarth::NodeOperationVector& preMergeOperations = object->preMergeOperations();
            if (!preMergeOperations.empty())
                treeItem->addChild(helpers::str_plus_count("Pre-Merge operations", preMergeOperations.size()), cloneItem<SGIItemOsg>(SGIItemTypePreMergeOps, ~0u));

            const osgEarth::NodeOperationVector& postMergeOperations = object->postMergeOperations();
            if (!postMergeOperations.empty())
                treeItem->addChild(helpers::str_plus_count("Post-Merge operations", postMergeOperations.size()), cloneItem<SGIItemOsg>(SGIItemTypePostMergeOps, ~0u));
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)

        }
        break;
    case SGIItemTypePreMergeOps:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            const osgEarth::NodeOperationVector& preMergeOperations = object->preMergeOperations();
            for (osgEarth::NodeOperationVector::const_iterator it = preMergeOperations.begin(); it != preMergeOperations.end(); it++)
            {
                const osg::ref_ptr<osgEarth::NodeOperation> & nodeop = *it;
                SGIHostItemOsg item(nodeop.get());
                treeItem->addChild(std::string(), &item);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypePostMergeOps:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            const osgEarth::NodeOperationVector& postMergeOperations = object->postMergeOperations();
            for (osgEarth::NodeOperationVector::const_iterator it = postMergeOperations.begin(); it != postMergeOperations.end(); it++)
            {
                const osg::ref_ptr<osgEarth::NodeOperation> & nodeop = *it;
                SGIHostItemOsg item(nodeop.get());
                treeItem->addChild(std::string(), &item);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeDataExtents:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            if(_item->number() == ~0u)
            {
                const osgEarth::DataExtentList& dataExtents = object->getDataExtents();
                unsigned num = 0;
                for(osgEarth::DataExtentList::const_iterator it = dataExtents.begin(); it != dataExtents.end(); ++it, ++num)
                {
                    const osgEarth::DataExtent & extent = *it;
                    treeItem->addChild(extent.toString(), cloneItem<SGIItemOsg>(SGIItemTypeDataExtents, num));
                }
            }
#endif
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::MaskSource>::build(IObjectTreeItem * treeItem)
{
    osgEarth::MaskSource * object = getObject<osgEarth::MaskSource,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<ElevationQueryReferenced>::build(IObjectTreeItem * treeItem)
{
    ElevationQueryReferenced * query_ref = getObject<ElevationQueryReferenced, SGIItemOsg>();
    osgEarth::ElevationQuery* object = query_ref->get();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            ElevationQueryAccess * access = (ElevationQueryAccess*)object;

            treeItem->addChild("TileCache", cloneItem<SGIItemOsg>(SGIItemTypeTileCache));
        }
        break;
    case SGIItemTypeTileCache:
        {
            treeItem->addChild("LRU", cloneItem<SGIItemOsg>(SGIItemTypeTileCacheLRU));
            treeItem->addChild("Map", cloneItem<SGIItemOsg>(SGIItemTypeTileCacheMap));
            ret = true;
        }
        break;
    case SGIItemTypeTileCacheMap:
        {
            ElevationQueryAccess * access = (ElevationQueryAccess*)object;
            const ElevationQueryAccess::TileCache & tileCache = access->tileCache();

            for(ElevationQueryAccess::TileCache::map_const_iter it = tileCache.map().begin(); it != tileCache.map().end(); it++)
            {
                const osgEarth::TileKey & tilekey = it->first;
                const osg::ref_ptr<osg::HeightField> & hf = it->second.first;
                std::stringstream ss;
                ss << tilekey;
                SGIHostItemOsg tileHf(hf.get());
                if(tileHf.hasObject())
                    treeItem->addChild(ss.str(), &tileHf);
            }
            ret = true;
        }
        break;
    case SGIItemTypeTileCacheLRU:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<TileKeyReferenced>::build(IObjectTreeItem * treeItem)
{
    TileKeyReferenced * object_ref = getObject<TileKeyReferenced, SGIItemOsg>();
    const osgEarth::TileKey & object = object_ref->data();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<TileSourceTileKey>::build(IObjectTreeItem * treeItem)
{
    TileSourceTileKey * object_ref = getObject<TileSourceTileKey, SGIItemOsg>();
    TileSourceTileKeyData & object = object_ref->data();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = callNextHandler(treeItem);
            SGIHostItemOsg tileSource(object.tileSource.get());
            if(tileSource.hasObject())
                treeItem->addChild("TileSource", &tileSource);
            SGIHostItemOsg tileData(object.tileData.get());
            if(tileData.hasObject())
                treeItem->addChild("TileData", &tileData);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::VirtualProgram>::build(IObjectTreeItem * treeItem)
{
    osgEarth::VirtualProgram * object = static_cast<osgEarth::VirtualProgram*>(item<SGIItemOsg>()->object());
    const VirtualProgramAccessor * access = (const VirtualProgramAccessor*)object;
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg templateprg(object->getTemplate());
            if(templateprg.hasObject())
                treeItem->addChild("Template", &templateprg);

            osgEarth::VirtualProgram::ShaderMap shadermap;
            access->getShaderMap(shadermap);
            if(!shadermap.empty())
                treeItem->addChild(helpers::str_plus_count("Shader map", shadermap.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramShaderMap));

            osgEarth::ShaderComp::FunctionLocationMap functions;
            access->getFunctions(functions);
            if(!functions.empty())
                treeItem->addChild(helpers::str_plus_count("Functions", functions.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramFunctions));
        }
        break;
    case SGIItemTypeVirtualProgramShaderMap:
        {
            osgEarth::VirtualProgram::ShaderMap shadermap;
            access->getShaderMap(shadermap);
            unsigned shaderNum = 0;
            for(osgEarth::VirtualProgram::ShaderMap::const_iterator it = shadermap.begin(); it != shadermap.end(); it++, shaderNum++)
            {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
                const std::string & name = it->first;
				const osgEarth::VirtualProgram::ShaderEntry & entry = it->second;
#else
				const auto & id = it->key();
				const auto & entry = it->data();
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
				const auto & shader = entry._shader;
				const osg::StateAttribute::OverrideValue & overrideValue = entry._overrideValue;
#else
                const osg::ref_ptr<osg::Shader> & shader = entry.first;
                const osg::StateAttribute::OverrideValue & overrideValue = entry.second;
#endif

                std::stringstream ss;
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
				ss << name;
#else
				ss << "0x" << std::hex << id;
#endif
				ss << '(' << glOverrideValueName(overrideValue) << ')';
                SGIHostItemOsg child(shader.get());
                treeItem->addChild(ss.str(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramFunctions:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Cache>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Cache * object = static_cast<osgEarth::Cache*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
			SGIHostItemOsgEarthConfigOptions cacheOptions(object->getCacheOptions());
			treeItem->addChild("Options", &cacheOptions);

            SGIHostItemOsg defaultBin(((CacheAccess*)object)->getDefaultBin());
            if(defaultBin.hasObject())
                treeItem->addChild("Default bin", &defaultBin);

            const ThreadSafeCacheBinMapAccessor::MAP & cacheBinMap = ((const ThreadSafeCacheBinMapAccessor&)((CacheAccess*)object)->getCacheBinMap())._data;
            if(!cacheBinMap.empty())
                treeItem->addChild(helpers::str_plus_count("Cache bins", cacheBinMap.size()), cloneItem<SGIItemOsg>(SGIItemTypeChilds));
        }
        break;
    case SGIItemTypeChilds:
        {
            const ThreadSafeCacheBinMapAccessor::MAP & cacheBinMap = ((const ThreadSafeCacheBinMapAccessor&)((CacheAccess*)object)->getCacheBinMap())._data;
            for(ThreadSafeCacheBinMapAccessor::MAP::const_iterator it = cacheBinMap.begin(); it != cacheBinMap.end(); it++)
            {
                const std::string & name = it->first;
                const osg::ref_ptr<osgEarth::CacheBin> & cachebin = it->second;

                SGIHostItemOsg bin(cachebin.get());
                treeItem->addChild(name, &bin);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::CacheBin>::build(IObjectTreeItem * treeItem)
{
    osgEarth::CacheBin * object = static_cast<osgEarth::CacheBin*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Metadata", cloneItem<SGIItemOsg>(SGIItemTypeConfig));
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Features::FeatureProfile>::build(IObjectTreeItem * treeItem)
{
	osgEarth::Features::FeatureProfile * object = getObject<osgEarth::Features::FeatureProfile, SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			SGIHostItemOsg srs(object->getSRS());
			if (srs.hasObject())
				treeItem->addChild("SRS", &srs);

			SGIHostItemOsg profile(object->getProfile());
			if (profile.hasObject())
				treeItem->addChild("Profile", &profile);
		}
		break;
	case SGIItemTypeConfig:
		ret = true;
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgEarth::Features::FeatureSource>::build(IObjectTreeItem * treeItem)
{
	osgEarth::Features::FeatureSource * object = getObject<osgEarth::Features::FeatureSource, SGIItemOsg>();
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			SGIHostItemOsg featureProfile(object->getFeatureProfile());
			if (featureProfile.hasObject())
				treeItem->addChild("FeatureProfile", &featureProfile);

			SGIHostItemOsgEarthConfigOptions featureSourceOptions(object->getFeatureSourceOptions());
			treeItem->addChild("FeatureSourceOptions", &featureSourceOptions);
		}
		break;
	case SGIItemTypeConfig:
		ret = true;
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgEarth::Features::FeatureModelSource>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::FeatureModelSource * object = getObject<osgEarth::Features::FeatureModelSource,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            FeatureModelSourceAccess * access = (FeatureModelSourceAccess*)object;
            osg::ref_ptr<const osgEarth::Map> map;
            if(access->getMap(map))
            {
                SGIHostItemOsg item(const_cast<osgEarth::Map*>(map.get()));
                treeItem->addChild("Map", &item);
            }

            SGIHostItemOsg featureNodeFactory(access->getFeatureNodeFactory());
            if(featureNodeFactory.hasObject())
                treeItem->addChild("FeatureNodeFactory", &featureNodeFactory);

            SGIHostItemOsg dbOpts(access->getDBOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("Database Options", &dbOpts);

            SGIHostItemOsg featureSource(object->getFeatureSource());
            if(featureSource.hasObject())
                treeItem->addChild("FeatureSource", &featureSource);

            SGIHostItemOsgEarthConfigOptions featureModelOptions(object->getFeatureModelOptions());
            treeItem->addChild("FeatureModelOptions", &featureModelOptions);
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::LevelDBDatabase>::build(IObjectTreeItem * treeItem)
{
	osgEarth::LevelDBDatabase * object = static_cast<osgEarth::LevelDBDatabase*>(item<SGIItemOsg>()->object());
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
			SGIHostItemOsgEarthConfigOptions options(object->getOptions());
			treeItem->addChild("Options", &options);
		}
		break;
	default:
		ret = callNextHandler(treeItem);
		break;
	}
	return ret;
}

bool objectTreeBuildImpl<osgEarth::Config>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Config * object = getObject<osgEarth::Config,SGIItemEarthConfig>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            const osgEarth::ConfigSet& children = object->children();
            for(osgEarth::ConfigSet::const_iterator it = children.begin(); it != children.end(); it++)
            {
                const osgEarth::Config & child = *it;
                SGIHostItemOsgEarthConfig childItem(&child);
                treeItem->addChild(child.key(), &childItem);
            }
            ret = true;
        }
        break;
    default:
        // do nothing here
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ConfigOptions>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ConfigOptions * object = getObject<osgEarth::ConfigOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            SGIHostItemOsgEarthConfig configItem(object->getConfig());
            treeItem->addChild("Config", &configItem);

            ret = true;
        }
        break;
    default:
        // do nothing here
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ModelLayerOptions>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ModelLayerOptions * object = getObject<osgEarth::ModelLayerOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsgEarthConfigOptions driver(object->driver());
            if(driver.hasObject())
                treeItem->addChild("Driver", &driver);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Features::FeatureModelSourceOptions>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::FeatureModelSourceOptions * object = getObject<osgEarth::Features::FeatureModelSourceOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsgEarthConfigOptions featureOptions(object->featureOptions());
            if(featureOptions.hasObject())
                treeItem->addChild("FeatureOptions", &featureOptions);

            SGIHostItemOsg featureSource(object->featureSource().get());
            if(featureSource.hasObject())
                treeItem->addChild("FeatureSource", &featureSource);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Drivers::FeatureGeomModelOptions>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Drivers::FeatureGeomModelOptions * object = getObject<osgEarth::Drivers::FeatureGeomModelOptions,SGIItemEarthConfigOptions>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsgEarthConfigOptions compilerOptions(object->compilerOptions());
            if(compilerOptions.hasObject())
                treeItem->addChild("CompilerOptions", &compilerOptions);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_REGISTER(ISceneGraphDialog)

struct RegistrySingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        SGIHostItemOsg hostItem(osgEarth::Registry::instance());
        hostInterface->generateItem(item, &hostItem);
        return item.release();
    }
};

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

	SGIHostItemOsg hostItem(new SGIProxyItemT<RegistrySingleton>(_hostInterface, "osgEarth::Registry"));
	treeItem->addChild(std::string(), &hostItem);

	SGIItemOsg * osgitem = dynamic_cast<SGIItemOsg *>(object->item());
	if (osgitem)
	{
        osg::Node * node = NULL;
        osg::View * view = dynamic_cast<osg::View*>(osgitem->object());
        if(view)
            node = view->getCamera();
        else
            node = dynamic_cast<osg::Node*>(osgitem->object());
        if (node)
		{
			osgEarth::MapNode * mapNode = osg_helpers::findRelativeNodeOfType<osgEarth::MapNode>(node);
			if (mapNode)
			{
				SGIHostItemOsg hostItem(mapNode);
				treeItem->addChild(std::string(), &hostItem);
			}

			if(!view)
            {
                osg::Camera * camera = osg_helpers::findRelativeNodeOfType<osg::Camera>(node);
                if (camera)
                    view = camera->getView();
            }
            if (view)
            {
                osgEarth::Util::Controls::ControlCanvas * canvas = osgEarth::Util::Controls::ControlCanvas::get(view);
                if (canvas)
                {
                    SGIHostItemOsg hostItem(canvas);
                    treeItem->addChild(std::string(), &hostItem);
                }
            }
		}
	}
    return true;
}

} // namespace osgearth_plugin
} // namespace sgi
