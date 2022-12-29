#define final
#include "stdafx.h"
#include "ObjectTreeOSGEarth.h"
#include "string_helpers.h"
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/SGIItemOsg>
#include "SGIItemOsgEarth"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/helpers/string>
#include <sgi/helpers/osg>

#include <osgEarth/Version>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#define protected public
#include <osgEarth/ElevationPool>
#undef protected
#endif
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarth/MaskLayer>
#endif
#include <osgEarth/Registry>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarth/ShaderFactory>
#include <osgEarth/ResourceReleaser>
#include <osgEarth/VideoLayer>
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#include <osgEarth/ElevationPool>
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/Sky>
#elif OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/Controls>
#include <osgEarth/RTTPicker>
#else
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/RTTPicker>
#include <osgEarth/TaskService>
#endif

#include <osgEarth/Capabilities>
#include <osgEarth/StateSetCache>
#include <osgEarth/OverlayDecorator>
#include <osgEarth/TraversalData>

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/FeatureModelLayer>
#elif OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarthFeatures/FeatureSourceLayer>
#include <osgEarthFeatures/FeatureModelLayer>
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/FeatureModelGraph>

#include <osgEarth/Style>

#include <osgEarth/AnnotationNode>
#else
#include <osgEarthFeatures/FeatureModelGraph>

#include <osgEarthSymbology/Style>

#include <osgEarthAnnotation/LabelNode>
#endif

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#endif

#include "osgearth_accessor.h"
#include "geo_helpers.h"

namespace sgi {

namespace osgearth_plugin {

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Extension)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Registry)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::StateSetCache)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::SkyNode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Control)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Container)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlCanvas)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlNodeBin)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlNode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::LabelControl)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ImageControl)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Layer)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::TileLayer)
#else
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainLayer)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ImageLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelLayer)
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::MaskLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::MaskSource)
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::VideoLayer)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Terrain)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainEngineNode)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::TileSource)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(TileSourceInfo)

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Contrib::TileBlacklist)
#else
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::TileBlacklist)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelSource)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(ElevationQueryReferenced)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(TileKeyReferenced)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(TileSourceTileKey)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::VirtualProgram)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Cache)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::CacheBin)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::CacheSettings)
#if OSGEARTH_VERSION_LESS_THAN(3,3,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::PolyShader)
#endif
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationPool)
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationPool::Tile)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationEnvelope)
#endif
#endif

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Picker)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::RTTPicker)

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::FeatureProfile)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::FeatureSource)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Feature)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::FeatureModelLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::FeatureModelGraph)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::FeatureDisplayLayout)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Style)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::StyleSelector)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::StyleSheet)
#else
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureProfile)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureSource)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::Feature)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureModelSource)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureSourceLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureModelLayer)
#endif
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureModelGraph)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureDisplayLayout)

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Symbology::Style)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Symbology::StyleSelector)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Symbology::StyleSheet)
#endif

OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Config)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ConfigOptions)
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelLayerOptions)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Features::FeatureModelSourceOptions)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Drivers::FeatureGeomModelOptions)
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::AnnotationNode)
#else
OBJECT_TREE_BUILD_IMPL_DECLARE_AND_REGISTER(osgEarth::Annotation::AnnotationNode)
#endif

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::StageMaskValues & t);

using namespace osg_helpers;

bool objectTreeBuildImpl<osgEarth::Map>::build(IObjectTreeItem * treeItem)
{
    MapAccess * object = static_cast<MapAccess*>(getObject<osgEarth::Map,SGIItemOsg>());
    SGIRefPtrOsg * refptr = _item->userData<SGIRefPtrOsg>();
    osg::Referenced * ref = refptr ? refptr->get() : nullptr;
    osgEarth::MapNode * mapNode = dynamic_cast<osgEarth::MapNode*>(ref);
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            osgEarth::MapCallbackList callbacks;
            object->getMapCallbacks(callbacks);

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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
            SGIHostItemOsg elevationPool(object->getElevationPool());
            if(elevationPool.hasObject())
                treeItem->addChild("ElevationPool", &elevationPool);
#endif

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsg globalOpts(object->getGlobalOptions());
            if(globalOpts.hasObject())
                treeItem->addChild("GlobalOptions", &globalOpts);
#endif

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg dbOpts(object->getDBOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("DBOptions", &dbOpts);
#else
            SGIHostItemOsg readOpts(object->getReadOptions());
            if(readOpts.hasObject())
                treeItem->addChild("ReadOptions", &readOpts);
#endif

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsgEarthConfigOptions initialOptions(object->getInitialMapOptions());
            treeItem->addChild("Initial Options", &initialOptions);

            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getMapOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
#endif

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#else
            unsigned numLayers = object->getNumLayers();
            if(numLayers)
                treeItem->addChild(helpers::str_plus_count("Layers", numLayers), cloneItem<SGIItemOsg>(SGIItemTypeLayers));
#endif
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

            osgEarth::MapCallbackList callbacks;
            object->getMapCallbacks(callbacks);
            for(osgEarth::MapCallbackList::const_iterator it = callbacks.begin(); it != callbacks.end(); it++)
            {
                SGIHostItemOsg callback(*it);
                if(callback.hasObject())
                    treeItem->addChild(std::string(), &callback);
            }
        }
        break;
    case SGIItemTypeLayers:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            osgEarth::LayerVector layers;
            osgEarth::Revision rev = object->getLayers(layers);
            for(osgEarth::LayerVector::const_iterator it = layers.begin(); it != layers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::Layer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), new SGIRefPtrOsg(mapNode));
                treeItem->addChild(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeElevationLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ELEVATION_LAYERS);
            const osgEarth::ElevationLayerVector & elevationLayers = mapframe.elevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = elevationLayers.begin(); it != elevationLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ElevationLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeImageLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::IMAGE_LAYERS);
            const osgEarth::ImageLayerVector & imageLayers = mapframe.imageLayers();
            for(osgEarth::ImageLayerVector::const_iterator it = imageLayers.begin(); it != imageLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ImageLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeModelLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MODEL_LAYERS);
            const osgEarth::ModelLayerVector & modelLayers = mapframe.modelLayers();
            for(osgEarth::ModelLayerVector::const_iterator it = modelLayers.begin(); it != modelLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ModelLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeMaskLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MASK_LAYERS);
            const osgEarth::MaskLayerVector & maskLayers = mapframe.terrainMaskLayers();
            for(osgEarth::MaskLayerVector::const_iterator it = maskLayers.begin(); it != maskLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::MaskLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get(), mapNode);
                treeItem->addChild(std::string(), &childItem);
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

bool objectTreeBuildImpl<osgEarth::MapNode>::build(IObjectTreeItem * treeItem)
{
    MapNodeAccess * object = static_cast<MapNodeAccess *>(getObject<osgEarth::MapNode, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg map(object->getMap(), new SGIRefPtrOsg(object));
            if(map.hasObject())
                treeItem->addChild("Map", &map);

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsgEarthConfigOptions runtimeOptions(object->getMapNodeOptions());
            treeItem->addChild("Runtime Options", &runtimeOptions);
#endif

			const auto & extensions = object->getExtensions();
			if(!extensions.empty())
				treeItem->addChild("Extensions", cloneItem<SGIItemOsg>(SGIItemTypeExtensions));

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            treeItem->addChild("CullData", cloneItem<SGIItemOsg>(SGIItemTypeCullData, ~0u));

            SGIHostItemOsg terrain(object->terrain());
            if(terrain.hasObject())
                treeItem->addChild("Terrain", &terrain);
            SGIHostItemOsg terrainEngine(object->terrainEngineNode());
            if(terrainEngine.hasObject())
                treeItem->addChild("TerrainEngine", &terrainEngine);
            SGIHostItemOsg terrainEngineContainer(object->terrainEngineContainer());
			if (terrainEngineContainer.hasObject())
				treeItem->addChild("TerrainEngineContainer", &terrainEngineContainer);
#elif OSGEARTH_VERSION_LESS_THAN(3,3,0)
            SGIHostItemOsg terrainEngine(object->getTerrainEngine());
            if(terrainEngine.hasObject())
                treeItem->addChild("TerrainEngine", &terrainEngine);
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            SGIHostItemOsg layerNodeGroup(object->getLayerNodeGroup());
            if(layerNodeGroup.hasObject())
                treeItem->addChild("LayerNodeGroup", &layerNodeGroup);
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
            SGIHostItemOsg modelLayerGroup(object->getModelLayerGroup());
            if(modelLayerGroup.hasObject())
                treeItem->addChild("ModelLayerGroup", &modelLayerGroup);
#endif
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsg overlayDecorator(object->getOverlayDecorator());
            if(overlayDecorator.hasObject())
                treeItem->addChild("OverlayDecorator", &overlayDecorator);
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0) && OSGEARTH_VERSION_LESS_THAN(3,3,0)
            SGIHostItemOsg resourceReleaser(object->getResourceReleaser());
            if(resourceReleaser.hasObject())
                treeItem->addChild("ResourceReleaser", &resourceReleaser);
#endif
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
            ret = true;
		}
		break;
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
    case SGIItemTypeCullData:
        {
            osg::NodeList cameras;
            object->getCullDataCameras(cameras);
            if (itemNumber() == ~0u)
            {
                unsigned index = 0;
                for (const auto & camera : cameras)
                {
                    SGIHostItemOsg item(camera);
                    treeItem->addChild(helpers::str_plus_info("Camera", (void*)camera), cloneItem<SGIItemOsg>(SGIItemTypeCullData, index));
                    ++index;
                }
            }
            else
            {
                unsigned index = 0;
                for (const auto & camera : cameras)
                {
                    if (index == itemNumber())
                    {
                        const osgEarth::MapNodeCullData * data = object->getCullDataForCamera(static_cast<osg::Camera*>(camera.get()));
                        if (data)
                        {
                            SGIHostItemOsg stateSet(data->_stateSet.get());
                            treeItem->addChild("StateSet", &stateSet);
                            SGIHostItemOsg windowMatrixUniform(data->_windowMatrixUniform.get());
                            treeItem->addChild("windowMatrixUniform", &windowMatrixUniform);
                            SGIHostItemOsg cameraAltitudeUniform(data->_cameraAltitudeUniform.get());
                            treeItem->addChild("CameraAltitudeUniform", &cameraAltitudeUniform);
                        }
                    }
                    ++index;
                }
            }
            ret = true;
        }
        break;
#endif
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::Extension>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Extension * object = getObject<osgEarth::Extension, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            SGIHostItemOsgEarthConfigOptions configOptions(object->getConfigOptions());
            treeItem->addChild("Config Options", &configOptions);
#endif
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
    osgEarth::Registry * object = getObject<osgEarth::Registry, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            if(object->getURIReadCallback() )
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg cache(object->getCache());
#else
            SGIHostItemOsg cache(object->getDefaultCache());
#endif
            if(cache.hasObject())
                treeItem->addChild("Cache", &cache);

			treeItem->addChild("Blacklist", cloneItem<SGIItemOsg>(SGIItemTypeBlacklist));
            treeItem->addChild("Profiles", cloneItem<SGIItemOsg>(SGIItemTypeProfiles));

            SGIHostItemOsg defaultFont(object->getDefaultFont());
            if(defaultFont.hasObject())
                treeItem->addChild("Default Font", &defaultFont);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            SGIHostItemOsg shaderFactory(object->getShaderFactory());
            if(shaderFactory.hasObject())
                treeItem->addChild("ShaderFactory", &shaderFactory);

            SGIHostItemOsg objectIndex(object->getObjectIndex());
            if(objectIndex.hasObject())
                treeItem->addChild("ObjectIndex", &objectIndex);
#endif

            SGIHostItemOsg stateSetCache(object->getStateSetCache());
            if(stateSetCache.hasObject())
                treeItem->addChild("StateSetCache", &stateSetCache);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            treeItem->addChild("ProgramSharedRepo", cloneItem<SGIItemOsg>(SGIItemTypeProgramSharedRepo));
#endif

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg taskServiceManager(object->getTaskServiceManager());
            if(taskServiceManager.hasObject())
                treeItem->addChild("TaskServiceManager", &taskServiceManager);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            SGIHostItemOsg asyncOperationQueue(object->getAsyncOperationQueue());
            if(asyncOperationQueue.hasObject())
                treeItem->addChild("AsyncOperationQueue", &asyncOperationQueue);
#endif
#endif

            SGIHostItemOsg defaultOptions(object->getDefaultOptions());
            if(defaultOptions.hasObject())
                treeItem->addChild("Default options", &defaultOptions);

            SGIHostItemOsg capabilities(&object->getCapabilities());
            if(capabilities.hasObject())
                treeItem->addChild("Capabilities", &capabilities);
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
    case SGIItemTypeProfiles:
        {
            SGIHostItemOsg globalGeodeticProfile(object->getGlobalGeodeticProfile());
            if(globalGeodeticProfile.hasObject())
                treeItem->addChild("Global geodetic profile", &globalGeodeticProfile);

            SGIHostItemOsg globalMercatorProfile(object->getGlobalMercatorProfile());
            if(globalMercatorProfile.hasObject())
                treeItem->addChild("Global mercator profile", &globalMercatorProfile);

            SGIHostItemOsg sphericalMercatorProfile(object->getSphericalMercatorProfile());
            if(sphericalMercatorProfile.hasObject())
                treeItem->addChild("Spherical mercator profile", &sphericalMercatorProfile);

#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
            SGIHostItemOsg cubeProfile(object->getCubeProfile());
            if(cubeProfile.hasObject())
                treeItem->addChild("Cube profile", &cubeProfile);
#endif
            ret = true;
        }
        break;
    case SGIItemTypeProgramSharedRepo:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            //const osgEarth::ProgramSharedRepo * repo = object->getProgramSharedRepo();
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && defined(SGI_USE_OSGEARTH_FAST)
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
    LayerAccessor * object = static_cast<LayerAccessor*>(getObject<osgEarth::Layer,SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,8,0)
            SGIHostItemOsg readOptions(object->getReadOptions());
            if(readOptions.hasObject())
                treeItem->addChild("Read options", &readOptions);
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            treeItem->addChild("Config", cloneItem<SGIItemOsg>(SGIItemTypeConfig));

            SGIHostItemOsg cacheSettings(object->getCacheSettings());
            if (cacheSettings.hasObject())
                treeItem->addChild("Cache Settings", &cacheSettings);

            LayerAccessor::LayerCallbackList callbacks;
            object->getLayerCallbacks(callbacks);

            if (!callbacks.empty())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));
#endif
        }
        break;
    case SGIItemTypeConfig:
        {
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            // first add all callbacks from base classes
            callNextHandler(treeItem);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            LayerAccessor::LayerCallbackList callbacks;
            object->getLayerCallbacks(callbacks);
            for(LayerAccessor::LayerCallbackList::const_iterator it = callbacks.begin(); it != callbacks.end(); it++)
            {
                SGIHostItemOsg callback(*it);
                if(callback.hasObject())
                    treeItem->addChild(std::string(), &callback);
            }
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
bool objectTreeBuildImpl<osgEarth::TerrainLayer>::build(IObjectTreeItem * treeItem)
{
    TerrainLayerAccessor * object = static_cast<TerrainLayerAccessor*>(getObject<osgEarth::TerrainLayer,SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgEarth::Profile * profile = object->profileNoInit();
            const osgEarth::Profile * targetProfileHint = object->targetProfileHintNoInit();

            SGIHostItemOsg profileItem(profile);
            if(profile)
                treeItem->addChild("Profile", &profileItem);
            SGIHostItemOsg targetProfileItem(targetProfileHint);
            if(targetProfileHint && targetProfileHint != profile)
                treeItem->addChild("Target profile", &targetProfileItem);

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
            SGIHostItemOsg tileSource(object->tileSourceNoInit());
            if(tileSource.hasObject())
                treeItem->addChild("Tile source", &tileSource);
#elif OSGEARTH_VERSION_LESS_THAN(2,8,0)
			SGIHostItemOsg tileSource((osg::Referenced*)NULL);
			if (object->tileSourceInitAttempted())
				tileSource = object->getTileSource();
            if(tileSource.hasObject())
                treeItem->addChild("Tile source", &tileSource);
#else
            SGIHostItemOsg tileSource(object->getTileSource());
            if (tileSource.hasObject())
                treeItem->addChild("Tile source", &tileSource);
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            SGIHostItemOsg dbOptions(object->dbOptions());
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
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#else
bool objectTreeBuildImpl<osgEarth::TileLayer>::build(IObjectTreeItem * treeItem)
{
    TileLayerAccessor * object = static_cast<TileLayerAccessor*>(getObject<osgEarth::TileLayer,SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            const osgEarth::Profile * profile = object->profileNoInit();

            SGIHostItemOsg profileItem(profile);
            if(profile)
                treeItem->addChild("Profile", &profileItem);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif

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
    SGIRefPtrOsg * refptr = _item->userData<SGIRefPtrOsg>();
    osg::Referenced * ref = refptr ? refptr->get() : nullptr;
    osgEarth::MapNode * mapnode = dynamic_cast<osgEarth::MapNode*>(ref);
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsg maskSource(object->getMaskSource());
            if(maskSource.hasObject())
                treeItem->addChild("Mask source", &maskSource);
#endif

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            SGIHostItemOsg modelNode(mapNode?mapNode->getModelLayerNode(object):NULL);
            if(modelNode.hasObject())
                treeItem->addChild("Model node", &modelNode);

            SGIHostItemOsgEarthConfigOptions initialOptions(object->getModelLayerOptions());
            treeItem->addChild("Initial Options", &initialOptions);
#endif

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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
bool objectTreeBuildImpl<osgEarth::MaskLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::MaskLayer * object = getObject<osgEarth::MaskLayer, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg maskSource(object->getMaskSource());
            if(maskSource.hasObject())
                treeItem->addChild("Mask source", &maskSource);
#endif

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
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
bool objectTreeBuildImpl<osgEarth::VideoLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::VideoLayer * object = getObject<osgEarth::VideoLayer, SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg texture(object->getTexture());
            if(texture.hasObject())
                treeItem->addChild("Texture", &texture);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif

bool objectTreeBuildImpl<osgEarth::Terrain>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Terrain * object = getObject<osgEarth::Terrain,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg profile(object->getProfile());
            if (profile.hasObject())
                treeItem->addChild("Profile", &profile);
            SGIHostItemOsg srs(object->getSRS());
            if (srs.hasObject())
                treeItem->addChild("SRS", &srs);
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
            if (object->getComputeRangeCallback())
                treeItem->addChildIfNotExists("Callbacks", cloneItem<SGIItemOsg>(SGIItemTypeCallbacks));

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
#else
            SGIHostItemOsg surfacestateset(object->getSurfaceStateSet());
            if (surfacestateset.hasObject())
                treeItem->addChild("SurfaceStateSet", &surfacestateset);
#endif
		}
		break;
    case SGIItemTypeCallbacks:
        callNextHandler(treeItem);
        {
            SGIHostItemOsg computeRangeCallback(object->getComputeRangeCallback());
            if (computeRangeCallback.hasObject())
                treeItem->addChild("ComputeRangeCallback", &computeRangeCallback);
            ret = true;
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

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            const osgEarth::TileSourceOptions & tileSourceOpts = object->getOptions();
            SGIHostItemOsgEarthConfigOptions runtimeOptions(tileSourceOpts);
            treeItem->addChild("Runtime Options", &runtimeOptions);
#endif

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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::Contrib::TileBlacklist>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Contrib::TileBlacklist * object = dynamic_cast<osgEarth::Contrib::TileBlacklist*>(item<SGIItemOsg>()->object());
#else
bool objectTreeBuildImpl<osgEarth::TileBlacklist>::build(IObjectTreeItem * treeItem)
{
    osgEarth::TileBlacklist * object = dynamic_cast<osgEarth::TileBlacklist*>(item<SGIItemOsg>()->object());
#endif
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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            const osgEarth::NodeOperationVector& preMergeOperations = object->preMergeOperations();
            if (!preMergeOperations.empty())
                treeItem->addChild(helpers::str_plus_count("Pre-Merge operations", preMergeOperations.size()), cloneItem<SGIItemOsg>(SGIItemTypePreMergeOps, ~0u));

            const osgEarth::NodeOperationVector& postMergeOperations = object->postMergeOperations();
            if (!postMergeOperations.empty())
                treeItem->addChild(helpers::str_plus_count("Post-Merge operations", postMergeOperations.size()), cloneItem<SGIItemOsg>(SGIItemTypePostMergeOps, ~0u));
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)

        }
        break;
    case SGIItemTypePreMergeOps:
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && OSGEARTH_VERSION_LESS_THAN(2,9,0)
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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0) && OSGEARTH_VERSION_LESS_THAN(2,9,0)
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

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
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
#endif

bool objectTreeBuildImpl<ElevationQueryReferenced>::build(IObjectTreeItem * treeItem)
{
    ElevationQueryReferenced * query_ref = getObject<ElevationQueryReferenced, SGIItemOsg>();
    ElevationQueryAccess* object = static_cast<ElevationQueryAccess*>(query_ref->get());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("TileCache", cloneItem<SGIItemOsg>(SGIItemTypeTileCache));
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
            SGIHostItemOsg map(object->getMap());
            if (map.hasObject())
                treeItem->addChild("Map", &map);
#endif
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
            const ElevationQueryAccess::TileCache & tileCache = object->tileCache();

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
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
            SGIHostItemOsg tileLayer(object.tileLayer.get());
            if (tileLayer.hasObject())
                treeItem->addChild("TileLayer", &tileLayer);
#else
            SGIHostItemOsg terrainLayer(object.terrainLayer.get());
            if (terrainLayer.hasObject())
                treeItem->addChild("TerrainLayer", &terrainLayer);
#endif
            SGIHostItemOsg cacheBin(object.cacheBin.get());
            if (cacheBin.hasObject())
                treeItem->addChild("CacheBin", &cacheBin);
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
    VirtualProgramAccessor * object = static_cast<VirtualProgramAccessor*>(getObject<osgEarth::VirtualProgram, SGIItemOsg>());
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

            treeItem->addChild("Effective Program", cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramEffectiveProgram, ~0u));

            osgEarth::VirtualProgram::ShaderMap shadermap;
            object->getShaderMap(shadermap);
            if(!shadermap.empty())
                treeItem->addChild(helpers::str_plus_count("Shader map", shadermap.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramShaderMap));

            osgEarth::ShaderComp::FunctionLocationMap functions;
            object->getFunctions(functions);
            if(!functions.empty())
                treeItem->addChild(helpers::str_plus_count("Functions", functions.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramFunctions, ~0u));

#if 0
            VirtualProgramAccessor::ProgramMap programCache;
            object->getProgramCache(programCache);
            if(!programCache.empty())
                treeItem->addChild(helpers::str_plus_count("Cache", programCache.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramCache, ~0u));
#endif
        }
        break;
    case SGIItemTypeVirtualProgramShaderMap:
        {
            osgEarth::VirtualProgram::ShaderMap shadermap;
            object->getShaderMap(shadermap);
            unsigned shaderNum = 0;
            for(osgEarth::VirtualProgram::ShaderMap::const_iterator it = shadermap.begin(); it != shadermap.end(); it++, shaderNum++)
            {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
                const std::string & name = it->first;
				const osgEarth::VirtualProgram::ShaderEntry & entry = it->second;
#elif OSGEARTH_VERSION_LESS_THAN(3,0,0)
                const auto & id = it->key();
                const auto & entry = it->data();
#else
                const auto & id = it->first;
                const auto & entry = it->second;
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
                ss << shader->getName() << " Id=0x" << std::hex << id;
#endif
				ss << '(' << glOverrideValueName(overrideValue) << ')';
                SGIHostItemOsg child(shader.get());
                treeItem->addChild(ss.str(), &child);
            }
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramFunctions:
        {
            osgEarth::ShaderComp::FunctionLocationMap functions;
            object->getFunctions(functions);
            if (itemNumber() == ~0u)
            {
                for (osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.begin(); it != functions.end(); it++)
                {
                    const osgEarth::ShaderComp::FunctionLocation & location = it->first;
                    const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                    std::stringstream ss;
                    ss << location;
                    treeItem->addChild(helpers::str_plus_count(ss.str(), orderedFunctions.size()), cloneItem<SGIItemOsg>(SGIItemTypeVirtualProgramFunctions, (unsigned)location));
                }
            }
            else
            {
                osgEarth::ShaderComp::FunctionLocation location = (osgEarth::ShaderComp::FunctionLocation)itemNumber();
                osgEarth::ShaderComp::FunctionLocationMap::const_iterator it = functions.find(location);
                if (it != functions.end())
                {
                    const osgEarth::ShaderComp::OrderedFunctionMap & orderedFunctions = it->second;
                    for (osgEarth::ShaderComp::OrderedFunctionMap::const_iterator it = orderedFunctions.begin(); it != orderedFunctions.end(); it++)
                    {
                        const osgEarth::ShaderComp::Function & function = it->second;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                        SGIHostItemOsg accept(function._accept.get());
                        if(accept.hasObject())
                            treeItem->addChild(helpers::str_plus_info(function._name, "accept"), &accept);
#endif
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeVirtualProgramEffectiveProgram:
        ret = true;
        break;
    case SGIItemTypeVirtualProgramCache:
        {
#if 0
            osgEarth::VirtualProgram::ProgramMap programCache;
            object->getProgramCache(programCache);
            for (auto it = programCache.begin(); it != programCache.end(); it++)
            {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                const osgEarth::ProgramKey & keys = it->first;
#else
                const std::vector< osg::ref_ptr<osg::Shader> > & keys = it->first;
#endif
                for (const auto & k : keys)
                {
                    SGIHostItemOsg item(k.get());
                    if (item.hasObject())
                    {
                        std::string name = k->getName();
                        if (name.empty())
                            name = helpers::str_plus_info("PolyShader", (void*)k.get());
                        treeItem->addChild(name, &item);
                    }
                }
                const osgEarth::VirtualProgram::ProgramEntry & entry = it->second;
                SGIHostItemOsg program(entry._program.get());
                if (program.hasObject())
                {
                    std::string name = entry._program->getName();
                    if (name.empty())
                        name = helpers::str_plus_info("Program", (void*)entry._program.get());
                    treeItem->addChild(name, &program);
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
    osgEarth::CacheBin * object = getObject<osgEarth::CacheBin,SGIItemOsg>();
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)

bool objectTreeBuildImpl<osgEarth::CacheSettings>::build(IObjectTreeItem * treeItem)
{
    osgEarth::CacheSettings * object = getObject<osgEarth::CacheSettings, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg cache(object->getCache());
            if (cache.hasObject())
                treeItem->addChild("Cache", &cache);
            SGIHostItemOsg bin(object->getCacheBin());
            if(bin.hasObject())
                treeItem->addChild("CacheBin", &bin);
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
#if OSGEARTH_VERSION_LESS_THAN(3,3,0)
bool objectTreeBuildImpl<osgEarth::PolyShader>::build(IObjectTreeItem * treeItem)
{
    PolyShaderAccessor * object = static_cast<PolyShaderAccessor*>(getObject<osgEarth::PolyShader, SGIItemOsg>());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild("Source", cloneItem<SGIItemOsg>(SGIItemTypeShaderSource));

            SGIHostItemOsg nominalShader(object->getNominalShader());
            if (nominalShader.hasObject())
                treeItem->addChild("NominalShader", &nominalShader);

            SGIHostItemOsg geometryShader(object->getGeometryShader());
            if (geometryShader.hasObject())
                treeItem->addChild("GeometryShader", &geometryShader);

            SGIHostItemOsg tessellationShader(object->getTessellationShader());
            if (tessellationShader.hasObject())
                treeItem->addChild("TessellationShader", &tessellationShader);
        }
        break;
    case SGIItemTypeShaderSource:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif // OSGEARTH_VERSION_LESS_THAN(3,3,0)

#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
bool objectTreeBuildImpl<osgEarth::ElevationPool>::build(IObjectTreeItem * treeItem)
{
    ElevationPoolAccess * object = static_cast<ElevationPoolAccess*>(getObject<osgEarth::ElevationPool, SGIItemOsg>());
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Elevation Layers", cloneItem<SGIItemOsg>(SGIItemTypeElevationLayers));
            treeItem->addChild("Tile cache", cloneItem<SGIItemOsg>(SGIItemTypeTileCache));
            treeItem->addChild("MRU", cloneItem<SGIItemOsg>(SGIItemTypeTileCacheLRU));

        }
        break;
    case SGIItemTypeElevationLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            const osgEarth::ElevationLayerVector & elevationLayers = object->getElevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = elevationLayers.begin(); it != elevationLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ElevationLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get());
                treeItem->addChild(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeTileCache:
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            ElevationPoolAccess::Tiles tiles;
            object->getTiles(tiles);
            for(ElevationPoolAccess::Tiles::const_iterator it = tiles.begin(); it != tiles.end(); it++)
            {
                const osgEarth::TileKey & key = it->first;
                const osg::observer_ptr<osgEarth::ElevationPool::Tile> & tile = it->second;
                SGIHostItemOsg childItem(tile.get());
                treeItem->addChild(key.str(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeTileCacheLRU:
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            ElevationPoolAccess::MRU mru;
            object->getMRU(mru);
            for(ElevationPoolAccess::MRU::const_iterator it = mru.begin(); it != mru.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ElevationPool::Tile> & tile = *it;
                SGIHostItemOsg childItem(tile.get());
                treeItem->addChild(tile->_key.str(), &childItem);
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

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
bool objectTreeBuildImpl<osgEarth::ElevationPool::Tile>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ElevationPool::Tile * object = getObject<osgEarth::ElevationPool::Tile, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            treeItem->addChild("Heightfield", cloneItem<SGIItemOsg>(SGIItemTypeGeoHeightfield));
        }
        break;
    case SGIItemTypeGeoHeightfield:
        {
            SGIHostItemOsg hf(object->_hf.getHeightField());
            if(hf.hasObject())
                treeItem->addChild("Heightfield", &hf);
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<osgEarth::ElevationEnvelope>::build(IObjectTreeItem * treeItem)
{
    osgEarth::ElevationEnvelope * object = getObject<osgEarth::ElevationEnvelope, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

#endif

bool objectTreeBuildImpl<osgEarth::Picker>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Picker * object = getObject<osgEarth::Picker, SGIItemOsg, DynamicCaster>();
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

bool objectTreeBuildImpl<osgEarth::Util::RTTPicker>::build(IObjectTreeItem * treeItem)
{
    RTTPickerAccess * object = static_cast<RTTPickerAccess*>(getObject<osgEarth::Util::RTTPicker, SGIItemOsg, DynamicCaster>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsg group(object->getGroup());
            if (group.hasObject())
                treeItem->addChild("Group", &group);
            SGIHostItemOsg defaultCallback(object->getDefaultCallback());
            if (defaultCallback.hasObject())
                treeItem->addChild("DefaultCallback", &defaultCallback);

            RTTPickerAccess::PickContexts contexts;
            object->getPickContexts(contexts);
            unsigned i = 0;
            for(const auto & context : contexts)
            {
                treeItem->addChild(helpers::str_plus_number("Picker", i), cloneItem<SGIItemOsg>(SGIItemTypePickerContext, i));
                ++i;
            }
        }
        break;
    case SGIItemTypePickerContext:
        {
            RTTPickerAccess::PickContexts contexts;
            object->getPickContexts(contexts);
            unsigned i = 0;
            for (const auto & context : contexts)
            {
                if(i == itemNumber())
                {
                    SGIHostItemOsg view(context._view.get());
                    if (view.hasObject())
                        treeItem->addChild("View", &view);
                    SGIHostItemOsg camera(context._pickCamera.get());
                    if (camera.hasObject())
                        treeItem->addChild("Camera", &camera);
                    SGIHostItemOsg image(context._image.get());
                    if (image.hasObject())
                        treeItem->addChild("Image", &image);
                    SGIHostItemOsg tex(context._tex.get());
                    if (tex.hasObject())
                        treeItem->addChild("Tex", &tex);
                    break;
                }
                ++i;
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::FeatureProfile>::build(IObjectTreeItem * treeItem)
{
    osgEarth::FeatureProfile * object = getObject<osgEarth::FeatureProfile, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Features::FeatureProfile>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::FeatureProfile * object = getObject<osgEarth::Features::FeatureProfile, SGIItemOsg>();
#endif
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

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
			SGIHostItemOsg profile(object->getProfile());
			if (profile.hasObject())
                treeItem->addChild("Profile", &profile);
#endif
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::FeatureSource>::build(IObjectTreeItem * treeItem)
{
    FeatureSourceAccess * object = static_cast<FeatureSourceAccess*>(getObject<osgEarth::FeatureSource, SGIItemOsg>());
#else
bool objectTreeBuildImpl<osgEarth::Features::FeatureSource>::build(IObjectTreeItem * treeItem)
{
    FeatureSourceAccess * object = static_cast<FeatureSourceAccess*>(getObject<osgEarth::Features::FeatureSource, SGIItemOsg>());
#endif
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(treeItem);
		if (ret)
		{
            SGIHostItemOsg readOptions(object->getReadOptions());
            if (readOptions.hasObject())
                treeItem->addChild("Read options", &readOptions);

			SGIHostItemOsg featureProfile(object->getFeatureProfile());
			if (featureProfile.hasObject())
				treeItem->addChild("FeatureProfile", &featureProfile);

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsgEarthConfigOptions featureSourceOptions(object->getFeatureSourceOptions());
            treeItem->addChild("FeatureSourceOptions", &featureSourceOptions);
#endif

            int numFeatures = object->getFeatureCount();
            std::string name = numFeatures > 0 ? helpers::str_plus_count("Features", object->getFeatureCount()) : std::string("Features");
            treeItem->addChild(name, cloneItem<SGIItemOsg>(SGIItemTypeFeatureSourceFeatures));
		}
		break;
	case SGIItemTypeConfig:
		ret = true;
		break;
    case SGIItemTypeFeatureSourceFeatures:
        {
            auto * cursor = object->createFeatureCursor(nullptr);
            if (cursor)
            {
                while (cursor->hasMore())
                {
                    auto * feature = cursor->nextFeature();
                    if (feature)
                    {
                        SGIHostItemOsg item(feature);
                        treeItem->addChild(helpers::str_plus_number("Feature", feature->getFID()), &item);
                    }
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::Feature>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Feature * object = getObject<osgEarth::Feature, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Features::Feature>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::Feature * object = getObject<osgEarth::Features::Feature, SGIItemOsg>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild("Config", cloneItem<SGIItemOsg>(SGIItemTypeConfig));
            auto style = object->style();
            if(style.isSet())
                treeItem->addChild("Style", cloneItem<SGIItemOsg>(SGIItemTypeStyle));
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    case SGIItemTypeStyle:
        ret = true;
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
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

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg dbOpts(access->getDBOptions());
            if(dbOpts.hasObject())
                treeItem->addChild("Database Options", &dbOpts);
#endif

            SGIHostItemOsg featureSource(object->getFeatureSource());
            if(featureSource.hasObject())
                treeItem->addChild("FeatureSource", &featureSource);

            const auto& featureModelOptions = object->getFeatureModelOptions();

            if(featureModelOptions.layout().isSet())
            {
                SGIHostItemOsg layout(&featureModelOptions.layout().value());
                if (layout.hasObject())
                    treeItem->addChild("Layout", &layout);
            }

            SGIHostItemOsgEarthConfigOptions featureModelOptionsItem(featureModelOptions);
            treeItem->addChild("FeatureModelOptions", &featureModelOptionsItem);
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
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0) && OSGEARTH_VERSION_LESS_THAN(3,0,0)
bool objectTreeBuildImpl<osgEarth::Features::FeatureSourceLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::FeatureSourceLayer * object = getObject<osgEarth::Features::FeatureSourceLayer,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg featureSource(object->getFeatureSource());
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
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::FeatureModelLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::FeatureModelLayer * object = getObject<osgEarth::FeatureModelLayer,SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Features::FeatureModelLayer>::build(IObjectTreeItem * treeItem)
{
    osgEarth::Features::FeatureModelLayer * object = getObject<osgEarth::Features::FeatureModelLayer,SGIItemOsg>();
#endif
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if(ret)
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsgEarthConfigOptions featureModelLayerOptions(object->getFeatureModelLayerOptions());
            treeItem->addChild("FeatureModelLayerOptions", &featureModelLayerOptions);
#endif

            SGIHostItemOsg node(object->getNode());
            if(node.hasObject())
                treeItem->addChild("Node", &node);

            SGIHostItemOsg featureSource(object->getFeatureSource());
            if (featureSource.hasObject())
                treeItem->addChild("FeatureSource", &featureSource);

            SGIHostItemOsg createFeatureNodeFactoryCallback(object->getCreateFeatureNodeFactoryCallback());
            if (createFeatureNodeFactoryCallback.hasObject())
                treeItem->addChild("CreateFeatureNodeFactoryCallback", &createFeatureNodeFactoryCallback);
        }
        break;
    case SGIItemTypeCallbacks:
        ret = callNextHandler(treeItem);
        if(ret)
        {
            SGIHostItemOsg createFeatureNodeFactoryCallback(object->getCreateFeatureNodeFactoryCallback());
            if (createFeatureNodeFactoryCallback.hasObject())
                treeItem->addChild("CreateFeatureNodeFactoryCallback", &createFeatureNodeFactoryCallback);
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::FeatureModelGraph>::build(IObjectTreeItem* treeItem)
{
    osgEarth::FeatureModelGraph* object = getObject<osgEarth::FeatureModelGraph, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Features::FeatureModelGraph>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Features::FeatureModelGraph* object = getObject<osgEarth::Features::FeatureModelGraph, SGIItemOsg>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {

            SGIHostItemOsg session(object->getSession());
            if (session.hasObject())
                treeItem->addChild("Session", &session);

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            SGIHostItemOsg styles(object->getStyles());
            if (styles.hasObject())
                treeItem->addChild("Styles", &styles);
#endif

            const auto & levels = object->getLevels();
            treeItem->addChild(helpers::str_plus_count("Levels", levels.size()), cloneItem<SGIItemOsg>(SGIItemTypeLevels, ~0u));
        }
        break;
    case SGIItemTypeLevels:
        {
            if(itemNumber() == ~0u)
            {
                const auto& levels = object->getLevels();
                for(unsigned i = 0; i < levels.size(); ++i)
                {
                    const auto * level = levels[i];
                    if(level)
                        treeItem->addChild(helpers::str_plus_number("Level", i) + std::string(":") + level->styleName().value(), cloneItem<SGIItemOsg>(SGIItemTypeLevels, i));
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::FeatureDisplayLayout>::build(IObjectTreeItem* treeItem)
{
    osgEarth::FeatureDisplayLayout* object = getObject<osgEarth::FeatureDisplayLayout, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Features::FeatureDisplayLayout>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Features::FeatureDisplayLayout* object = getObject<osgEarth::Features::FeatureDisplayLayout, SGIItemOsg>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            unsigned num = object->getNumLevels();
            treeItem->addChild(helpers::str_plus_count("Levels", num), cloneItem<SGIItemOsg>(SGIItemTypeChilds, ~0u));
        }
        break;
    case SGIItemTypeChilds:
        {
            if(itemNumber() == ~0u)
            {
                unsigned num = object->getNumLevels();
                for(unsigned i = 0; i < num; ++i)
                {
                    auto * level = object->getLevel(i);
                    if(level)
                        treeItem->addChild(helpers::str_plus_number("Level", i) + std::string(":") + level->styleName().value(), cloneItem<SGIItemOsg>(SGIItemTypeChilds, i));
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
            SGIHostItemOsgEarthConfig configItem(new osgEarth::Config(object->getConfig()));
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

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
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
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsgEarthConfigOptions compilerOptions(object->compilerOptions());
            if(compilerOptions.hasObject())
                treeItem->addChild("CompilerOptions", &compilerOptions);
#endif
        }
        break;
    default:
        ret = callNextHandler(treeItem);
        break;
    }
    return ret;
}
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::Style>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Style* object = getObject<osgEarth::Style, SGIItemEarthStyle>();
#else
bool objectTreeBuildImpl<osgEarth::Symbology::Style>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Symbology::Style* object = getObject<osgEarth::Symbology::Style, SGIItemEarthStyle>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            treeItem->addChild("Config", cloneItem<SGIItemOsg>(SGIItemTypeConfig));
            for(const auto & symbol : object->symbols())
            {
                SGIHostItemOsg item(symbol.get());
                if (item.hasObject())
                    treeItem->addChild(std::string(), &item);
            }
            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::StyleSelector>::build(IObjectTreeItem* treeItem)
{
    osgEarth::StyleSelector* object = getObject<osgEarth::StyleSelector, SGIItemEarthStyleSelector>();
#else
bool objectTreeBuildImpl<osgEarth::Symbology::StyleSelector>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Symbology::StyleSelector* object = getObject<osgEarth::Symbology::StyleSelector, SGIItemEarthStyleSelector>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            treeItem->addChild("Config", cloneItem<SGIItemOsg>(SGIItemTypeConfig));
            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::StyleSheet>::build(IObjectTreeItem* treeItem)
{
    osgEarth::StyleSheet* object = getObject<osgEarth::StyleSheet, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Symbology::StyleSheet>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Symbology::StyleSheet* object = getObject<osgEarth::Symbology::StyleSheet, SGIItemOsg>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            SGIHostItemOsgEarthStyle defaultStyle(object->getDefaultStyle());
            if (defaultStyle.hasObject())
                treeItem->addChild("DefaultStyle", &defaultStyle);

            SGIHostItemOsg defaultResourceLibrary(object->getDefaultResourceLibrary());
            if (defaultResourceLibrary.hasObject())
                treeItem->addChild("DefaultResourceLibrary", &defaultResourceLibrary);

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            const auto & styles = object->styles();
            treeItem->addChild(helpers::str_plus_count("Styles", styles.size()), cloneItem<SGIItemOsg>(SGIItemTypeChilds));

            const auto & selectors = object->selectors();
            treeItem->addChild(helpers::str_plus_count("Selectors", selectors.size()), cloneItem<SGIItemOsg>(SGIItemTypeSelectors));
#else
            const auto & styles = object->getStyles();
            treeItem->addChild(helpers::str_plus_count("Styles", styles.size()), cloneItem<SGIItemOsg>(SGIItemTypeChilds));

            const auto & selectors = object->getSelectors();
            treeItem->addChild(helpers::str_plus_count("Selectors", selectors.size()), cloneItem<SGIItemOsg>(SGIItemTypeSelectors));
#endif

        }
        break;
    case SGIItemTypeChilds:
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            for(const auto & style : object->styles())
#else
            for(const auto & style : object->getStyles())
#endif
            {
                SGIHostItemOsgEarthStyle item(&style.second);
                if (item.hasObject())
                    treeItem->addChild(style.first, &item);
            }
            ret = true;
        }
        break;
    case SGIItemTypeSelectors:
        {
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            for(const auto & selector : object->selectors())
            {
                SGIHostItemOsgEarthStyleSelector item(&selector);
#else
            for(const auto & selector : object->getSelectors())
            {
                SGIHostItemOsgEarthStyleSelector item(selector.second);
#endif
                if (item.hasObject())
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

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
bool objectTreeBuildImpl<osgEarth::AnnotationNode>::build(IObjectTreeItem* treeItem)
{
    osgEarth::AnnotationNode* object = getObject<osgEarth::AnnotationNode, SGIItemOsg>();
#else
bool objectTreeBuildImpl<osgEarth::Annotation::AnnotationNode>::build(IObjectTreeItem* treeItem)
{
    osgEarth::Annotation::AnnotationNode* object = getObject<osgEarth::Annotation::AnnotationNode, SGIItemOsg>();
#endif
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(treeItem);
        if (ret)
        {
            treeItem->addChild("Config", cloneItem<SGIItemOsg>(SGIItemTypeConfig));
            treeItem->addChild("Style", cloneItem<SGIItemOsg>(SGIItemTypeStyle));
            

            SGIHostItemOsg mapNode(object->getMapNode());
            if (mapNode.hasObject())
                treeItem->addChild("MapNode", &mapNode);

            ret = true;
        }
        break;
    case SGIItemTypeConfig:
        ret = true;
        break;
    case SGIItemTypeStyle:
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE_AND_REGISTER(ISceneGraphDialog)

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

	SGIHostItemInternal hostItem(new SGIProxyItemT<RegistrySingleton>(_hostInterface, "osgEarth::Registry"));
	treeItem->addChild(std::string(), &hostItem);

	SGIItemBase* item = object->item();
	SGIItemOsg * osgitem = dynamic_cast<SGIItemOsg *>(item);
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
                // DO NOT add the same MapNode instance twice if it's marked as 'sgi_tree_item'
                bool sgi_tree_item = false;
                mapNode->getUserValue<bool>("sgi_tree_item", sgi_tree_item);
                if (!sgi_tree_item)
                {
                    SGIHostItemOsg hostItem(mapNode);
                    treeItem->addChild(std::string(), &hostItem);
                }
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
