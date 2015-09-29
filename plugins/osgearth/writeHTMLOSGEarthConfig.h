#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <typeinfo>

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
        class GeometryCompilerOptions;
    }

} // namespace osgEarth

namespace sgi {

namespace osgearth_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Config)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ConfigOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::DriverConfigOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::MapOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::MapNodeOptions)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ProfileOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TerrainOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TerrainLayerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ImageLayerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ElevationLayerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ModelLayerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::MaskLayerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TileSourceOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ModelSourceOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::CacheOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::LevelDBOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Features::FeatureModelSourceOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Features::GeometryCompilerOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::TMSOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::WMSOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::VPBOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::ArcGISOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::FileSystemCacheOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::SimpleModelOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::FeatureGeomModelOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::FeatureStencilModelOptions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Drivers::OGRFeatureOptions)

/*
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Drivers::TMSOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Drivers::WMSOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Drivers::VPBOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Drivers::ArcGISOptions & opts, bool table);

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::GeoCircle & object, bool table, bool brief=true);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::GeoExtent & object, bool table, bool brief=true);


void writePrettyCallbacksHTML(std::basic_ostream<char>& os, const osgEarth::Util::Controls::Control * object, bool table);

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Util::SkyNode * object, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const ElevationQueryRef * object, bool table);

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Picker::Hit & hit, bool table, bool brief=false);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Picker::Hits & hits, bool table, bool brief=false);

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::DataExtentList & object);
*/

} // namespace osgearth_plugin
} // namespace sgi
