#include "stdafx.h"
#include "MenuActionOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>
#include <sgi/plugins/SGIHostItemOsg.h>

#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/ImagePreviewDialog>

#include <osg/BlendFunc>

#include <osgViewer/CompositeViewer>

#include <osgEarth/Version>
#include <osgEarth/MapNode>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/ImageUtils>
#include <osgEarth/MaskLayer>
#include <osgEarth/TileSource>
#include <osgEarth/TerrainEngineNode>

#include <osgEarthDrivers/debug/DebugOptions>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#include <osgEarth/LineDrawable>
#endif

#include <osgEarthFeatures/FeatureModelLayer>

#include "SGIItemOsgEarth"
#include "osgearth_accessor.h"
#include "SettingsDialogOSGEarth.h"
#include "geo_helpers.h"

#include "TileInspectorDialog.h"

#include <sgi/helpers/osg>
#include <sgi/helpers/osg_helper_nodes>
#include <sgi/helpers/string>

#include <cassert>

namespace sgi {

namespace osgearth_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeRegenerateShaders)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeEditShaders)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapCachePolicyUsage)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapDebugImageLayer)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapInspector)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileInspector)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAddExtension)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainProfile)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerCacheUsage)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerClearCacheTiles)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerOpacity)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerMinVisibleRange)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerMaxVisibleRange)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaskLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLayerEnable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLayerReload)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerEnable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerEnable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerLighting)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileSourceUpdateMetaData)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileBlacklistClear)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSkyNodeSetDateTime)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSkyNodeSetSunVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSkyNodeSetStarsVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSkyNodeSetMoonVisible)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoClipPlaneCullCallbackSetup)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoClipPlaneCullCallbackMinNearFarRatio)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoClipPlaneCullCallbackHeightThreshold)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAutoClipPlaneCullCallbackClampFarClipPlane)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionVirtualProgramMask)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionVirtualProgramInherit)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionVirtualProgramLogging)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionVirtualProgramLoggingFile)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionElevationQueryCustom)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionControlCanvasDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionControlDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionControlActive)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionControlAbsorbEvents)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionControlVisible)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileKeyAdd)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLODScaleOverrideNodeLODScale)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionRTTPickerView)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionRTTPickerTexture)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCacheClear)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCacheCompact)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCacheBinClear)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionCacheBinCompact)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileSourceTileKeyRemoveFromCache)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineDrawableSetLineWidth)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineDrawableSetStipplePattern)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineDrawableSetStippleFactor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineDrawableSetColor)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLineDrawableDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLightSettings)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionFeatureModelLayerDirty)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainEngineNodeDirty)

using namespace sgi::osg_helpers;

namespace {
    // This callback installs a control canvas under a view.
    struct ImageDebugLayerUpdateCallback
#if OSG_VERSION_GREATER_THAN(3,3,1)
        : public osg::Callback
#else
        : public osg::NodeCallback
#endif
    {
        ImageDebugLayerUpdateCallback(osgEarth::MapNode * mapNode, osgEarth::ImageLayer * oldImageLayer, osgEarth::ImageLayer * newImageLayer)
            : _oldImageLayer(oldImageLayer)
            , _newImageLayer(newImageLayer)
        {
            _oldCallback = mapNode->getUpdateCallback();
            mapNode->setUpdateCallback( this );
        }

#if OSG_VERSION_GREATER_THAN(3,3,1)
        virtual bool run(osg::Object* object, osg::Object * data)
        {
            osgEarth::MapNode * mapNode = static_cast<osgEarth::MapNode *>(object);

            if(_oldImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                map->removeLayer(_oldImageLayer.get());
#else
                map->removeImageLayer(_oldImageLayer.get());
#endif
            }
            if(_newImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                map->addLayer(_newImageLayer.get());
#else
                map->addImageLayer(_newImageLayer.get());
#endif
            }
            // call the old callback
            if(_oldCallback.valid())
                _oldCallback->run(object, data);
            mapNode->setUpdateCallback( _oldCallback.get() );
            return osg::Callback::run(object, data);
        }
#else
        void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osgEarth::MapNode * mapNode = static_cast<osgEarth::MapNode *>(node);

            if(_oldImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                map->removeLayer(_oldImageLayer.get());
#else
                map->removeImageLayer(_oldImageLayer.get());
#endif
            }
            if(_newImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                map->addLayer(_newImageLayer.get());
#else
                map->addImageLayer(_newImageLayer.get());
#endif
            }
            // call the old callback
            if(_oldCallback.valid())
            {
                (*_oldCallback.get())(node, nv);
            }
            mapNode->setUpdateCallback( _oldCallback.get() );
        }
#endif

        osg::ref_ptr<osgEarth::ImageLayer> _oldImageLayer;
        osg::ref_ptr<osgEarth::ImageLayer> _newImageLayer;
#if OSG_VERSION_GREATER_THAN(3,3,1)
        osg::ref_ptr<osg::Callback> _oldCallback;
#else
        osg::ref_ptr<osg::NodeCallback> _oldCallback;
#endif
    };

}

MapDebugImageLayer getDebugImageLayer(const osgEarth::Map* object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer )
{
    MapDebugImageLayer ret = MapDebugImageLayerNone;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::ImageLayerVector imageLayers;
    object->getImageLayers(imageLayers);
    for(osgEarth::ImageLayerVector::iterator it = imageLayers.begin(); ret == MapDebugImageLayerNone && it != imageLayers.end(); it++)
    {
        osg::ref_ptr<osgEarth::ImageLayer> layer = *it;
        const osgEarth::TerrainLayerOptions & opts = layer->getTerrainLayerRuntimeOptions();
        const osgEarth::TileSourceOptions & tileOpts = opts.driver().value();
        if(tileOpts.getDriver() == "debug")
        {
            osgEarth::Drivers::DebugOptions debugOpts(tileOpts);
            if(debugOpts.invertY().value())
                ret = MapDebugImageLayerInverted;
            else
                ret = MapDebugImageLayerNormal;
            imageLayer = layer;
        }
    }
#else
#endif
    return ret;
}

MapDebugImageLayer getDebugImageLayer(const osgEarth::MapNode * object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer )
{
    osgEarth::Map * map = const_cast<osgEarth::MapNode*>(object)->getMap();
    return getDebugImageLayer(map, imageLayer);
}

bool setDebugImageLayer(osgEarth::Map * object, MapDebugImageLayer mode)
{
    osg::ref_ptr<osgEarth::ImageLayer> layer;
    MapDebugImageLayer current = getDebugImageLayer(object, layer);
    if(mode != current)
    {
        switch(mode)
        {
        case MapDebugImageLayerNone:
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            object->removeLayer(layer.get());
#else
            object->removeImageLayer(layer.get());
#endif
            break;
        case MapDebugImageLayerNormal:
        case MapDebugImageLayerInverted:
            {
                osgEarth::Drivers::DebugOptions tileOpts;
                osgEarth::ImageLayerOptions opts;
                tileOpts.invertY() = (mode == MapDebugImageLayerInverted);
                tileOpts.setDriver("debug");
                opts.driver() = tileOpts;
                osgEarth::ImageLayer * imageLayer = new osgEarth::ImageLayer(opts);

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
                if(layer.valid())
                    object->removeLayer(layer.get());
                object->addLayer(imageLayer);
#else
                if(layer.valid())
                    object->removeImageLayer(layer.get());
                object->addImageLayer(imageLayer);
#endif
            }
            break;
        }
    }
    return true;

}

bool actionHandlerImpl<MenuActionObjectInfo>::execute()
{
    IHostCallback * callback = hostCallback();
    assert(callback);
    callback->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
    return true;
}

bool actionHandlerImpl<MenuActionNotifyLevel>::execute()
{
    osg::NotifySeverity notifyLevel = (osg::NotifySeverity)menuAction()->mode();
    osgEarth::setNotifyLevel(notifyLevel);
    return true;
}

bool actionHandlerImpl<MenuActionNodeEditShaders>::execute()
{
    return openSettingsDialog(SettingsDialogShaderEditor);
}

bool actionHandlerImpl<MenuActionNodeRegenerateShaders>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
    osg::ref_ptr<osgEarth::StateSetCache> cache = new osgEarth::StateSetCache();
    osgEarth::Registry::shaderGenerator().run(object, getObjectName(object), cache.get());
#endif
    return true;
}

bool actionHandlerImpl<MenuActionMapCachePolicyUsage>::execute()
{
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    const osgEarth::MapOptions & mapOptions = object->getMapOptions();
    osgEarth::CachePolicy newCachePolicy = mapOptions.cachePolicy().value();
    newCachePolicy.usage() = (osgEarth::CachePolicy::Usage)menuAction()->mode();
    /// @todo apply new default cache policy
    //object->setCachePolicy(newCachePolicy);
    return false;
}

bool actionHandlerImpl<MenuActionMapDebugImageLayer>::execute()
{
    osg::ref_ptr<osgEarth::ImageLayer> layer;
    osgEarth::Map * object = static_cast<osgEarth::Map*>(item<SGIItemOsg>()->object());
    return setDebugImageLayer(object, (MapDebugImageLayer)menuAction()->mode());
}

bool actionHandlerImpl<MenuActionTerrainLayerCacheUsage>::execute()
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::CachePolicy newCachePolicy = ((TerrainLayerAccessor*)object)->getCachePolicy();
    newCachePolicy.usage() = (osgEarth::CachePolicy::Usage)menuAction()->mode();
    ((TerrainLayerAccessor*)object)->setCachePolicy(newCachePolicy);
#endif
    return true;
}

bool actionHandlerImpl<MenuActionMapInspector>::execute()
{
    MapNodeAccess * object = static_cast<MapNodeAccess*>(getObject<osgEarth::MapNode,SGIItemOsg>());

    runMethodInUpdateCallback(object, &MapNodeAccess::toggleMapInspector);
	return true;
}

bool actionHandlerImpl<MenuActionAddExtension>::execute()
{
	osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());

	std::string extensionName = "mapinspector";
	bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), extensionName, "Extension:", "Enter name of extension to add", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
	if (gotInput)
	{
		osgEarth::ConfigOptions options;
		osgEarth::Extension * extension = osgEarth::Extension::create(extensionName, options);
		if (extension)
		{
			object->addExtension(extension);
		}
	}
	return true;
}

bool actionHandlerImpl<MenuActionTerrainProfile>::execute()
{
	osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
	MapNodeAccess * access = static_cast<MapNodeAccess*>(object);

	access->toggleTerrainProfile(menu()->parentWidget());
	return true;
}

bool actionHandlerImpl<MenuActionTileInspector>::execute()
{
    return openSettingsDialog(SettingsDialogTileInspector);
}

bool actionHandlerImpl<MenuActionImageLayerOpacity>::execute()
{
	osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());

	double value = object->getOpacity();
	bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Opacity", "Set opacity", 0.0, 1.0, 6, _item);
	if (gotInput)
	{
		object->setOpacity(value);
		triggerRepaint();
	}
	return true;
}

bool actionHandlerImpl<MenuActionImageLayerMinVisibleRange>::execute()
{
	osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());

	double value = object->getMinVisibleRange();
	bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Range", "Set minimum visible range", 0.0, 10000000.0, 1, _item);
	if (gotInput)
	{
		object->setMinVisibleRange(value);
		triggerRepaint();
	}
	return true;
}

bool actionHandlerImpl<MenuActionImageLayerMaxVisibleRange>::execute()
{
	osgEarth::ImageLayer * object = static_cast<osgEarth::ImageLayer*>(item<SGIItemOsg>()->object());

	double value = object->getMaxVisibleRange();
	bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Range", "Set maximum visible range", 0.0, 10000000.0, 1, _item);
	if (gotInput)
	{
		object->setMaxVisibleRange(value);
		triggerRepaint();
	}
	return true;
}

bool actionHandlerImpl<MenuActionTerrainLayerSetURL>::execute()
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    std::string url = ((TerrainLayerAccessor*)object)->getURL();

    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), url, "URL:", "Set URL", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        //const_cast<LayerNode*>(layerNode)->setFilename(newFilename);
        //triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionModelLayerSetURL>::execute()
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    std::string url;
    const osgEarth::ModelSourceOptions & layerOpts = object->getModelSource()->getOptions();
    osgEarth::Config layerConf = layerOpts.getConfig();
    if(layerConf.hasValue("url"))
        url = layerConf.value("url");

    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), url, "URL:", "Set URL", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        //const_cast<LayerNode*>(layerNode)->setFilename(newFilename);
        //triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionMaskLayerSetURL>::execute()
{
    osgEarth::MaskLayer * object = static_cast<osgEarth::MaskLayer*>(item<SGIItemOsg>()->object());
    std::string url;
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    const osgEarth::MaskSourceOptions & layerOpts = object->getMaskSource()->getOptions();
    osgEarth::Config layerConf = layerOpts.getConfig();
    if(layerConf.hasValue("url"))
        url = layerConf.value("url");
#endif
    bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), url, "URL:", "Set URL", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if(gotInput)
    {
        //const_cast<LayerNode*>(layerNode)->setFilename(newFilename);
        //triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionTerrainLayerEnable>::execute()
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    ((TerrainLayerAccessor*)object)->setEnabled(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTerrainLayerVisible>::execute()
{
    osgEarth::TerrainLayer * object = static_cast<osgEarth::TerrainLayer*>(item<SGIItemOsg>()->object());
    object->setVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionLayerEnable>::execute()
{
	osgEarth::Layer* object = getObject<osgEarth::Layer, SGIItemOsg>();
	object->setEnabled(menuAction()->state());
	return true;
}

bool actionHandlerImpl<MenuActionLayerReload>::execute()
{
	osgEarth::Layer* object = getObject<osgEarth::Layer, SGIItemOsg>();
    class LayerReload : public osgEarth::Layer {
    public:
        void reload() {
            fireCallback(&osgEarth::LayerCallback::onEnabledChanged);
        }
    };
    static_cast<LayerReload*>(object)->reload();
	return true;
}

bool actionHandlerImpl<MenuActionModelLayerEnable>::execute()
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    ((ModelLayerAccessor*)object)->setEnabled(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionModelLayerVisible>::execute()
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    object->setVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionModelLayerLighting>::execute()
{
    osgEarth::ModelLayer * object = static_cast<osgEarth::ModelLayer*>(item<SGIItemOsg>()->object());
    object->setLightingEnabled(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionTileSourceUpdateMetaData>::execute()
{
    osgEarth::TileSource * object = getObject<osgEarth::TileSource,SGIItemOsg,DynamicCaster>();
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
    osgEarth::Config config;
    object->readMetaData(config);
    object->writeMetaData(config);
#endif
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)
    return true;
}

bool actionHandlerImpl<MenuActionTileBlacklistClear>::execute()
{
    osgEarth::TileBlacklist * tileblacklist = getObject<osgEarth::TileBlacklist,SGIItemOsg,DynamicCaster>();
    osgEarth::TerrainLayer * terrainlayer = getObject<osgEarth::TerrainLayer, SGIItemOsg, DynamicCaster>();
    osgEarth::TileSource * tilesource = getObject<osgEarth::TileSource, SGIItemOsg, DynamicCaster>();
    osgEarth::Map * map = getObject<osgEarth::Map, SGIItemOsg, DynamicCaster>();
    osgEarth::MapNode * mapnode = getObject<osgEarth::MapNode, SGIItemOsg, DynamicCaster>();
    if (terrainlayer)
        tilesource = terrainlayer->getTileSource();
    if (tilesource)
        tileblacklist = tilesource->getBlacklist();
    if (tileblacklist)
        tileblacklist->clear();
    if (mapnode || map)
    {
        if(!map && mapnode)
            map = mapnode->getMap();
        Q_ASSERT(map != NULL);
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        osgEarth::MapFrame frame(map);
        for (osgEarth::ElevationLayer * elevationLayer : frame.elevationLayers())
        {
            tilesource = elevationLayer->getTileSource();
            if (tilesource)
                tileblacklist = tilesource->getBlacklist();
            if (tileblacklist)
                tileblacklist->clear();
        }
        for (osgEarth::ImageLayer * imageLayer : frame.imageLayers())
        {
            tilesource = imageLayer->getTileSource();
            if (tilesource)
                tileblacklist = tilesource->getBlacklist();
            if (tileblacklist)
                tileblacklist->clear();
        }
#else
        osgEarth::LayerVector layers;
        map->getLayers(layers);
        for (osgEarth::Layer * layer : layers)
        {
            osgEarth::TerrainLayer* terrainLayer = dynamic_cast<osgEarth::TerrainLayer*>(layer);
            if (terrainLayer)
            {
                tilesource = terrainLayer->getTileSource();
                if (tilesource)
                    tileblacklist = tilesource->getBlacklist();
                if (tileblacklist)
                    tileblacklist->clear();
            }
        }
#endif
    }
    return true;
}

bool actionHandlerImpl<MenuActionSkyNodeSetDateTime>::execute()
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode, SGIItemOsg>();
    /// @todo open settings dialog for sky/light
    //object->clear();
    return true;
}

bool actionHandlerImpl<MenuActionSkyNodeSetSunVisible>::execute()
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode, SGIItemOsg>();
    object->setSunVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionSkyNodeSetStarsVisible>::execute()
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode, SGIItemOsg>();
    object->setStarsVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionSkyNodeSetMoonVisible>::execute()
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode, SGIItemOsg>();
    object->setMoonVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackSetup>::execute()
{
    return openSettingsDialog(SettingsDialogAutoClipPlaneCullCallback);
}

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackMinNearFarRatio>::execute()
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback, SGIItemOsg,DynamicCaster>();

    double value = object->getMinNearFarRatio();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set mininum near/far ratio", 0.0, 1.0, 6, _item);
    if(gotInput)
    {
        object->setMinNearFarRatio(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio>::execute()
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback, SGIItemOsg,DynamicCaster>();

    double value = object->getMaxNearFarRatio();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set maxinum near/far ratio", 0.0, 1.0, 6, _item);
    if(gotInput)
    {
        object->setMaxNearFarRatio(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackHeightThreshold>::execute()
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback, SGIItemOsg,DynamicCaster>();

    double value = object->getHeightThreshold();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Value", "Set height threshold (above ellipsoid)", 0.0, 10000.0, 1, _item);
    if(gotInput)
    {
        object->setHeightThreshold(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackClampFarClipPlane>::execute()
{
    osgEarth::Util::AutoClipPlaneCullCallback * object = getObject<osgEarth::Util::AutoClipPlaneCullCallback, SGIItemOsg,DynamicCaster>();
    object->setClampFarClipPlane(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionVirtualProgramMask>::execute()
{
    osgEarth::VirtualProgram * object = getObject<osgEarth::VirtualProgram, SGIItemOsg>();
    unsigned value = ((VirtualProgramAccessor*)object)->mask();
    bool gotInput = _hostInterface->inputDialogBitmask(menuAction()->menu()->parentWidget(), value, "Mask", "Set mask for VirtualProgram", _item);
    if(gotInput)
    {
        ((VirtualProgramAccessor*)object)->setMask(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionVirtualProgramInherit>::execute()
{
    osgEarth::VirtualProgram * object = getObject<osgEarth::VirtualProgram, SGIItemOsg>();
    VirtualProgramInheritMode mode = (VirtualProgramInheritMode)menuAction()->mode();
    switch(mode)
    {
    case VirtualProgramInheritModeUnspecified:
        ((VirtualProgramAccessor*)object)->setInheritShadersToUnspecified();
        break;
    case VirtualProgramInheritModeEnabled:
        object->setInheritShaders(true);
        break;
    case VirtualProgramInheritModeDisabled:
        object->setInheritShaders(false);
        break;
    }
    return true;
}

bool actionHandlerImpl<MenuActionVirtualProgramLogging>::execute()
{
    osgEarth::VirtualProgram * object = getObject<osgEarth::VirtualProgram, SGIItemOsg>();
    object->setShaderLogging(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionVirtualProgramLoggingFile>::execute()
{
    osgEarth::VirtualProgram * object = getObject<osgEarth::VirtualProgram, SGIItemOsg>();

    std::string filename = ((VirtualProgramAccessor*)object)->getShaderLogFile();
    std::vector<std::string> filters;

    filters.push_back("Log files (*.txt *.log)");

    if (_hostInterface->inputDialogFilename(menu()->parentWidget(), SGIPluginHostInterface::InputDialogFilenameSave, filename, filters, "Set shader log file", _item))
    {
        object->setShaderLogging(true, filename);;
    }
    return true;
}

bool actionHandlerImpl<MenuActionElevationQueryCustom>::execute()
{
    return openSettingsDialog(SettingsDialogRetrieveElevation);
}

bool actionHandlerImpl<MenuActionControlDirty>::execute()
{
    osgEarth::Util::Controls::Control * object = getObject<osgEarth::Util::Controls::Control, SGIItemOsg>();
    object->dirty();
    return true;
}

bool actionHandlerImpl<MenuActionControlActive>::execute()
{
    osgEarth::Util::Controls::Control * object = getObject<osgEarth::Util::Controls::Control, SGIItemOsg>();
    object->setActive(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionControlAbsorbEvents>::execute()
{
    osgEarth::Util::Controls::Control * object = getObject<osgEarth::Util::Controls::Control, SGIItemOsg>();
    object->setAbsorbEvents(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionControlVisible>::execute()
{
    osgEarth::Util::Controls::Control * object = getObject<osgEarth::Util::Controls::Control, SGIItemOsg>();
    object->setVisible(menuAction()->state());
    return true;
}

bool actionHandlerImpl<MenuActionControlCanvasDirty>::execute()
{
    osgEarth::Util::Controls::ControlCanvas * object = getObject<osgEarth::Util::Controls::ControlCanvas, SGIItemOsg>();
    ControlCanvasAccess* access = (ControlCanvasAccess*)object;
    access->dirty();
    return true;
}

bool actionHandlerImpl<MenuActionTileKeyAdd>::execute()
{
    TileKeyReferenced * tk_data_ref = getObject<TileKeyReferenced, SGIItemOsg, DynamicCaster>();
    TileSourceTileKey * tksrc_data_ref = getObject<TileSourceTileKey, SGIItemOsg, DynamicCaster>();

    osgEarth::TileKey tilekey;
    if(tk_data_ref)
        tilekey = tk_data_ref->data();
    else if(tksrc_data_ref)
        tilekey = tksrc_data_ref->data().tileKey;

    if(!tilekey.valid())
        return true;

    typedef std::list<osgEarth::TileKey> TileKeyList;
    TileKeyList tilekeylist;
    switch(menuAction()->mode())
    {
    default:
    case TileKeyAddModeUnknown:
        break;
    case TileKeyAddModeParent:
        tilekeylist.push_back(tilekey.createParentKey());
        break;
    case TileKeyAddModeChildren:
        for(unsigned q = 0; q < 4; ++q)
            tilekeylist.push_back(tilekey.createChildKey(q));
        break;
    case TileKeyAddModeNeighborNorth:
        tilekeylist.push_back(tilekey.createNeighborKey(0, -1));
        break;
    case TileKeyAddModeNeighborSouth:
        tilekeylist.push_back(tilekey.createNeighborKey(0, +1));
        break;
    case TileKeyAddModeNeighborWest:
        tilekeylist.push_back(tilekey.createNeighborKey(-1, 0));
        break;
    case TileKeyAddModeNeighborEast:
        tilekeylist.push_back(tilekey.createNeighborKey(+1, 0));
        break;
    case TileKeyAddModeNeighborsAll:
        tilekeylist.push_back(tilekey.createNeighborKey(-1, -1));
        tilekeylist.push_back(tilekey.createNeighborKey( 0, -1));
        tilekeylist.push_back(tilekey.createNeighborKey(+1, -1));

        tilekeylist.push_back(tilekey.createNeighborKey(-1,  0));
        //tilekeylist.push_back(tilekey.createNeighborKey( 0,  0));
        tilekeylist.push_back(tilekey.createNeighborKey(+1,  0));

        tilekeylist.push_back(tilekey.createNeighborKey(-1, +1));
        tilekeylist.push_back(tilekey.createNeighborKey( 0, +1));
        tilekeylist.push_back(tilekey.createNeighborKey(+1, +1));
        break;
    case TileKeyAddModeNeighborsAdjacent:
        tilekeylist.push_back(tilekey.createNeighborKey(0, -1));
        tilekeylist.push_back(tilekey.createNeighborKey(0, +1));
        tilekeylist.push_back(tilekey.createNeighborKey(-1, 0));
        tilekeylist.push_back(tilekey.createNeighborKey(+1, 0));
        break;
    }
    TileInspectorDialog* tileInspector = dynamic_cast<TileInspectorDialog*>(menu()->parentWidget());
    if(tileInspector)
    {
        for(const osgEarth::TileKey & key : tilekeylist)
        {
            if(key.valid())
                tileInspector->addTileKey(key);
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionLODScaleOverrideNodeLODScale>::execute()
{
    return openSettingsDialog(SettingsDialogLODScaleOverride);
}

namespace {

    // Configures a window that lets you see what the RTT camera sees.
    void setupRTTView(osgViewer::View* view, osg::Texture* rttTex)
    {
        view->setCameraManipulator(0L);
        view->getCamera()->setName("osgearth_pick RTT view");
        view->getCamera()->setViewport(0, 0, 256, 256);
        view->getCamera()->setClearColor(osg::Vec4(1, 1, 1, 1));
        view->getCamera()->setProjectionMatrixAsOrtho2D(-.5, .5, -.5, .5);
        view->getCamera()->setViewMatrixAsLookAt(osg::Vec3d(0, -1, 0), osg::Vec3d(0, 0, 0), osg::Vec3d(0, 0, 1));
        view->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);

        osg::Vec3Array* v = new osg::Vec3Array(6);
        (*v)[0].set(-.5, 0, -.5); (*v)[1].set(.5, 0, -.5); (*v)[2].set(.5, 0, .5); (*v)[3].set((*v)[2]); (*v)[4].set(-.5, 0, .5);(*v)[5].set((*v)[0]);

        osg::Vec2Array* t = new osg::Vec2Array(6);
        (*t)[0].set(0, 0); (*t)[1].set(1, 0); (*t)[2].set(1, 1); (*t)[3].set((*t)[2]); (*t)[4].set(0, 1); (*t)[5].set((*t)[0]);

        osg::Geometry* g = new osg::Geometry();
        g->setUseVertexBufferObjects(true);
        g->setUseDisplayList(false);
        g->setVertexArray(v);
        g->setTexCoordArray(0, t);
        g->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6));

        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(g);

        osg::StateSet* stateSet = geode->getOrCreateStateSet();
        stateSet->setDataVariance(osg::Object::DYNAMIC);

        stateSet->setTextureAttributeAndModes(0, rttTex, 1);
        rttTex->setUnRefImageDataAfterApply(false);
        rttTex->setResizeNonPowerOfTwoHint(false);

        stateSet->setMode(GL_LIGHTING, 0);
        stateSet->setMode(GL_CULL_FACE, 0);
        stateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ZERO), 1);

        const char* fs =
            "#version " GLSL_VERSION_STR "\n"
            "void swap(inout vec4 c) { c.rgba = c==vec4(0)? vec4(1) : vec4(vec3((c.r+c.g+c.b+c.a)/4.0),1); }\n";
        osgEarth::Registry::shaderGenerator().run(geode);
        osgEarth::VirtualProgram::getOrCreate(geode->getOrCreateStateSet())->setFunction("swap", fs, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING);

        view->setSceneData(geode);
    }

}

namespace {
    class AddViewCallback : public osgGA::EventHandler
    {
        osg::Camera * _camera;
        osg::ref_ptr<osgViewer::CompositeViewer> _viewer;
        osg::ref_ptr<osgViewer::View> _view;
    public:
        AddViewCallback(osg::Camera * camera, osgViewer::CompositeViewer * viewer, osgViewer::View * view)
            : _camera(camera), _viewer(viewer), _view(view)
        {

        }
        
        void operator()(osg::Node* node, osg::NodeVisitor* nv) override
        {
            if (node == _camera)
            {
                _viewer->addView(_view);
                _view = nullptr;
                _viewer = nullptr;
                _camera->removeEventCallback(this);
            }
        }

    };
}

bool actionHandlerImpl<MenuActionRTTPickerTexture>::execute()
{
    osgEarth::Util::RTTPicker * object = getObject<osgEarth::Util::RTTPicker, SGIItemOsg, DynamicCaster>();
    osgViewer::View * mainview = userData<osgViewer::View>();
    if (!mainview)
        return false;

    object->getOrCreateTexture(mainview);
    return true;
}

bool actionHandlerImpl<MenuActionRTTPickerView>::execute()
{
    osgEarth::Util::RTTPicker * object = getObject<osgEarth::Util::RTTPicker, SGIItemOsg, DynamicCaster>();
    osgViewer::View * mainview = userData<osgViewer::View>();
    if(!mainview)
        return false;

    osgViewer::CompositeViewer * viewer = dynamic_cast<osgViewer::CompositeViewer *>(mainview->getViewerBase());
    if(!viewer)
        return false;

    SGIHostItemOsg txt(object->getOrCreateTexture(mainview));
    if (txt.hasObject())
    {
        IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), &txt);
        if (dialog.valid())
        {
            //dialog->setObject(&txt, nullptr, std::string(), hostCallback());
            dialog->show();
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionCacheClear>::execute()
{
    osgEarth::Cache * object = getObject<osgEarth::Cache, SGIItemOsg>();
    object->clear();
    return true;
}

bool actionHandlerImpl<MenuActionCacheCompact>::execute()
{
    osgEarth::Cache * object = getObject<osgEarth::Cache, SGIItemOsg>();
    object->compact();
    return true;
}

bool actionHandlerImpl<MenuActionCacheBinClear>::execute()
{
    osgEarth::CacheBin * object = getObject<osgEarth::CacheBin, SGIItemOsg>();
    object->clear();
    return true;
}

bool actionHandlerImpl<MenuActionCacheBinCompact>::execute()
{
    osgEarth::CacheBin * object = getObject<osgEarth::CacheBin, SGIItemOsg>();
    object->compact();
    return true;
}

bool actionHandlerImpl<MenuActionTerrainLayerClearCacheTiles>::execute()
{
    TileSourceTileKey * data_ref = getObject<TileSourceTileKey, SGIItemOsg, DynamicCaster>();
    osgEarth::TerrainLayer * terrainLayer = getObject<osgEarth::TerrainLayer,SGIItemOsg, DynamicCaster>();
    const osgEarth::Profile * profile = terrainLayer ? terrainLayer->getProfile() : nullptr;
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    osgEarth::CacheSettings * cs = terrainLayer ? terrainLayer->getCacheSettings() : nullptr;
    osgEarth::CacheBin * bin = cs ? cs->getCacheBin() : nullptr;
#else
    osgEarth::CacheBin * bin = terrainLayer ? terrainLayer->getCacheBin(profile) : nullptr;
#endif

    std::string keys;
    if(terrainLayer)
        keys = "8/12/2\r\n8/12/3";
    else if (data_ref)
    {
        const TileSourceTileKeyData & object = data_ref->data();
        keys = object.tileKey.str();
        profile = object.tileKey.getProfile();

        if (object.cacheBin.valid())
            bin = object.cacheBin.get();
        else if (object.terrainLayer.valid())
        {
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
            cs = object.terrainLayer->getCacheSettings();
            bin = cs ? cs->getCacheBin() : nullptr;
#else
            bin = terrainLayer ? terrainLayer->getCacheBin(profile) : nullptr;
#endif
        }
    }

    bool gotInput = _hostInterface->inputDialogText(menuAction()->menu()->parentWidget(), keys, "Tile keys:", "Enter tile keys to delete from cache", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
    if (gotInput)
    {
        if (bin && profile)
        {
            TileKeyList tilekeylist;
            if (tilekeylist.fromText(keys, profile))
            {
                for (const osgEarth::TileKey & tilekey : tilekeylist)
                {
                    std::string cacheKey = osgEarth::Stringify() << tilekey.str() << "_" << tilekey.getProfile()->getHorizSignature();
                    bin->remove(cacheKey);
                }
            }
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionTileSourceTileKeyRemoveFromCache>::execute()
{
    TileSourceTileKey * data_ref = getObject<TileSourceTileKey, SGIItemOsg>();
    const TileSourceTileKeyData & object = data_ref->data();
    if (object.cacheBin.valid())
    {
        const osgEarth::TileKey & tilekey = object.tileKey;
        std::string cacheKey = osgEarth::Stringify() << tilekey.str() << "_" << tilekey.getProfile()->getHorizSignature();
        object.cacheBin->remove(cacheKey);
    }
    return true;
}

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
bool actionHandlerImpl<MenuActionLineDrawableSetLineWidth>::execute()
{
    osgEarth::LineDrawable * object = getObject<osgEarth::LineDrawable, SGIItemOsg>();
    double value = object->getLineWidth();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Width", "Set line width", 0.0, 10000.0, 1, _item);
    if (gotInput)
    {
        object->setLineWidth(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionLineDrawableSetStipplePattern>::execute()
{
    osgEarth::LineDrawable * object = getObject<osgEarth::LineDrawable, SGIItemOsg>();
    int value = object->getStipplePattern();
    bool gotInput = _hostInterface->inputDialogInteger(menuAction()->menu()->parentWidget(), value, "Pattern", "Set stipple pattern", 0, 0xffff, 1, _item);
    if (gotInput)
    {
        object->setStipplePattern(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionLineDrawableSetStippleFactor>::execute()
{
    osgEarth::LineDrawable * object = getObject<osgEarth::LineDrawable, SGIItemOsg>();
    double value = object->getStippleFactor();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "Factor", "Set stipple factor", 0.0, 1000.0, 1, _item);
    if (gotInput)
    {
        object->setStippleFactor(value);
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionLineDrawableSetColor>::execute()
{
    osgEarth::LineDrawable * object = getObject<osgEarth::LineDrawable, SGIItemOsg>();
    sgi::Color color = osgColor(object->getColor());
    if (_hostInterface->inputDialogColor(menu()->parentWidget(), color, "Color", "Set line color", _item))
    {
        object->setColor(osgColor(color));
        triggerRepaint();
    }
    return true;
}

bool actionHandlerImpl<MenuActionLineDrawableDirty>::execute()
{
	osgEarth::LineDrawable* object = getObject<osgEarth::LineDrawable, SGIItemOsg>();
    runMethodInUpdateCallback(object, &osgEarth::LineDrawable::dirty);
	return true;
}

#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)


bool actionHandlerImpl<MenuActionTerrainEngineNodeDirty>::execute()
{
	osgEarth::TerrainEngineNode* object = getObject<osgEarth::TerrainEngineNode, SGIItemOsg>();
    object->dirtyTerrain();
	return true;
}




bool actionHandlerImpl<MenuActionLightSettings>::execute()
{
    return openSettingsDialog(SettingsDialogLightSettings);
}


bool actionHandlerImpl<MenuActionFeatureModelLayerDirty>::execute()
{
    osgEarth::Features::FeatureModelLayer * object = getObject<osgEarth::Features::FeatureModelLayer, SGIItemOsg>();
    object->dirty();
    return true;
}

} // namespace osgearth_plugin
} // namespace sgi
