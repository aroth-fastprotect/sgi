#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/GenerateItemImpl>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SGIHostItemOsgEarth.h>
#include <sgi/plugins/SettingsDialog>
#include "../osg/SGIItemOsg"
#include "SGIItemOsgEarth"

#include "writeHTMLOSGEarth.h"
#include "writeHTMLOSGEarthConfig.h"
#include "getObjectInfoOSGEarth.h"
#include "ObjectTreeOSGEarth.h"
#include "ContextMenuOSGEarth.h"
#include "MenuActionOSGEarth.h"
#include "SettingsDialogOSGEarth.h"

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/OverlayDecorator>
#include <osgEarth/VirtualProgram>
#include <osgEarth/LevelDBFactory>

#include <osgEarth/Version>

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,6,0)
#include <osgEarthUtil/Sky>
#else
#include <osgEarthUtil/SkyNode>
#endif
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>

#include <osgEarthFeatures/FeatureModelSource>
#include <osgEarthFeatures/FeatureTileSource>
#include <osgEarthFeatures/GeometryCompiler>

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
#include <osgEarthDrivers/model_feature_stencil/FeatureStencilModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/gdal/GDALOptions>

#include "ElevationQueryReferenced"
#include "geo_helpers.h"

using namespace sgi::osgearth_plugin;

SGI_OBJECT_INFO_BEGIN(osg::Referenced)
    osg::Object, osgEarth::Map, osgEarth::Registry, osgEarth::Capabilities, osgEarth::CacheBin,
    osgEarth::Layer, osgEarth::SpatialReference, osgEarth::Profile, osgEarth::Terrain,
    osgEarth::TileBlacklist, osgEarth::Util::Controls::ControlEventHandler,
    osgEarth::StateSetCache,
    osgEarth::LevelDBDatabase,
    osgEarth::NodeOperation,
    osgEarth::Features::FeatureCursor, osgEarth::Features::FeatureProfile,
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Annotation::Decoration,
#endif
    ElevationQueryReferenced,
    TileKeyReferenced,
    TileSourceTileKey,
    TileSourceInfo
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Object)
    osgEarth::Cache, osgEarth::TileSource,
    osgEarth::ModelSource,
    osgEarth::Extension,
    osgEarth::Features::FeatureSource,
    osg::Node, osg::NodeCallback, osg::StateAttribute, osg::BufferData
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgDB::ReaderWriter)
    osgEarth::Features::FeatureSourceDriver
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::StateAttribute)
    osgEarth::VirtualProgram
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Node)
    osg::Group, osgEarth::Util::Controls::ControlNode
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Group)
    osg::CoordinateSystemNode,
    osgEarth::MapNode, osgEarth::TerrainDecorator, osgEarth::Util::SkyNode,
    osgEarth::Util::Controls::Control, osgEarth::Util::Controls::ControlCanvas,
    osgEarth::Util::Controls::ControlNodeBin,
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Annotation::AnnotationNode,
#endif
	osgEarth::LODScaleOverrideNode,
    osg::Transform
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::CoordinateSystemNode)
    osgEarth::TerrainEngineNode
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::AnnotationNode)
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
    osgEarth::Annotation::PositionedAnnotationNode,
#else
    osgEarth::Annotation::GeoPositionNode,
#endif
    osgEarth::Annotation::FeatureNode
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::PositionedAnnotationNode)
    osgEarth::Annotation::LocalizedNode,
    osgEarth::Annotation::OrthoNode
SGI_OBJECT_INFO_END()
#else // OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::GeoPositionNode)
    osgEarth::Annotation::CircleNode,
    osgEarth::Annotation::EllipseNode,
    osgEarth::Annotation::RectangleNode,
    osgEarth::Annotation::ModelNode
SGI_OBJECT_INFO_END()
#endif // OSGEARTH_VERSION_LESS_THAN(2,9,0)

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

SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureCursor)
    osgEarth::Features::FeatureListCursor, osgEarth::Features::GeometryFeatureCursor
SGI_OBJECT_INFO_END()


SGI_OBJECT_INFO_BEGIN(osg::BufferData)
    osg::Image
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::Transform)
    osg::Camera
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osg::Camera)
    osgEarth::Util::Controls::ControlCanvas
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osg::NodeCallback)
    osgEarth::Util::AutoClipPlaneCullCallback
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

SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainDecorator)
    osgEarth::OverlayDecorator
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Layer)
    osgEarth::TerrainLayer, osgEarth::ModelLayer, osgEarth::MaskLayer
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainLayer)
    osgEarth::ImageLayer, osgEarth::ElevationLayer
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::ModelSource)
    osgEarth::Features::FeatureModelSource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Features::FeatureSource)
    osgEarth::Features::FeatureModelSource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TileSource)
    osgEarth::Features::FeatureTileSource
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::Config)
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::ConfigOptions)
    osgEarth::TerrainLayerOptions, osgEarth::ModelLayerOptions, osgEarth::MapOptions,
    osgEarth::MapNodeOptions, osgEarth::ProfileOptions, osgEarth::DriverConfigOptions,
    osgEarth::LevelDBOptions,
    osgEarth::Features::GeometryCompilerOptions
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TerrainLayerOptions)
    osgEarth::ImageLayerOptions, osgEarth::ElevationLayerOptions
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::DriverConfigOptions)
    osgEarth::TileSourceOptions,
    osgEarth::CacheOptions,
    osgEarth::ModelSourceOptions,
                                                                               osgEarth::MaskSourceOptions, osgEarth::TerrainOptions,
                                                                               osgEarth::Features::FeatureSourceOptions,
                                                                               osgEarth::Features::ScriptEngineOptions
SGI_OBJECT_INFO_END()

SGI_OBJECT_INFO_BEGIN(osgEarth::TileSourceOptions)
    osgEarth::Drivers::TMSOptions,
    osgEarth::Drivers::WMSOptions,
    osgEarth::Drivers::ArcGISOptions,
    osgEarth::Drivers::VPBOptions,
    osgEarth::Drivers::GDALOptions
SGI_OBJECT_INFO_END()

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
    osgEarth::Drivers::FeatureGeomModelOptions,
    osgEarth::Drivers::FeatureStencilModelOptions
SGI_OBJECT_INFO_END()

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::Decoration)
    osgEarth::Annotation::InjectionDecoration
SGI_OBJECT_INFO_END()
SGI_OBJECT_INFO_BEGIN(osgEarth::Annotation::InjectionDecoration)
    osgEarth::Annotation::ScaleDecoration, osgEarth::Annotation::HighlightDecoration
SGI_OBJECT_INFO_END()
#endif

namespace sgi {
namespace osgearth_plugin {

GENERATE_IMPL_TEMPLATE()
GENERATE_IMPL_NO_ACCEPT(osg::Referenced)
GENERATE_IMPL_NO_ACCEPT(osg::Object)
GENERATE_IMPL_NO_ACCEPT(osg::NodeCallback)
//GENERATE_IMPL_NO_ACCEPT(osg::Node)
GENERATE_IMPL_NO_ACCEPT(osg::StateAttribute)
GENERATE_IMPL_NO_ACCEPT(osg::Group)
GENERATE_IMPL_NO_ACCEPT(osg::LOD)
GENERATE_IMPL_NO_ACCEPT(osg::PagedLOD)
GENERATE_IMPL_NO_ACCEPT(osg::CoordinateSystemNode)
GENERATE_IMPL_NO_ACCEPT(osg::Transform)
GENERATE_IMPL_NO_ACCEPT(osg::MatrixTransform)
GENERATE_IMPL_NO_ACCEPT(osg::Camera)
GENERATE_IMPL_NO_ACCEPT(osg::BufferData)
//GENERATE_IMPL_NO_ACCEPT(osg::Image)

GENERATE_IMPL_NO_ACCEPT(osgDB::ReaderWriter)

class generateSGIItemEarthConfig
{
public:
    typedef SGIItemEarthConfig SGIItemClass;
    typedef osgEarth::Config * ObjectStorageType;
    generateSGIItemEarthConfig()
        : _level(0), _acceptLevel(-1), _itemType(SGIItemTypeInvalid), _accepted(NULL)
    {
    }
    template<typename T>
    void ascend(T * object)
    {
        _level--;
    }
    template<typename T>
    void decend(T * object)
    {
        _level++;
    }
    template<typename T>
    void accept(T * object)
    {
        _accepted = object;
        _acceptLevel = _level;
        _itemType = SGIItemTypeObject;
    }
    template<typename T>
    bool canAccept(T * object)
    {
        return true;
    }
    bool wasAccepted() const
    {
        return (_accepted != NULL);
    }

    SGIItemEarthConfig * getItem()
    {
        if(_accepted != NULL)
            return new SGIItemEarthConfig(_itemType, *_accepted, 0, _acceptLevel);
        else
            return NULL;
    }

private:
    unsigned    _level;
    unsigned    _acceptLevel;
    SGIItemType _itemType;
    ObjectStorageType   _accepted;
};

typedef generateItemImplT<generateItemAcceptImpl, SGIItemOsg, SGIItemEarthConfig, SGIItemEarthConfigOptions> generateItemImpl;

typedef SGIPluginImplementationT<       generateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectNameImpl,
                                        getObjectTypeImpl,
                                        defaultPluginGetObjectPathImpl,
                                        getObjectSuggestedFilenameImpl,
                                        getObjectSuggestedFilenameExtensionImpl,
                                        getObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        writeObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl
                                        >
    SGIPluginImpl;

} // namespace osgearth_plugin

class SGIPlugin_osgearth_Implementation : public osgearth_plugin::SGIPluginImpl
{
public:
    SGIPlugin_osgearth_Implementation(SGIPluginHostInterface * hostInterface=NULL)
        : osgearth_plugin::SGIPluginImpl(hostInterface)
    {
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
        SGIITEMTYPE_NAME(SGIItemTypeOptions);
        SGIITEMTYPE_NAME(SGIItemTypeConfig);
        SGIITEMTYPE_NAME(SGIItemTypeDataExtents);
        SGIITEMTYPE_NAME(SGIItemTypeTileCache);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheLRU);
        SGIITEMTYPE_NAME(SGIItemTypeTileCacheMap);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerShaderMap);
        SGIITEMTYPE_NAME(SGIItemTypeShaderComposerProgramFunctions);
    }
    SGIPlugin_osgearth_Implementation(const SGIPlugin_osgearth_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osgearth_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_osgearth, SGIPlugin_osgearth_Implementation);
};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(osgearth)
