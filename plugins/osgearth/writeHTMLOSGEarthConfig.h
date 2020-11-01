#pragma once

#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <osgEarth/Version>

namespace osgEarth {

    class ConfigOptions;
    class Config;
    class DriverConfigOptions;
#if OSGEARTH_VERSION_GREATER_THAN(3,0,0)
    class Map {
        class Options;
    };
    class MapNode {
        class Options;
    };
#else
    class MapOptions;
    class MapNodeOptions;
#endif
    class ProfileOptions;
    class CacheOptions;
    class TerrainOptions;
    class TerrainLayerOptions;
    class ImageLayerOptions;
    class ElevationLayerOptions;
    class ModelLayerOptions;
    class MaskLayerOptions;
    class TileSourceOptions;
    class ModelSourceOptions;
    namespace Drivers {
        class TMSOptions;
        class WMSOptions;
        class VPBOptions;
        class ArcGISOptions;
        class FileSystemCacheOptions;
        class SimpleModelOptions;
        class FeatureGeomModelOptions;
        class FeatureStencilModelOptions;
        class OGRFeatureOptions;
    }
    namespace Features {
        class FeatureModelSourceOptions;
		class FeatureSourceOptions;
        class GeometryCompilerOptions;
    }

} // namespace osgEarth

namespace sgi {

namespace osgearth_plugin {

} // namespace osgearth_plugin
} // namespace sgi
