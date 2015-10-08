#pragma once

#include <sgi/plugins/ActionHandlerImpl>

namespace osgEarth {
    class Map;
    class MapNode;
    class ImageLayer;
}

namespace sgi {
namespace osgearth_plugin {

enum MenuAction {
    MenuActionObjectInfo,
    MenuActionNotifyLevel,
    MenuActionNodeRegenerateShaders,
    MenuActionMapCachePolicyUsage,
    MenuActionMapDebugImageLayer,
    MenuActionTerrainLayerEnable,
    MenuActionTerrainLayerVisible,
    MenuActionTerrainLayerCacheUsage,
    MenuActionTileInspector,
	MenuActionMapInspector,
	MenuActionAddExtension,
	MenuActionTerrainProfile,
    MenuActionTerrainLayerSetURL,
    MenuActionModelLayerSetURL,
    MenuActionModelLayerEnable,
    MenuActionModelLayerVisible,
    MenuActionModelLayerLighting,
    MenuActionMaskLayerSetURL,
    MenuActionSkyNodeLightSettings,
    MenuActionSkyNodeSetDateTime,
    MenuActionSkyNodeSetSunVisible,
    MenuActionSkyNodeSetStarsVisible,
    MenuActionSkyNodeSetMoonVisible,

    MenuActionAutoClipPlaneCullCallbackSetup,
    MenuActionAutoClipPlaneCullCallbackMinNearFarRatio,
    MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio,
    MenuActionAutoClipPlaneCullCallbackHeightThreshold,
    MenuActionAutoClipPlaneCullCallbackClampFarClipPlane,

    MenuActionTileSourceCreateImage,
    MenuActionTileSourceCreateHeightField,
    MenuActionTileSourceUpdateMetaData,

    MenuActionTileBlacklistClear,

    MenuActionVirtualProgramMask,
    MenuActionVirtualProgramInherit,

    MenuActionElevationQueryCustom,
    MenuActionElevationQueryCameraPosition,

    MenuActionControlCanvasDirty,
    MenuActionControlDirty,
    MenuActionControlActive,
    MenuActionControlAbsorbEvents,
    MenuActionControlVisible,

    MenuActionImagePreviewRGBA,

	MenuActionLevelDBDatabaseRead,
	MenuActionLevelDBDatabaseWrite,
};

enum MapDebugImageLayer {
    MapDebugImageLayerUnknown = 0,
    MapDebugImageLayerNone,
    MapDebugImageLayerNormal,
    MapDebugImageLayerInverted
};

enum VirtualProgramInheritMode {
    VirtualProgramInheritModeUnspecified,
    VirtualProgramInheritModeEnabled,
    VirtualProgramInheritModeDisabled
};

MapDebugImageLayer getDebugImageLayer(const osgEarth::Map * object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer );
MapDebugImageLayer getDebugImageLayer(const osgEarth::MapNode * object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer );

ACTION_HANDLER_IMPL_TEMPLATE()
ACTION_HANDLER_IMPL_DECLARE(MenuActionObjectInfo)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNotifyLevel)
ACTION_HANDLER_IMPL_DECLARE(MenuActionNodeRegenerateShaders)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMapCachePolicyUsage)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMapDebugImageLayer)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMapInspector)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTileInspector)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAddExtension)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTerrainProfile)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTerrainLayerCacheUsage)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTerrainLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE(MenuActionModelLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE(MenuActionMaskLayerSetURL)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTerrainLayerEnable)
ACTION_HANDLER_IMPL_DECLARE(MenuActionTerrainLayerVisible)
ACTION_HANDLER_IMPL_DECLARE(MenuActionModelLayerEnable)
ACTION_HANDLER_IMPL_DECLARE(MenuActionModelLayerVisible)
ACTION_HANDLER_IMPL_DECLARE(MenuActionModelLayerLighting)

ACTION_HANDLER_IMPL_DECLARE(MenuActionSkyNodeLightSettings)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoClipPlaneCullCallbackSetup)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoClipPlaneCullCallbackMinNearFarRatio)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoClipPlaneCullCallbackMaxNearFarRatio)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoClipPlaneCullCallbackHeightThreshold)
ACTION_HANDLER_IMPL_DECLARE(MenuActionAutoClipPlaneCullCallbackClampFarClipPlane)

ACTION_HANDLER_IMPL_DECLARE(MenuActionTileSourceUpdateMetaData)

ACTION_HANDLER_IMPL_DECLARE(MenuActionTileBlacklistClear)

ACTION_HANDLER_IMPL_DECLARE(MenuActionVirtualProgramMask)
ACTION_HANDLER_IMPL_DECLARE(MenuActionVirtualProgramInherit)

ACTION_HANDLER_IMPL_DECLARE(MenuActionElevationQueryCustom)

ACTION_HANDLER_IMPL_DECLARE(MenuActionControlCanvasDirty)
ACTION_HANDLER_IMPL_DECLARE(MenuActionControlDirty)
ACTION_HANDLER_IMPL_DECLARE(MenuActionControlActive)
ACTION_HANDLER_IMPL_DECLARE(MenuActionControlAbsorbEvents)
ACTION_HANDLER_IMPL_DECLARE(MenuActionControlVisible)

ACTION_HANDLER_IMPL_DECLARE(MenuActionImagePreviewRGBA)

ACTION_HANDLER_IMPL_DECLARE(MenuActionLevelDBDatabaseRead)
ACTION_HANDLER_IMPL_DECLARE(MenuActionLevelDBDatabaseWrite)

} // namespace osgearth_plugin
} // namespace sgi
