#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/plugins/SettingsDialog>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIItemOsg>
#include "SGIItemOsgEarth"
#include <sgi/SGIItemInternal>

#include "writeHTMLOSGEarth.h"
#include "writeHTMLOSGEarthConfig.h"
#include "getObjectInfoOSGEarth.h"
#include "ObjectTreeOSGEarth.h"
#include "ContextMenuOSGEarth.h"
#include "MenuActionOSGEarth.h"
#include "SettingsDialogOSGEarth.h"
#include "GUIAdapterOSGEarth.h"

#include <osgEarth/Version>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#define protected public
#include <osgEarth/ElevationPool>
#undef protected
#endif
#include <osgEarth/Map>
#include <osgEarth/MapNode>

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarth/MaskSource>
#include <osgEarth/MaskLayer>
#endif
#include <osgEarth/ModelLayer>
#include <osgEarth/ScreenSpaceLayout>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarth/LandCoverLayer>
#include <osgEarth/PatchLayer>
#include <osgEarth/VideoLayer>
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
#include <osgEarthFeatures/FeatureModelLayer>
#include <osgEarthFeatures/FeatureModelGraph>
#include <osgEarthFeatures/FeatureMaskLayer>
#include <osgEarthFeatures/ImageToFeatureLayer>
//#include <osgEarthUtil/FlatteningLayer>
#include <osgEarthUtil/FractalElevationLayer>
#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/GARSGraticule>
#include <osgEarthUtil/MGRSGraticule>
#include <osgEarthUtil/UTMGraticule>
#include <osgEarthUtil/MultiElevationLayer>
#include <osgEarthUtil/SimpleOceanLayer>
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/EarthManipulator>
#include <osgEarth/TileSource>
#else
#include <osgEarthUtil/EarthManipulator>
#endif
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/OverlayDecorator>
#include <osgEarth/VirtualProgram>
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarth/ShaderFactory>
#include <osgEarth/ResourceReleaser>
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
#include <osgEarth/LineDrawable>
#endif

#include <osg/PagedLOD>
#include <osg/ProxyNode>

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#include <osgEarth/Sky>
#include <osgEarth/AutoClipPlaneHandler>
#include <osgEarth/Controls>
#include <osgEarth/RTTPicker>

#include <osgEarth/FeatureModelSource>
#include <osgEarth/FeatureModelGraph>
#include <osgEarth/FeatureCursor>

#include <osgEarth/AnnotationNode>

#else // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/SkyNode>
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)

#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/RTTPicker>

#include <osgEarthFeatures/FeatureModelSource>
#include <osgEarthFeatures/FeatureTileSource>

#include <osgEarthSymbology/BillboardSymbol>
#include <osgEarthSymbology/AltitudeSymbol>
#include <osgEarthSymbology/IconSymbol>
#include <osgEarthSymbology/TextSymbol>
#include <osgEarthSymbology/RenderSymbol>

#include <osgEarthSymbology/IconResource>
#include <osgEarthSymbology/BillboardResource>

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
#include <osgEarthAnnotation/Decoration>
#include <osgEarthAnnotation/ScaleDecoration>
#include <osgEarthAnnotation/HighlightDecoration>
#endif
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/ModelNode>
#include <osgEarthAnnotation/TrackNode>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include <osgEarthDrivers/wms/WMSOptions>
#include <osgEarthDrivers/vpb/VPBOptions>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
#include <osgEarthDrivers/model_feature_stencil/FeatureStencilModelOptions>
#endif
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/gdal/GDALOptions>

#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)

#include "geo_helpers.h"

using namespace sgi::osgearth_plugin;

SGI_OBJECT_INFO_BEGIN(sgi::details::Referenced)
    sgi::ISceneGraphDialog
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Referenced)
    osg::Object, 
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Map, 
    osgEarth::Layer,
#endif
    osgEarth::Registry, osgEarth::Capabilities, osgEarth::CacheBin,
    osgEarth::SpatialReference, osgEarth::Profile, osgEarth::Terrain,
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::TileBlacklist,
    osgEarth::Util::Controls::ControlEventHandler,
#endif
    osgEarth::StateSetCache,
    osgEarth::NodeOperation,
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    osgEarth::FeatureCursor, osgEarth::FeatureProfile,
    osgEarth::FeatureDisplayLayout,
#else
    osgEarth::Features::FeatureCursor, osgEarth::Features::FeatureProfile,
    osgEarth::Features::FeatureDisplayLayout,
    osgEarth::Symbology::StyleSheet,
#endif
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Annotation::Decoration,
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    osgEarth::ShaderFactory,
    osgEarth::ElevationPool,
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::ElevationPool::Tile,
    osgEarth::ElevationEnvelope,
#endif
#if OSGEARTH_VERSION_LESS_THAN(3,3,0)
    osgEarth::PolyShader,
#endif
    osgEarth::ScreenSpaceLayoutData,
#endif
    ElevationQueryReferenced,
    TileKeyReferenced,
    TileSourceTileKey,
    TileSourceInfo
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Object)
    osg::Callback,
    osg::StateSet,
    osg::Shader,
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    osgEarth::Map, 
    osgEarth::Layer,
    osgEarth::CacheSettings,
#endif
    osgEarth::Cache,
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    osgEarth::Contrib::TileSource,
#else
    osgEarth::TileSource,
#endif
    osgEarth::ModelSource,
    osgEarth::Extension,
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    osgEarth::FeatureSource,
    osgEarth::Feature,
    osgEarth::Symbol,
#else
    osgEarth::Features::FeatureSource,
    osgEarth::Features::Feature,
    osgEarth::Symbology::Taggable<osg::Object>,
    osgEarth::Symbology::Symbol,
#endif
    osg::Node, osg::StateAttribute, osg::BufferData
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgDB::ReaderWriter)
    osgEarth::Features::FeatureSourceDriver
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

SGI_OBJECT_INFO_BEGIN(osg::StateAttribute)
    osg::Program,
    osgEarth::VirtualProgram
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Node)
    osg::Group, osgEarth::Util::Controls::ControlNode
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
    , osg::Drawable
#endif
SGI_OBJECT_INFO_END()
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
SGI_OBJECT_INFO_BEGIN(osg::Drawable)
    osg::Geometry,
    osgEarth::ResourceReleaser
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Geometry)
    osgEarth::LineDrawable
SGI_OBJECT_INFO_END()
#endif

SGI_OBJECT_INFO_BEGIN(osg::Group)
    osg::CoordinateSystemNode,
    osg::ProxyNode,
    osg::LOD,
    osgEarth::MapNode,
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::TerrainDecorator,
#else
    osgEarth::OverlayDecorator,
#endif
    osgEarth::Util::Controls::Control, osgEarth::Util::Controls::ControlCanvas,
    osgEarth::Util::Controls::ControlNodeBin,
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    osgEarth::FeatureModelGraph,
    osgEarth::AnnotationNode,
    osgEarth::SkyNode,
#else
    osgEarth::Features::FeatureModelGraph,
    osgEarth::Annotation::AnnotationNode,
    osgEarth::Util::SkyNode,
#endif
#ifdef SGI_USE_OSGEARTH_FAST
	osgEarth::LODScaleOverrideNode,
#endif
    osg::Transform
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::CoordinateSystemNode)
    osgEarth::TerrainEngineNode
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::LOD)
    osg::PagedLOD
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Callback)
    osg::NodeCallback
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::NodeCallback)
    osgGA::EventHandler,
    osgEarth::Util::AutoClipPlaneCullCallback
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgGA::EventHandler)
    osgGA::GUIEventHandler
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgGA::GUIEventHandler)
    osgGA::CameraManipulator, 
    osgEarth::Picker
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgGA::CameraManipulator)
    osgEarth::Util::EarthManipulator
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Picker)
    osgEarth::Util::RTTPicker
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::AnnotationNode)
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Annotation::PositionedAnnotationNode,
#else
    osgEarth::Annotation::GeoPositionNode,
#endif
    osgEarth::Annotation::FeatureNode
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::PositionedAnnotationNode)
    osgEarth::Annotation::LocalizedNode,
    osgEarth::Annotation::OrthoNode
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::OrthoNode)
    osgEarth::Annotation::PlaceNode,
    osgEarth::Annotation::LabelNode,
    osgEarth::Annotation::TrackNode
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::LocalizedNode)
    osgEarth::Annotation::CircleNode,
    osgEarth::Annotation::EllipseNode,
    osgEarth::Annotation::RectangleNode,
    osgEarth::Annotation::ModelNode
SGI_OBJECT_INFO_END()

#elif OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::GeoPositionNode)
    osgEarth::Annotation::CircleNode,
    osgEarth::Annotation::EllipseNode,
    osgEarth::Annotation::RectangleNode,
    osgEarth::Annotation::LocalGeometryNode,
    osgEarth::Annotation::ModelNode,
    osgEarth::Annotation::PlaceNode,
    osgEarth::Annotation::LabelNode,
    osgEarth::Annotation::TrackNode
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    SGI_OBJECT_INFO_BEGIN(osgEarth::FeatureCursor)
        osgEarth::FeatureListCursor, osgEarth::GeometryFeatureCursor
    SGI_OBJECT_INFO_END()
#else // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureCursor)
    osgEarth::Features::FeatureListCursor, osgEarth::Features::GeometryFeatureCursor
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)

SGI_OBJECT_INFO_BEGIN(osg::BufferData)
    osg::Image
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Transform)
    osg::Camera
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Camera)
    osgEarth::Util::Controls::ControlCanvas
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Util::Controls::Control)
    osgEarth::Util::Controls::Container, osgEarth::Util::Controls::LabelControl,
    osgEarth::Util::Controls::ImageControl, osgEarth::Util::Controls::CheckBoxControl,
    osgEarth::Util::Controls::HSliderControl
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Util::Controls::Container)
    osgEarth::Util::Controls::HBox,
    osgEarth::Util::Controls::VBox,
    osgEarth::Util::Controls::Grid
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Util::Controls::LabelControl)
    osgEarth::Util::Controls::ButtonControl
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Util::Controls::ImageControl)
    osgEarth::Util::Controls::Frame
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Util::Controls::Frame)
    osgEarth::Util::Controls::RoundedFrame
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainDecorator)
    osgEarth::OverlayDecorator
SGI_OBJECT_INFO_END()
#endif

SGI_OBJECT_INFO_BEGIN(osgEarth::Layer)
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::TerrainLayer, osgEarth::ModelLayer,
#else
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
            osgEarth::Features::FeatureSourceLayer,
    osgEarth::MaskLayer,
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::VisibleLayer
#endif
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::VisibleLayer)
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::Features::FeatureModelLayer,
    osgEarth::Util::SimpleOceanLayer,
    osgEarth::Util::GeodeticGraticule,
    osgEarth::Util::GARSGraticule,
    osgEarth::Util::MGRSGraticule,
    osgEarth::Util::UTMGraticule,
    osgEarth::TerrainLayer,
#else
    osgEarth::TileLayer,
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)
    osgEarth::ModelLayer,
    osgEarth::PatchLayer
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::MaskLayer)
    osgEarth::Features::FeatureMaskLayer
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureSourceLayer)
    osgEarth::Features::ImageToFeatureLayer
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)


#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::TileLayer)
    osgEarth::ImageLayer, osgEarth::ElevationLayer
SGI_OBJECT_INFO_END()
#else
SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainLayer)
    osgEarth::ImageLayer, osgEarth::ElevationLayer
SGI_OBJECT_INFO_END()
#endif

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::ImageLayer)
    osgEarth::LandCoverLayer, osgEarth::VideoLayer
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::ElevationLayer)
    //osgEarth::Util::FlatteningLayer,
    osgEarth::Util::FractalElevationLayer,
    osgEarth::Util::MultiElevationLayer
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

#endif

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::ModelSource)
    osgEarth::Features::FeatureModelSource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureSource)
    osgEarth::Features::FeatureModelSource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TileSource)
    osgEarth::Features::FeatureTileSource
SGI_OBJECT_INFO_END()
#endif

SGI_OBJECT_INFO_BEGIN(osgEarth::Config)
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::ConfigOptions)
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(3,0,0)
    osgEarth::Map::Options,
    osgEarth::MapNode::Options
#else
    osgEarth::MapOptions
    osgEarth::ModelLayerOptions,
    osgEarth::MapNodeOptions, osgEarth::ProfileOptions, osgEarth::DriverConfigOptions,
    osgEarth::TerrainLayerOptions,
    osgEarth::LayerOptions
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    , osgEarth::Features::GeometryCompilerOptions
    , osgEarth::ScreenSpaceLayoutOptions
#endif
#endif
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::LayerOptions)
    osgEarth::VisibleLayerOptions
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::VisibleLayerOptions)
    osgEarth::TerrainLayerOptions
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainLayerOptions)
    osgEarth::ImageLayerOptions, osgEarth::ElevationLayerOptions
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::DriverConfigOptions)
    osgEarth::TileSourceOptions,
    osgEarth::CacheOptions,
    osgEarth::ModelSourceOptions,
    osgEarth::MaskSourceOptions, 
    osgEarth::TerrainOptions,
    osgEarth::Features::FeatureSourceOptions
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    , osgEarth::Features::ScriptEngineOptions
#endif
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TileSourceOptions)
    osgEarth::Drivers::TMSOptions,
    osgEarth::Drivers::WMSOptions,
    osgEarth::Drivers::ArcGISOptions,
    osgEarth::Drivers::VPBOptions,
    osgEarth::Drivers::GDALOptions
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

SGI_OBJECT_INFO_BEGIN(osgEarth::CacheOptions)
    osgEarth::Drivers::FileSystemCacheOptions
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::ModelSourceOptions)
    osgEarth::Drivers::SimpleModelOptions, osgEarth::Features::FeatureModelSourceOptions
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureSourceOptions)
    osgEarth::Drivers::OGRFeatureOptions
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureModelSourceOptions)
    osgEarth::Drivers::FeatureGeomModelOptions
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
    , osgEarth::Drivers::FeatureStencilModelOptions
#endif
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::Decoration)
    osgEarth::Annotation::InjectionDecoration
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::InjectionDecoration)
    osgEarth::Annotation::ScaleDecoration, osgEarth::Annotation::HighlightDecoration
SGI_OBJECT_INFO_END()
#endif

#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::Style)
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::StyleSelector)
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::Taggable<osg::Object>)
    osgEarth::Symbology::Resource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::Resource)
    osgEarth::Symbology::InstanceResource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::InstanceResource)
    osgEarth::Symbology::IconResource,
    osgEarth::Symbology::BillboardResource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::Symbol)
    osgEarth::Symbology::InstanceSymbol,
    osgEarth::Symbology::AltitudeSymbol,
    osgEarth::Symbology::TextSymbol,
    osgEarth::Symbology::RenderSymbol,
    osgEarth::Symbology::ExtrusionSymbol,
    osgEarth::Symbology::LineSymbol,
    osgEarth::Symbology::PointSymbol,
    osgEarth::Symbology::PolygonSymbol
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Symbology::InstanceSymbol)
    osgEarth::Symbology::IconSymbol,
    osgEarth::Symbology::ModelSymbol,
    osgEarth::Symbology::BillboardSymbol
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(3,0,0)

namespace sgi {
namespace osgearth_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)
GENERATE_IMPL_NO_ACCEPT(osg::Object)
GENERATE_IMPL_NO_ACCEPT(osg::Callback)
GENERATE_IMPL_NO_ACCEPT(osg::NodeCallback)
GENERATE_IMPL_NO_ACCEPT(osgGA::EventHandler)
GENERATE_IMPL_NO_ACCEPT(osgGA::GUIEventHandler)
GENERATE_IMPL_NO_ACCEPT(osgGA::CameraManipulator)
//GENERATE_IMPL_NO_ACCEPT(osg::Node)
GENERATE_IMPL_NO_ACCEPT(osg::StateAttribute)
GENERATE_IMPL_NO_ACCEPT(osg::Group)
GENERATE_IMPL_NO_ACCEPT(osg::LOD)
//GENERATE_IMPL_NO_ACCEPT(osg::PagedLOD)
GENERATE_IMPL_NO_ACCEPT(osg::CoordinateSystemNode)
GENERATE_IMPL_NO_ACCEPT(osg::Transform)
GENERATE_IMPL_NO_ACCEPT(osg::MatrixTransform)
//GENERATE_IMPL_NO_ACCEPT(osg::Camera)
GENERATE_IMPL_NO_ACCEPT(osg::BufferData)
GENERATE_IMPL_NO_ACCEPT(osg::Image)

GENERATE_IMPL_NO_ACCEPT(osgDB::ReaderWriter)


typedef generateItemImplT<generateItemAcceptImpl,
                          SGIItemOsg,
                          SGIItemEarthConfig,
                          SGIItemEarthConfigOptions,
#if OSGEARTH_VERSION_LESS_THAN(3,0,0)
                          SGIItemEarthStyle,
                          SGIItemEarthStyleSelector,
#endif
                          SGIItemInternal>
        generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectNameImpl,
                                        getObjectTypeImpl,
                                        defaultPluginGetObjectPathImpl,
                                        getObjectSuggestedFilenameExtensionImpl,
                                        getObjectSuggestedFilenameImpl,
                                        getObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        writeObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl,
                                        defaultPluginGuiAdapterParentWidgetImpl,
                                        guiAdapterSetViewImpl
                                        >
    SGIPluginImpl;

} // namespace osgearth_plugin

class SGIPlugin_osgearth_Implementation : public osgearth_plugin::SGIPluginImpl
{
public:
    SGIPlugin_osgearth_Implementation(SGIPluginHostInterface * hostInterface=nullptr)
        : osgearth_plugin::SGIPluginImpl(hostInterface)
    {
        SGIITEMTYPE_NAME(SGIItemTypeLayers);
        SGIITEMTYPE_NAME(SGIItemTypeImageLayers);
        SGIITEMTYPE_NAME(SGIItemTypeElevationLayers);
        SGIITEMTYPE_NAME(SGIItemTypeModelLayers);
        SGIITEMTYPE_NAME(SGIItemTypeMaskLayers);
        SGIITEMTYPE_NAME(SGIItemTypeImageLayer);
        SGIITEMTYPE_NAME(SGIItemTypeElevationLayer);
        SGIITEMTYPE_NAME(SGIItemTypeModelLayer);
        SGIITEMTYPE_NAME(SGIItemTypeMaskLayer);
        SGIITEMTYPE_NAME(SGIItemTypeMap);
        SGIITEMTYPE_NAME(SGIItemTypeMapNode);
        SGIITEMTYPE_NAME(SGIItemTypeVirtualProgramShaderMap);
        SGIITEMTYPE_NAME(SGIItemTypeVirtualProgramFunctions);
        SGIITEMTYPE_NAME(SGIItemTypeVirtualProgramEffectiveProgram);
        SGIITEMTYPE_NAME(SGIItemTypeOptions);
        SGIITEMTYPE_NAME(SGIItemTypeConfig);
        SGIITEMTYPE_NAME(SGIItemTypeDataExtents);
        SGIITEMTYPE_NAME(SGIItemTypeTileCache);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheLRU);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheMap);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerShaderMap);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerProgramFunctions);
        SGIITEMTYPE_NAME(SGIItemTypeBlacklist);
        SGIITEMTYPE_NAME(SGIItemTypeExtensions);
        SGIITEMTYPE_NAME(SGIItemTypeCullData);
        SGIITEMTYPE_NAME(SGIItemTypeDatabases);
        SGIITEMTYPE_NAME(SGIItemTypeChangeset);
        SGIITEMTYPE_NAME(SGIItemTypeInfo);
        SGIITEMTYPE_NAME(SGIItemTypePreMergeOps);
        SGIITEMTYPE_NAME(SGIItemTypePostMergeOps);
        SGIITEMTYPE_NAME(SGIItemTypeProfiles);
        SGIITEMTYPE_NAME(SGIItemTypeProgramSharedRepo);
        SGIITEMTYPE_NAME(SGIItemTypePickerContext);
        SGIITEMTYPE_NAME(SGIItemTypeGeoHeightfield);
        SGIITEMTYPE_NAME(SGIItemTypeFeatureSourceFeatures);
        SGIITEMTYPE_NAME(SGIItemTypeStyle);
        SGIITEMTYPE_NAME(SGIItemTypeSelectors);
        SGIITEMTYPE_NAME(SGIItemTypeResourceLibraries);
    }
    SGIPlugin_osgearth_Implementation(const SGIPlugin_osgearth_Implementation & rhs)
        : osgearth_plugin::SGIPluginImpl(rhs)
    {
    }

    SGI_Object(sgi_osgearth, SGIPlugin_osgearth_Implementation)
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(osgearth)
