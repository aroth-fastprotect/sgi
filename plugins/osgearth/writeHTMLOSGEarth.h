#pragma once

#include <sgi/plugins/SGIPluginMacros.h>

namespace osg {
    class Image;
}

namespace osgEarth {

    class Registry;
    class Capabilities;
    class StateSetCache;

    class Cache;
    class CacheBin;
    class Layer;
    class TerrainLayer;
    class ImageLayer;
    class ElevationLayer;
    class ModelLayer;
    class MaskLayer;
    class Map;
    class MapNode;
    class TerrainDecorator;
    class OverlayDecorator;

    class GeoCircle;
    class GeoExtent;

    class SpatialReference;
    class Profile;
    class TileBlacklist;
    class ElevationQuery;
    class TileSource;
    class TileKey;
    class ModelSource;
    class VirtualProgram;

    class ConfigOptions;
    class Config;
    class DriverConfigOptions;
    class MapOptions;
    class MapNodeOptions;
    class ProfileOptions;
    class CacheOptions;
    class TerrainOptions;
    class TerrainLayerOptions;
    class TileSourceOptions;
    namespace Drivers {
        class TMSOptions;
        class WMSOptions;
        class VPBOptions;
        class ArcGISOptions;
    }
    namespace Util {
        class SkyNode;
        class AutoClipPlaneCullCallback;
        namespace Controls {
            struct ControlContext;
            class ControlCanvas;
            class Control;
            class ControlNodeBin;
            class ControlNode;
            class ControlEventHandler;
            class Container;
            class LabelControl;
            class ImageControl;

        } // namespace Controls
    } // namespace Util
    namespace Features {
        class FeatureModelSource;
    }
    namespace Annotation {
        class AnnotationNode;
        class PositionedAnnotationNode;
        class FeatureNode;
        class LocalizedNode;
        class OrthoNode;
        class PlaceNode;
        class LabelNode;
        class TrackNode;
        class CircleNode;
        class EllipseNode;
        class RectangleNode;
        class ModelNode;
    } // namespace Annotation

    class DataExtent;
    typedef std::vector<DataExtent>    DataExtentList;
} // namespace osgEarth

namespace sgi {
namespace osgearth_plugin {

typedef ReferencedRawPtrT<osgEarth::ElevationQuery> ElevationQueryReferenced;
typedef ReferencedDataT<osgEarth::TileKey> TileKeyReferenced;
struct TileSourceTileKeyData;
typedef ReferencedDataT<TileSourceTileKeyData> TileSourceTileKey;

WRITE_PRETTY_HTML_IMPL_TEMPLATE()

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Registry)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Capabilities)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::StateSetCache)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Cache)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::CacheBin)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Layer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TerrainLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ImageLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ElevationLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ModelLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::MaskLayer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Map)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::MapNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TerrainDecorator)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::OverlayDecorator)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::SpatialReference)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Profile)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TileSource)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::ModelSource)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::VirtualProgram)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::TileBlacklist)
WRITE_PRETTY_HTML_IMPL_DECLARE(ElevationQueryReferenced)

WRITE_PRETTY_HTML_IMPL_DECLARE(TileKeyReferenced)
WRITE_PRETTY_HTML_IMPL_DECLARE(TileSourceTileKey)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::ControlCanvas)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::Control)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::ControlEventHandler)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::Container)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::ControlNodeBin)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::ControlNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::LabelControl)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::Controls::ImageControl)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::SkyNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Util::AutoClipPlaneCullCallback)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Features::FeatureModelSource)

WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::AnnotationNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::PositionedAnnotationNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::FeatureNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::LocalizedNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::OrthoNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::PlaceNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::LabelNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::TrackNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::CircleNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::EllipseNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::RectangleNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgEarth::Annotation::ModelNode)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Image)

/*
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::ConfigOptions & opts);

void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Config & cfg, const std::string & parentReferrer, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::Config & cfg, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::ConfigOptions & opts, bool table, bool brief=true);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::DriverConfigOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::MapOptions & options, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::MapNodeOptions & options, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::ProfileOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::CacheOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::TerrainOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::TerrainLayerOptions & opts, bool table);
void writePrettyHTML(std::basic_ostream<char>& os, const osgEarth::TileSourceOptions & opts, bool table);
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
