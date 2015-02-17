#include "stdafx.h"
#include "MenuActionOSGEarth.h"
#include "../osg/SGIItemOsg.h"

#include <sgi/SceneGraphDialog>
#include <sgi/ContextMenu>

#include <osgEarth/Version>
#include <osgEarth/MapNode>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/ImageUtils>
#include <osgEarth/TileSource>

#include <osgEarthDrivers/debug/DebugOptions>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif

#include "osgearth_accessor.h"
#include "SettingsDialogOSGEarth.h"

#include "../osg/string_helpers.h"

namespace sgi {

class SGIItemOsg;

namespace osgearth_plugin {

ACTION_HANDLER_IMPL_REGISTER(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_REGISTER(MenuActionNodeRegenerateShaders)
ACTION_HANDLER_IMPL_REGISTER(MenuActionMapCachePolicyUsage)
ACTION_HANDLER_IMPL_REGISTER(MenuActionMapDebugImageLayer)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTileInspector)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTerrainLayerCacheUsage)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTerrainLayerSetURL)
ACTION_HANDLER_IMPL_REGISTER(MenuActionModelLayerSetURL)
ACTION_HANDLER_IMPL_REGISTER(MenuActionMaskLayerSetURL)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTerrainLayerEnable)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTerrainLayerVisible)
ACTION_HANDLER_IMPL_REGISTER(MenuActionModelLayerEnable)
ACTION_HANDLER_IMPL_REGISTER(MenuActionModelLayerVisible)
ACTION_HANDLER_IMPL_REGISTER(MenuActionModelLayerLighting)

ACTION_HANDLER_IMPL_REGISTER(MenuActionTileSourceCreateImage)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTileSourceCreateHeightField)
ACTION_HANDLER_IMPL_REGISTER(MenuActionTileSourceUpdateMetaData)

ACTION_HANDLER_IMPL_REGISTER(MenuActionTileBlacklistClear)

ACTION_HANDLER_IMPL_REGISTER(MenuActionSkyNodeLightSettings)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoClipPlaneCullCallbackSetup)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoClipPlaneCullCallbackMinNearFarRatio)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoClipPlaneCullCallbackHeightThreshold)
ACTION_HANDLER_IMPL_REGISTER(MenuActionAutoClipPlaneCullCallbackClampFarClipPlane)

ACTION_HANDLER_IMPL_REGISTER(MenuActionVirtualProgramMask)
ACTION_HANDLER_IMPL_REGISTER(MenuActionVirtualProgramInherit)

ACTION_HANDLER_IMPL_REGISTER(MenuActionElevationQueryCustom)

ACTION_HANDLER_IMPL_REGISTER(MenuActionControlCanvasDirty)
ACTION_HANDLER_IMPL_REGISTER(MenuActionControlDirty)
ACTION_HANDLER_IMPL_REGISTER(MenuActionControlActive)
ACTION_HANDLER_IMPL_REGISTER(MenuActionControlAbsorbEvents)
ACTION_HANDLER_IMPL_REGISTER(MenuActionControlVisible)

ACTION_HANDLER_IMPL_REGISTER(MenuActionImagePreviewRGBA)

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
    IContextMenuInfo * info = menuInfo();
    if(info)
        info->showSceneGraphDialog(_item->rootBase());
    else
    {
        ISceneGraphDialog * dlg = _hostInterface->showSceneGraphDialog(menuAction()->menu()->parentWidget(), _item->rootBase());
        if(dlg)
            dlg->show();
    }
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

bool actionHandlerImpl<MenuActionTileInspector>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoForMenu(SettingsDialogTileInspector, menu()->parentWidget(), menuInfo());
    ret = _hostInterface->openSettingsDialog(dialog, _item, info);
    if(ret)
    {
        if(dialog.valid())
            dialog->show();
    }
    return ret;
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

bool actionHandlerImpl<MenuActionTileSourceCreateImage>::execute()
{
    osgEarth::TileSource * object = getObject<osgEarth::TileSource,SGIItemOsg,DynamicCaster>();
    return true;
}

bool actionHandlerImpl<MenuActionTileSourceCreateHeightField>::execute()
{
    osgEarth::TileSource * object = getObject<osgEarth::TileSource,SGIItemOsg,DynamicCaster>();
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

bool actionHandlerImpl<MenuActionAutoClipPlaneCullCallbackSetup>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoForMenu(SettingsDialogAutoClipPlaneCullCallback, menu()->parentWidget(), menuInfo());
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

bool actionHandlerImpl<MenuActionElevationQueryCustom>::execute()
{
    ISettingsDialogPtr dialog;
    bool ret;
    ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(SettingsDialogRetrieveElevation, menu()->parentWidget());
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
        ISettingsDialogPtr dialog;
        bool ret;
        ISettingsDialogInfoPtr info = new SettingsDialogInfoBase(2, menu()->parentWidget());
        ret = _hostInterface->openSettingsDialog(dialog, item, info);
        if(ret)
        {
            if(dialog.valid())
                dialog->show();
        }
    }
    return true;
}

} // namespace osgearth_plugin
} // namespace sgi
