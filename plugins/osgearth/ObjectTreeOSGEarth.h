#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

namespace osgEarth {
    class Map;
    class MapNode;
    class Registry;
    class StateSetCache;
    class Layer;
    class TerrainLayer;
    class ImageLayer;
    class ElevationLayer;
    class ModelLayer;
    class MaskLayer;
    class Terrain;
	class TerrainEngineNode;
    class TileSource;
    class TileBlacklist;
    class TileKey;
    class ModelSource;
    class MaskSource;
    class ElevationQuery;
    class VirtualProgram;
    class Cache;
    class CacheBin;

    class Config;
    class ConfigOptions;
    class ModelLayerOptions;
	class LevelDBDatabase;

    namespace Util {
        class SkyNode;
        namespace Controls {
            struct ControlContext;
            class ControlCanvas;
            class Control;
            class ControlNodeBin;
            class ControlNode;
            class Container;
            class LabelControl;
            class ImageControl;

        } // namespace Controls
    }
    namespace Features {
        class FeatureModelSource;
        class FeatureModelSourceOptions;
    }
    namespace Drivers {
        class FeatureGeomModelOptions;
    }
}

namespace sgi {
namespace osgearth_plugin {

typedef ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::MapNode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Map)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Registry)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::StateSetCache)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::SkyNode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::Control)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::Container)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::ControlCanvas)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::ControlNodeBin)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::ControlNode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::LabelControl)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Util::Controls::ImageControl)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Layer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::TerrainLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ImageLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ElevationLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ModelLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::MaskLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Terrain)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::TerrainEngineNode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::TileSource)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::TileBlacklist)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ModelSource)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::MaskSource)
OBJECT_TREE_BUILD_IMPL_DECLARE(ElevationQueryReferenced)
OBJECT_TREE_BUILD_IMPL_DECLARE(TileKeyReferenced)
OBJECT_TREE_BUILD_IMPL_DECLARE(TileSourceTileKey)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::VirtualProgram)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Cache)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::CacheBin)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::LevelDBDatabase)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Features::FeatureModelSource)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Config)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ConfigOptions)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::ModelLayerOptions)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Features::FeatureModelSourceOptions)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgEarth::Drivers::FeatureGeomModelOptions)

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(ISceneGraphDialog)

} // namespace osgearth_plugin
} // namespace sgi
