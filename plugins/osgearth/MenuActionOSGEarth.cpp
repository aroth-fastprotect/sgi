#include "stdafx.h"
#include "MenuActionOSGEarth.h"
#include <sgi/plugins/SGIItemOsg>
#include <sgi/plugins/SGIHostItemOsg.h>

#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/ImagePreviewDialog>

#include <osgEarth/Version>
#include <osgEarth/MapNode>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/ImageUtils>
#include <osgEarth/TileSource>
#include <osgEarth/LevelDBFactory>

#include <osgEarthDrivers/debug/DebugOptions>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#include <osgEarthQt/TerrainProfileWidget>

#include "osgearth_accessor.h"
#include "SettingsDialogOSGEarth.h"
#include "ElevationQueryReferenced"

#include "TileInspectorDialog.h"

#include <sgi/helpers/osg>
#include <sgi/helpers/string>

#include <cassert>

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionNodeRegenerateShaders)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapCachePolicyUsage)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapDebugImageLayer)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMapInspector)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileInspector)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionAddExtension)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainProfile)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerCacheUsage)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerOpacity)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerMinVisibleRange)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImageLayerMaxVisibleRange)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionMaskLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerEnable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTerrainLayerVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerEnable)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerVisible)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionModelLayerLighting)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileSourceUpdateMetaData)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileBlacklistClear)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionSkyNodeLightSettings)
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

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionImagePreviewRGBA)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLevelDBDatabaseRead)
ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLevelDBDatabaseWrite)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionTileKeyAdd)

ACTION_HANDLER_IMPL_DECLARE_AND_REGISTER(MenuActionLODScaleOverrideNodeLODScale)

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
                map->removeImageLayer(_oldImageLayer.get());
            }
            if(_newImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
                map->addImageLayer(_newImageLayer.get());
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
                map->removeImageLayer(_oldImageLayer.get());
            }
            if(_newImageLayer.valid())
            {
                osgEarth::Map * map = mapNode->getMap();
                map->addImageLayer(_newImageLayer.get());
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
            object->removeImageLayer(layer.get());
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

                if(layer.valid())
                    object->removeImageLayer(layer.get());
                object->addImageLayer(imageLayer);
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

bool actionHandlerImpl<MenuActionNodeRegenerateShaders>::execute()
{
    osg::Node * object = getObject<osg::Node, SGIItemOsg>();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
    osgEarth::Registry::shaderGenerator().run(object, getObjectName(object));
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
    osgEarth::CachePolicy newCachePolicy = ((TerrainLayerAccessor*)object)->getCachePolicy();
    newCachePolicy.usage() = (osgEarth::CachePolicy::Usage)menuAction()->mode();
    ((TerrainLayerAccessor*)object)->setCachePolicy(newCachePolicy);
    return true;
}

bool actionHandlerImpl<MenuActionMapInspector>::execute()
{
	osgEarth::MapNode * object = static_cast<osgEarth::MapNode*>(item<SGIItemOsg>()->object());
	MapNodeAccess * access = static_cast<MapNodeAccess*>(object);

	access->toggleMapInspector();
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
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogTileInspector, menu()->parentWidget(), hostCallback());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
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
    const osgEarth::MaskSourceOptions & layerOpts = object->getMaskSource()->getOptions();
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
#ifdef OSGEARTH_WITH_FAST_MODIFICATIONS
    osgEarth::Config config;
    object->readMetaData(config);
    object->writeMetaData(config);
#endif
    return true;
}

bool actionHandlerImpl<MenuActionTileBlacklistClear>::execute()
{
    osgEarth::TileBlacklist * object = getObject<osgEarth::TileBlacklist,SGIItemOsg,DynamicCaster>();
    object->clear();
    return true;
}

bool actionHandlerImpl<MenuActionSkyNodeLightSettings>::execute()
{
    osgEarth::Util::SkyNode * object = getObject<osgEarth::Util::SkyNode, SGIItemOsg>();
    /// @todo open settings dialog for sky/light
    //object->clear();
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
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogAutoClipPlaneCullCallback, menu()->parentWidget(), hostCallback());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
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
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogRetrieveElevation, menu()->parentWidget(), hostCallback());
    ret = _hostInterface->openSettingsDialog(dialog, _item.get(), info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return true;
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

bool actionHandlerImpl<MenuActionImagePreviewRGBA>::execute()
{
    osg::Image * object = getObject<osg::Image, SGIItemOsg>();

    osg::ref_ptr<osg::Image> rgbaImage = osgEarth::ImageUtils::convertToRGBA8(object);
    SGIHostItemOsg hostItem(rgbaImage.get());

    osg::ref_ptr<SGIItemBase> item;
    if(_hostInterface->generateItem(item, &hostItem))
    {
        IImagePreviewDialogPtr dialog = hostCallback()->showImagePreviewDialog(menu()->parentWidget(), item.get());

        if(dialog.valid())
        {
            dialog->setObject(item.get(), osg_helpers::convertImage(object), std::string(), hostCallback());
            dialog->show();
        }
    }
    return true;
}

bool actionHandlerImpl<MenuActionLevelDBDatabaseRead>::execute()
{
	osgEarth::LevelDBDatabase * object = getObject<osgEarth::LevelDBDatabase, SGIItemOsg>();
	std::string key = "enter key";
	bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), key, "Key", "Enter key to read from database", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
	if (gotInput)
	{
		std::string value;
		if (object->read(key, value))
		{
			_hostInterface->inputDialogText(menuAction()->menu()->parentWidget(), value, "Value", helpers::str_plus_info("Value", key), SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
		}
		else
		{
            std::string msg = helpers::str_plus_info("failed to read", key);
			_hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), msg, "Key", helpers::str_plus_info("Value", key), SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
		}
	}
	return true;
}

bool actionHandlerImpl<MenuActionLevelDBDatabaseWrite>::execute()
{
	osgEarth::LevelDBDatabase * object = getObject<osgEarth::LevelDBDatabase, SGIItemOsg>();
	std::string key = "enter key";
	bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), key, "Key", "Enter key to write to database", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
	if (gotInput)
	{
		std::string value;
		bool gotInput = _hostInterface->inputDialogString(menuAction()->menu()->parentWidget(), value, "Value", "Enter value to write to database", SGIPluginHostInterface::InputDialogStringEncodingSystem, _item);
		if(gotInput)
		{
			object->write(key, value);
		}
	}
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
    osgEarth::LODScaleOverrideNode * object = static_cast<osgEarth::LODScaleOverrideNode*>(item<SGIItemOsg>()->object());

    double value = object->getLODScale();
    bool gotInput = _hostInterface->inputDialogDouble(menuAction()->menu()->parentWidget(), value, "LOD scale", "Set LOD scale", 0.0, 100.0, 1, _item);
    if (gotInput)
    {
        object->setLODScale(value);
        triggerRepaint();
    }

    return true;
}

} // namespace osgearth_plugin
} // namespace sgi
