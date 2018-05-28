#include "stdafx.h"
#include "ContextMenuOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>
#include <sgi/helpers/osg>
#include <sgi/helpers/string>

#include <osgEarth/Version>
#include <osgEarth/MapNode>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/RTTPicker>
#include <osgEarth/MaskLayer>
#include <osgEarth/TileSource>
#include <osgEarth/Registry>
#ifdef SGI_USE_OSGEARTH_FAST
#include <osgEarth/LevelDBFactory>
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#include <osgEarth/LineDrawable>
#endif

//#include <osgEarth/TimeControl>
#include "SGIItemOsgEarth"

#include "MenuActionOSGEarth.h"

#include "osgearth_accessor.h"
#include "ElevationQueryReferenced"

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osg::Node)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Registry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Map)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::MapNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Layer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::TerrainLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::ImageLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::ElevationLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::ModelLayer)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::MaskLayer)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::LineDrawable)
#endif

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::SkyNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::AutoClipPlaneCullCallback)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Control)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::Container)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::Controls::ControlCanvas)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::TileSource)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::TileBlacklist)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::VirtualProgram)
#ifdef SGI_USE_OSGEARTH_FAST
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::LevelDBDatabase)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::LODScaleOverrideNode)
#endif
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Cache)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::CacheBin)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(ElevationQueryReferenced)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(TileKeyReferenced)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(TileSourceTileKey)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::RTTPicker)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(osgEarth::Util::EarthManipulator)

bool contextMenuPopulateImpl<osg::Node>::populate(IContextMenuItem * menuItem)
{
    osg::Node * object = static_cast<osg::Node*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionNodeRegenerateShaders, "Re-generate shaders", _item);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Registry>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Registry * object = getObject<osgEarth::Registry,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            osg::NotifySeverity notifyLevel = osgEarth::getNotifyLevel();
            IContextMenuItem * notifyLevelMenu = menuItem->addModeMenu(MenuActionNotifyLevel, "Notify level", _item, notifyLevel);
            if(notifyLevelMenu)
            {
                notifyLevelMenu->addModeAction("Always", osg::ALWAYS);
                notifyLevelMenu->addModeAction("Fatal", osg::FATAL);
                notifyLevelMenu->addModeAction("Warn", osg::WARN);
                notifyLevelMenu->addModeAction("Notice", osg::NOTICE);
                notifyLevelMenu->addModeAction("Info", osg::INFO);
                notifyLevelMenu->addModeAction("Debug info", osg::DEBUG_INFO);
                notifyLevelMenu->addModeAction("Debug FP", osg::DEBUG_FP);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Map>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = callNextHandler(menuItem);
            if(ret)
            {
                SGIHostItemOsg cache(object->getCache());
                if(cache.hasObject())
                    menuItem->addMenu("Cache", &cache);

                IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
                if (manipulateMenu)
                {
                    manipulateMenu->addSimpleAction(MenuActionTileBlacklistClear, "Clear blacklist", _item);
                }

                const osgEarth::MapOptions & mapOptions = object->getMapOptions();
                osgEarth::CachePolicy::Usage cachePolicyUsage = mapOptions.cachePolicy().value().usage().value();

                IContextMenuItem * cacheMenu = menuItem->addModeMenu(MenuActionMapCachePolicyUsage, "Cache usage", _item, cachePolicyUsage);
                if(cacheMenu)
                {
                    cacheMenu->addModeAction("Read/write", osgEarth::CachePolicy::USAGE_READ_WRITE);
                    cacheMenu->addModeAction("Cache only", osgEarth::CachePolicy::USAGE_CACHE_ONLY);
                    cacheMenu->addModeAction("Read only", osgEarth::CachePolicy::USAGE_READ_ONLY);
                    cacheMenu->addModeAction("No cache", osgEarth::CachePolicy::USAGE_NO_CACHE);
                }

                menuItem->addSimpleAction(MenuActionTileInspector, "Tile inspector...", _item);
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
                unsigned numImage = object->getNumImageLayers();
                if(numImage || true)
                    menuItem->addMenu(helpers::str_plus_count("Image layers", numImage), cloneItem<SGIItemOsg>(SGIItemTypeImageLayers));
                unsigned numElev = object->getNumElevationLayers();
                if(numElev)
                    menuItem->addMenu(helpers::str_plus_count("Elevation layers", numElev), cloneItem<SGIItemOsg>(SGIItemTypeElevationLayers));
                unsigned numModel = object->getNumModelLayers();
                if(numModel)
                    menuItem->addMenu(helpers::str_plus_count("Model layers", numModel), cloneItem<SGIItemOsg>(SGIItemTypeModelLayers));
                unsigned numMask = 0;
                {
                    osgEarth::MaskLayerVector maskLayers;
                    object->getTerrainMaskLayers(maskLayers);
                    numMask = maskLayers.size();
                }
                if(numMask)
                    menuItem->addMenu(helpers::str_plus_count("Mask layers", numMask), cloneItem<SGIItemOsg>(SGIItemTypeMaskLayers));
#endif
            }
        }
        break;
    case SGIItemTypeElevationLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::ELEVATION_LAYERS);
            const osgEarth::ElevationLayerVector & elevationLayers = mapframe.elevationLayers();
            for(osgEarth::ElevationLayerVector::const_iterator it = elevationLayers.begin(); it != elevationLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ElevationLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get());
                menuItem->addMenu(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeImageLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            osg::ref_ptr<osgEarth::ImageLayer> imageLayer;
            MapDebugImageLayer debugImageLayer = getDebugImageLayer(object, imageLayer);
            IContextMenuItem * debugImageLayerMenu = menuItem->addModeMenu(MenuActionMapDebugImageLayer, "Debug image layer", _item, debugImageLayer);
            if(debugImageLayerMenu)
            {
                debugImageLayerMenu->addModeAction("Off", MapDebugImageLayerNone);
                debugImageLayerMenu->addModeAction("Normal", MapDebugImageLayerNormal);
                debugImageLayerMenu->addModeAction("Inverted", MapDebugImageLayerInverted);
            }

            osgEarth::MapFrame mapframe(object, osgEarth::Map::IMAGE_LAYERS);
            const osgEarth::ImageLayerVector & imageLayers = mapframe.imageLayers();
            if(!imageLayers.empty())
            {
                menuItem->addSeparator();
                for(osgEarth::ImageLayerVector::const_iterator it = imageLayers.begin(); it != imageLayers.end(); it++)
                {
                    const osg::ref_ptr<osgEarth::ImageLayer> & layer = *it;
                    SGIHostItemOsg childItem(layer.get());
                    menuItem->addMenu(std::string(), &childItem);
                }
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeModelLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MODEL_LAYERS);
            const osgEarth::ModelLayerVector & modelLayers = mapframe.modelLayers();
            for(osgEarth::ModelLayerVector::const_iterator it = modelLayers.begin(); it != modelLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::ModelLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get());
                menuItem->addMenu(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    case SGIItemTypeMaskLayers:
        {
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            osgEarth::MapFrame mapframe(object, osgEarth::Map::MASK_LAYERS);
            const osgEarth::MaskLayerVector & maskLayers = mapframe.terrainMaskLayers();
            for(osgEarth::MaskLayerVector::const_iterator it = maskLayers.begin(); it != maskLayers.end(); it++)
            {
                const osg::ref_ptr<osgEarth::MaskLayer> & layer = *it;
                SGIHostItemOsg childItem(layer.get());
                menuItem->addMenu(std::string(), &childItem);
            }
#endif
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::MapNode>::populate(IContextMenuItem * menuItem)
{
    osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
	MapNodeAccess * access = static_cast<MapNodeAccess*>(object);
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = callNextHandler(menuItem);
            if(ret)
            {
				IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
				if (manipulateMenu)
				{
                    manipulateMenu->addSimpleAction(MenuActionAddExtension, "Add extension...", _item);
                    manipulateMenu->addSimpleAction(MenuActionTileBlacklistClear, "Clear blacklist", _item);
                }

                SGIHostItemOsg map(object->getMap());
                if(map.hasObject())
                    menuItem->addMenu("Map", &map);
				menuItem->addBoolAction(MenuActionMapInspector, "Map inspector", _item, access->hasMapInspector());
				menuItem->addBoolAction(MenuActionTerrainProfile, "Terrain profile", _item, access->isTerrainProfileActive());
                menuItem->addSimpleAction(MenuActionTileInspector, "Tile inspector...", _item);
            }
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::Layer>::populate(IContextMenuItem * menuItem)
{
    LayerAccessor * object = static_cast<LayerAccessor*>(getObject<osgEarth::Layer, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            SGIHostItemOsg cacheSettings(object->getCacheSettings());
            if (cacheSettings.hasObject())
                menuItem->addMenu("Cache settings", &cacheSettings);
#endif

            /*
            SGIHostItemOsg sequencecontrol(object->getSequenceControl());
            if(sequencecontrol.hasObject())
                menuItem->addMenu("Sequence control", &sequencecontrol);
            */
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::TerrainLayer>::populate(IContextMenuItem * menuItem)
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionTileBlacklistClear, "Clear blacklist", _item);
            }

            menuItem->addBoolAction(MenuActionTerrainLayerEnable, "Enable", _item, object->getEnabled());
            menuItem->addBoolAction(MenuActionTerrainLayerVisible, "Visible", _item, object->getVisible());

            std::string url = ((TerrainLayerAccessor*)object)->getURL();
            if(!url.empty())
                menuItem->addSimpleAction(MenuActionTerrainLayerSetURL, helpers::str_plus_info("URL", url), _item);

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg cache(object->getCache());
            if(cache.hasObject())
                menuItem->addMenu("Cache", &cache);

            osgEarth::CachePolicy::Usage cachePolicyUsage = ((TerrainLayerAccessor*)object)->getCachePolicy().usage().value();
            IContextMenuItem * cacheMenu = menuItem->addModeMenu(MenuActionTerrainLayerCacheUsage, "Cache usage", _item, cachePolicyUsage);
            if(cacheMenu)
            {
                cacheMenu->addModeAction("Read/write", osgEarth::CachePolicy::USAGE_READ_WRITE);
                cacheMenu->addModeAction("Cache only", osgEarth::CachePolicy::USAGE_CACHE_ONLY);
                cacheMenu->addModeAction("Read only", osgEarth::CachePolicy::USAGE_READ_ONLY);
                cacheMenu->addModeAction("No cache", osgEarth::CachePolicy::USAGE_NO_CACHE);
            }
#endif

            SGIHostItemOsg tilesource(object->getTileSource());
            if(tilesource.hasObject())
                menuItem->addMenu("Tile source", &tilesource);

            menuItem->addSimpleAction(MenuActionTileInspector, "Tile inspector...", _item);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            if(object->getCacheSettings() && object->getCacheSettings()->getCacheBin())
                menuItem->addSimpleAction(MenuActionTerrainLayerClearCacheTiles, "Clear tiles from cache...", _item);
#else
            if (object->getCache() && object->getCacheBin(object->getProfile()))
                menuItem->addSimpleAction(MenuActionTerrainLayerClearCacheTiles, "Clear tiles from cache...", _item);
#endif
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::ImageLayer>::populate(IContextMenuItem * menuItem)
{
    osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
			menuItem->addSimpleAction(MenuActionImageLayerOpacity, helpers::str_plus_info("Opacity", object->getOpacity()), _item);
			menuItem->addSimpleAction(MenuActionImageLayerMinVisibleRange, helpers::str_plus_info("Min visible", object->getMinVisibleRange()), _item);
			menuItem->addSimpleAction(MenuActionImageLayerMaxVisibleRange, helpers::str_plus_info("Max visible", object->getMaxVisibleRange()), _item);
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::ElevationLayer>::populate(IContextMenuItem * menuItem)
{
    osgEarth::ElevationLayer * object = static_cast<osgEarth::ElevationLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::ModelLayer>::populate(IContextMenuItem * menuItem)
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addBoolAction(MenuActionModelLayerEnable, "Enable", _item, object->getEnabled());
            menuItem->addBoolAction(MenuActionModelLayerVisible, "Visible", _item, object->getVisible());
            menuItem->addBoolAction(MenuActionModelLayerLighting, "Lighting", _item, object->isLightingEnabled());

            SGIHostItemOsg modelsource(object->getModelSource());
            if(modelsource.hasObject())
            {
                menuItem->addMenu("Model source", &modelsource);

                std::string url;
                const osgEarth::ModelSourceOptions & layerOpts = static_cast<osgEarth::ModelSource*>(modelsource.object())->getOptions();
                osgEarth::Config layerConf = layerOpts.getConfig();
                if(layerConf.hasValue("url"))
                    url = layerConf.value("url");
                if(!url.empty())
                    menuItem->addSimpleAction(MenuActionModelLayerSetURL, helpers::str_plus_info("URL", url), _item);
            }
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::MaskLayer>::populate(IContextMenuItem * menuItem)
{
    osgEarth::MaskLayer * object = getObject<osgEarth::MaskLayer,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
            SGIHostItemOsg masksource(object->getMaskSource());
            if(masksource.hasObject())
            {
                menuItem->addMenu("Mask source", &masksource);

                std::string url;
                const osgEarth::MaskSourceOptions & layerOpts = static_cast<osgEarth::MaskSource*>(masksource.object())->getOptions();
                osgEarth::Config layerConf = layerOpts.getConfig();
                if(layerConf.hasValue("url"))
                    url = layerConf.value("url");
                if(!url.empty())
                    menuItem->addSimpleAction(MenuActionMaskLayerSetURL, helpers::str_plus_info("URL", url), _item);
            }
#endif
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)

bool contextMenuPopulateImpl<osgEarth::LineDrawable>::populate(IContextMenuItem * menuItem)
{
    osgEarth::LineDrawable * object = getObject<osgEarth::LineDrawable,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionLineDrawableSetLineWidth, helpers::str_plus_number("Line Width", object->getLineWidth()), _item);
            menuItem->addSimpleAction(MenuActionLineDrawableSetStipplePattern, helpers::str_plus_number("Stipple Pattern", object->getStipplePattern()), _item);
            menuItem->addSimpleAction(MenuActionLineDrawableSetStippleFactor, helpers::str_plus_number("Stipple Factor", object->getStippleFactor()), _item);
            menuItem->addSimpleAction(MenuActionLineDrawableSetColor, helpers::str_plus_number("Color", osg_helpers::vec4fToHtmlColor(object->getColor())), _item);
        }
        break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}
#endif


#ifdef SGI_USE_OSGEARTH_FAST
bool contextMenuPopulateImpl<osgEarth::LevelDBDatabase>::populate(IContextMenuItem * menuItem)
{
	osgEarth::LevelDBDatabase * object = static_cast<osgEarth::LevelDBDatabase*>(item<SGIItemOsg>()->object());
	bool ret = false;
	switch (itemType())
	{
	case SGIItemTypeObject:
		ret = callNextHandler(menuItem);
		if (ret)
		{
			menuItem->addSimpleAction(MenuActionLevelDBDatabaseRead, "Read...", _item);
			menuItem->addSimpleAction(MenuActionLevelDBDatabaseWrite, "Write...", _item);
		}
		break;
	default:
		ret = callNextHandler(menuItem);
		break;
	}
	return ret;
}

bool contextMenuPopulateImpl<osgEarth::LODScaleOverrideNode>::populate(IContextMenuItem * menuItem)
{
    osgEarth::LODScaleOverrideNode * object = getObject<osgEarth::LODScaleOverrideNode,SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addSimpleAction(MenuActionLODScaleOverrideNodeLODScale, helpers::str_plus_number("LOD scale", object->getLODScale()), _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}
#endif

bool contextMenuPopulateImpl<osgEarth::Cache>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Cache * object = getObject<osgEarth::Cache, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addSimpleAction(MenuActionCacheClear, "Clear", _item);
            menuItem->addSimpleAction(MenuActionCacheCompact, "Compact", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::CacheBin>::populate(IContextMenuItem * menuItem)
{
    osgEarth::CacheBin * object = getObject<osgEarth::CacheBin, SGIItemOsg>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            menuItem->addSimpleAction(MenuActionCacheBinClear, "Clear", _item);
            menuItem->addSimpleAction(MenuActionCacheBinCompact, "Compact", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::SkyNode>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode,SGIItemOsg>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionSkyNodeLightSettings, "Light settings...", _item);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            const osgEarth::DateTime & t = object->getDateTime();
#else
            osgEarth::DateTime t;
            object->getDateTime(t);
#endif
            menuItem->addSimpleAction(MenuActionSkyNodeSetDateTime, helpers::str_plus_info("Date/time", t.asRFC1123()), _item);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
            menuItem->addBoolAction(MenuActionSkyNodeSetSunVisible, "Sun", _item, object->getSunVisible());
#endif
            menuItem->addBoolAction(MenuActionSkyNodeSetMoonVisible, "Moon", _item, object->getMoonVisible());
            menuItem->addBoolAction(MenuActionSkyNodeSetStarsVisible, "Stars", _item, object->getStarsVisible());
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::AutoClipPlaneCullCallback>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback,SGIItemOsg,DynamicCaster>();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionAutoClipPlaneCullCallbackSetup, "Setup...", _item);
            menuItem->addSimpleAction(MenuActionAutoClipPlaneCullCallbackMinNearFarRatio, helpers::str_plus_info("MinNearFarRatio", object->getMinNearFarRatio()), _item);
            menuItem->addSimpleAction(MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio, helpers::str_plus_info("MaxNearFarRatio", object->getMaxNearFarRatio()), _item);
            menuItem->addSimpleAction(MenuActionAutoClipPlaneCullCallbackHeightThreshold, helpers::str_plus_info("HeightThreshold", object->getHeightThreshold()), _item);
            menuItem->addBoolAction(MenuActionAutoClipPlaneCullCallbackClampFarClipPlane, "ClampFarClipPlane", _item, object->getClampFarClipPlane());
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::Controls::Control>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::Controls::Control * object = static_cast<osgEarth::Util::Controls::Control*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            ControlAccess * access = (ControlAccess*)object;

            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addBoolAction(MenuActionControlVisible, "Visible", _item, object->visible());
                manipulateMenu->addBoolAction(MenuActionControlDirty, "Dirty", _item, object->isDirty());
                manipulateMenu->addBoolAction(MenuActionControlActive, "Active", _item, object->getActive());
                manipulateMenu->addBoolAction(MenuActionControlAbsorbEvents, "Absorb events", _item, object->getAbsorbEvents());
            }

            SGIHostItemOsg geode(access->geode());
            if(geode.hasObject())
                menuItem->addMenu("Geode", &geode);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::Controls::Container>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::Controls::Container * object = static_cast<osgEarth::Util::Controls::Container*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::Controls::ControlCanvas>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::Controls::ControlCanvas * object = static_cast<osgEarth::Util::Controls::ControlCanvas*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            ControlCanvasAccess* access = (ControlCanvasAccess*)object;
            const osgEarth::Util::Controls::ControlContext & controlContext = access->controlContext();

            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if(manipulateMenu)
            {
                manipulateMenu->addBoolAction(MenuActionControlCanvasDirty, "Dirty", _item, access->isContextDirty());
            }

            SGIHostItemOsg controlNodeBin(access->controlNodeBin());
            if(controlNodeBin.hasObject())
                menuItem->addMenu("ControlNodeBin", &controlNodeBin);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}


bool contextMenuPopulateImpl<osgEarth::TileSource>::populate(IContextMenuItem * menuItem)
{
    osgEarth::TileSource * object = dynamic_cast<osgEarth::TileSource*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            SGIHostItemOsg profile(object->getProfile());
            if(profile.hasObject())
                menuItem->addMenu("Profile", &profile);

            IContextMenuItem * manipulateMenu = menuItem->getOrCreateMenu("Manipulate");
            if (manipulateMenu)
            {
                manipulateMenu->addSimpleAction(MenuActionTileBlacklistClear, "Clear blacklist", _item);
            }

            SGIHostItemOsg blacklist(object->getBlacklist());
            if(blacklist.hasObject())
                menuItem->addMenu("Blacklist", &blacklist);

            menuItem->addSimpleAction(MenuActionTileSourceCreateImage, "Create image...", _item);
            menuItem->addSimpleAction(MenuActionTileSourceCreateHeightField, "Create height field...", _item);
            menuItem->addSimpleAction(MenuActionTileSourceUpdateMetaData, "Update meta data", _item);
            menuItem->addSimpleAction(MenuActionTileInspector, "Tile inspector...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::TileBlacklist>::populate(IContextMenuItem * menuItem)
{
    osgEarth::TileBlacklist * object = dynamic_cast<osgEarth::TileBlacklist*>(item<SGIItemOsg>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionTileBlacklistClear, "Clear", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::VirtualProgram>::populate(IContextMenuItem * menuItem)
{
    VirtualProgramAccessor * object = static_cast<VirtualProgramAccessor*>(getObject<osgEarth::VirtualProgram, SGIItemOsg>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionVirtualProgramMask, helpers::str_plus_hex("Mask", object->mask()), _item);
            menuItem->addBoolAction(MenuActionVirtualProgramLogging, "Logging", _item, object->getShaderLogging());
            menuItem->addSimpleAction(MenuActionVirtualProgramLoggingFile, helpers::str_plus_info("Logfile", object->getShaderLogFile()), _item);

            VirtualProgramInheritMode mode;
            if(!object->inheritSet())
                mode = VirtualProgramInheritModeUnspecified;
            else if(object->inherit())
                mode = VirtualProgramInheritModeEnabled;
            else
                mode = VirtualProgramInheritModeDisabled;
            IContextMenuItem * inheritMenu = menuItem->addModeMenu(MenuActionVirtualProgramInherit, "Inherit", _item, mode);
            if(inheritMenu)
            {
                inheritMenu->addModeAction("Unspecified", VirtualProgramInheritModeUnspecified);
                inheritMenu->addModeAction("Enabled", VirtualProgramInheritModeEnabled);
                inheritMenu->addModeAction("Disabled", VirtualProgramInheritModeDisabled);
            }
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<ElevationQueryReferenced>::populate(IContextMenuItem * menuItem)
{
    ElevationQueryReferenced * query_ref = getObject<ElevationQueryReferenced, SGIItemOsg>();
    osgEarth::ElevationQuery* object = query_ref->get();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            menuItem->addSimpleAction(MenuActionElevationQueryCustom, "Query...", _item);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

namespace {
    void contextMenuPopulate_TileKey(IContextMenuItem * menuItem, SGIItemBase * item, const osgEarth::TileKey & tileKey)
    {
        IContextMenuItem * keyMenu = menuItem->addModeMenu(MenuActionTileKeyAdd, "Add Keys", item, -1);
        if(keyMenu)
        {
            keyMenu->addModeAction("Parent", TileKeyAddModeParent);
            keyMenu->addModeAction("Children", TileKeyAddModeChildren);
            keyMenu->addModeAction("Adjacent Neighbors", TileKeyAddModeNeighborsAdjacent);
            keyMenu->addModeAction("All Neighbors", TileKeyAddModeNeighborsAll);
            keyMenu->addModeAction("Neighbor North", TileKeyAddModeNeighborNorth);
            keyMenu->addModeAction("Neighbor South", TileKeyAddModeNeighborSouth);
            keyMenu->addModeAction("Neighbor West", TileKeyAddModeNeighborWest);
            keyMenu->addModeAction("Neighbor East", TileKeyAddModeNeighborEast);
        }
    }
}

bool contextMenuPopulateImpl<TileKeyReferenced>::populate(IContextMenuItem * menuItem)
{
    TileKeyReferenced * data_ref = getObject<TileKeyReferenced, SGIItemOsg>();
    const osgEarth::TileKey & object = data_ref->data();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            contextMenuPopulate_TileKey(menuItem, _item.get(), object);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<TileSourceTileKey>::populate(IContextMenuItem * menuItem)
{
    TileSourceTileKey * data_ref = getObject<TileSourceTileKey, SGIItemOsg>();
    const TileSourceTileKeyData & object = data_ref->data();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if(ret)
        {
            contextMenuPopulate_TileKey(menuItem, _item.get(), object.tileKey);

            bool addCacheActions = false;
            SGIHostItemOsg tileSource(object.tileSource.get());
            if (tileSource.hasObject())
                menuItem->addMenu("TileSource", &tileSource);

            SGIHostItemOsg terrainLayer(object.terrainLayer.get());
            if (terrainLayer.hasObject())
            {
                menuItem->addMenu("TerrainLayer", &terrainLayer);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                if (!object.terrainLayer->getCacheID().empty())
                    addCacheActions = true;
#else
                if (object.terrainLayer->getCache())
                    addCacheActions = true;
#endif
            }

            SGIHostItemOsg cacheBin(object.cacheBin.get());
            if (cacheBin.hasObject())
            {
                menuItem->addMenu("CacheBin", &cacheBin);
                addCacheActions = true;
            }
            if(addCacheActions)
            {
                menuItem->addSimpleAction(MenuActionTileSourceTileKeyRemoveFromCache, "Remove from CacheBin", _item);
                menuItem->addSimpleAction(MenuActionTerrainLayerClearCacheTiles, "Clear tiles from CacheBin...", _item);
            }

            SGIHostItemOsg tileData(object.tileData.get());
            if (tileData.hasObject())
                menuItem->addMenu("TileData", &tileData);
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::RTTPicker>::populate(IContextMenuItem * menuItem)
{
    RTTPickerAccess * object = static_cast<RTTPickerAccess*>(getObject<osgEarth::Util::RTTPicker, SGIItemOsg, DynamicCaster>());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
            SGIHostItemOsg group(object->getGroup());
            if (group.hasObject())
                menuItem->addMenu("Group", &group);
            SGIHostItemOsg defaultCallback(object->getDefaultCallback());
            if (defaultCallback.hasObject())
                menuItem->addMenu("DefaultCallback", &defaultCallback);

            RTTPickerAccess::PickContexts contexts;
            object->getPickContexts(contexts);
            unsigned i = 0;
            for(const auto & context : contexts)
            {
                menuItem->addMenu(helpers::str_plus_number("Picker", i), cloneItem<SGIItemOsg>(SGIItemTypePickerContext, i));
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
                    menuItem->addSimpleAction(MenuActionRTTPickerView, "Preview", _item, context._view.get());
                    if (!context._tex.valid())
                    {
                        menuItem->addSimpleAction(MenuActionRTTPickerTexture, "Create texture", _item, context._view.get());
                    }

                    SGIHostItemOsg view(context._view.get());
                    if (view.hasObject())
                        menuItem->addMenu("View", &view);
                    SGIHostItemOsg camera(context._pickCamera.get());
                    if (camera.hasObject())
                        menuItem->addMenu("Camera", &camera);
                    SGIHostItemOsg image(context._image.get());
                    if (image.hasObject())
                        menuItem->addMenu("Image", &image);
                    SGIHostItemOsg tex(context._tex.get());
                    if (tex.hasObject())
                        menuItem->addMenu("Tex", &tex);
                    break;
                }
                ++i;
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<osgEarth::Util::EarthManipulator>::populate(IContextMenuItem * menuItem)
{
    osgEarth::Util::EarthManipulator * object = getObject<osgEarth::Util::EarthManipulator, SGIItemOsg, DynamicCaster>();
    bool ret = false;
    switch (itemType())
    {
    case SGIItemTypeObject:
        ret = callNextHandler(menuItem);
        if (ret)
        {
        }
        break;
    default:
        ret = callNextHandler(menuItem);
        break;
    }
    return ret;
}

} // namespace osgearth_plugin
} // namespace sgi
