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
	MenuActionImageLayerOpacity,
	MenuActionImageLayerMinVisibleRange,
	MenuActionImageLayerMaxVisibleRange,
    MenuActionTileInspector,
	MenuActionMapInspector,
	MenuActionAddExtension,
	MenuActionTerrainProfile,
    MenuActionTerrainLayerSetURL,
    MenuActionTerrainLayerClearCacheTiles,
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
    MenuActionVirtualProgramLogging,
    MenuActionVirtualProgramLoggingFile,

    MenuActionElevationQueryCustom,
    MenuActionElevationQueryCameraPosition,

    MenuActionControlCanvasDirty,
    MenuActionControlDirty,
    MenuActionControlActive,
    MenuActionControlAbsorbEvents,
    MenuActionControlVisible,

	MenuActionLevelDBDatabaseRead,
	MenuActionLevelDBDatabaseWrite,

    MenuActionTileKeyAdd,

    MenuActionLODScaleOverrideNodeLODScale,

    MenuActionRTTPickerView,
    MenuActionRTTPickerTexture,

    MenuActionCacheClear,
    MenuActionCacheCompact,
    MenuActionCacheBinClear,
    MenuActionCacheBinCompact,
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

enum TileKeyAddMode {
    TileKeyAddModeUnknown = 0,
    TileKeyAddModeParent,
    TileKeyAddModeChildren,
    TileKeyAddModeNeighborNorth,
    TileKeyAddModeNeighborSouth,
    TileKeyAddModeNeighborWest,
    TileKeyAddModeNeighborEast,
};

MapDebugImageLayer getDebugImageLayer(const osgEarth::Map * object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer );
MapDebugImageLayer getDebugImageLayer(const osgEarth::MapNode * object, osg::ref_ptr<osgEarth::ImageLayer> & imageLayer );

ACTION_HANDLER_IMPL_TEMPLATE()

} // namespace osgearth_plugin
} // namespace sgi
