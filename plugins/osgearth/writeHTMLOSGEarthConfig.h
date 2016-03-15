#pragma once

#include <sgi/plugins/SGIHostItemOsgEarth.h>

namespace osgEarth {

    class ConfigOptions;
    class Config;
    class DriverConfigOptions;
    class MapOptions;
    class MapNodeOptions;
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
	class LevelDBOptions;
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
