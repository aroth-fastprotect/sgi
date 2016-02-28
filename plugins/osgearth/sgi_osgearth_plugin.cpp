#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIPluginMacros.h>
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

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(osg::Object, osgEarth::Map, osgEarth::Registry, osgEarth::Capabilities, osgEarth::CacheBin,
                                                                 osgEarth::Layer, osgEarth::SpatialReference, osgEarth::Profile, osgEarth::Terrain,
                                                                 osgEarth::TileBlacklist, osgEarth::Util::Controls::ControlEventHandler,
                                                                 osgEarth::StateSetCache,
                                                                 osgEarth::LevelDBDatabase,
                                                                 osgEarth::NodeOperation,
                                                                 osgEarth::Features::FeatureCursor, osgEarth::Features::FeatureProfile,
                                                                 ElevationQueryReferenced,
                                                                 TileKeyReferenced,
                                                                 TileSourceTileKey,
                                                                 TileSourceInfo
                                                                ))
#else
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(osg::Object, osgEarth::Map, osgEarth::Registry, osgEarth::Capabilities, osgEarth::CacheBin,
                                                                 osgEarth::Layer, osgEarth::SpatialReference, osgEarth::Profile, osgEarth::Terrain,
                                                                 osgEarth::TileBlacklist, osgEarth::Util::Controls::ControlEventHandler,
                                                                 osgEarth::Annotation::Decoration,
                                                                 osgEarth::StateSetCache,
																 osgEarth::LevelDBDatabase,
                                                                 osgEarth::NodeOperation,
																 osgEarth::Features::FeatureCursor, osgEarth::Features::FeatureProfile,
                                                                 ElevationQueryReferenced,
                                                                 TileKeyReferenced,
                                                                 TileSourceTileKey,
                                                                 TileSourceInfo
                                                                ))
#endif

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Object, LOKI_TYPELIST(osgEarth::Cache, osgEarth::TileSource, osgEarth::ModelSource,
															 osgEarth::Extension,
															 osgEarth::Features::FeatureSource,
                                                             osg::Node, osg::NodeCallback, osg::StateAttribute, osg::BufferData
                                                            ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgDB::ReaderWriter, LOKI_TYPELIST(osgEarth::Features::FeatureSourceDriver))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::StateAttribute, LOKI_TYPELIST(osgEarth::VirtualProgram))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Node, LOKI_TYPELIST(osg::Group, osgEarth::Util::Controls::ControlNode))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Group, LOKI_TYPELIST(osg::CoordinateSystemNode,
															osgEarth::MapNode, osgEarth::TerrainDecorator, osgEarth::Util::SkyNode,
                                                            osgEarth::Util::Controls::Control, osgEarth::Util::Controls::ControlCanvas,
                                                            osgEarth::Util::Controls::ControlNodeBin, osg::Transform,
                                                            osgEarth::Annotation::AnnotationNode
                                                           ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::CoordinateSystemNode, LOKI_TYPELIST(osgEarth::TerrainEngineNode))

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::AnnotationNode, LOKI_TYPELIST(osgEarth::Annotation::PositionedAnnotationNode,
                                                                                      osgEarth::Annotation::FeatureNode
                                                                                    ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::PositionedAnnotationNode, LOKI_TYPELIST(osgEarth::Annotation::LocalizedNode,
                                                                                                osgEarth::Annotation::OrthoNode
                                                                                                ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::OrthoNode, LOKI_TYPELIST(osgEarth::Annotation::PlaceNode,
                                                                                 osgEarth::Annotation::LabelNode,
                                                                                 osgEarth::Annotation::TrackNode
                                                                                ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::LocalizedNode, LOKI_TYPELIST(osgEarth::Annotation::CircleNode,
                                                                                     osgEarth::Annotation::EllipseNode,
                                                                                     osgEarth::Annotation::RectangleNode,
                                                                                     osgEarth::Annotation::ModelNode
                                                                                    ))
#else
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::AnnotationNode, LOKI_TYPELIST(osgEarth::Annotation::FeatureNode,
                                                                                      osgEarth::Annotation::GeoPositionNode
                                                                                    ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::GeoPositionNode, LOKI_TYPELIST(osgEarth::Annotation::CircleNode,
                                                                                     osgEarth::Annotation::EllipseNode,
                                                                                     osgEarth::Annotation::RectangleNode,
                                                                                     osgEarth::Annotation::ModelNode
                                                                                    ))
#endif

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Features::FeatureCursor, LOKI_TYPELIST(osgEarth::Features::FeatureListCursor, osgEarth::Features::GeometryFeatureCursor))
	

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::BufferData, LOKI_TYPELIST(osg::Image))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Transform, LOKI_TYPELIST(osg::Camera))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Camera, LOKI_TYPELIST(osgEarth::Util::Controls::ControlCanvas))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::NodeCallback, LOKI_TYPELIST(osgEarth::Util::AutoClipPlaneCullCallback))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Util::Controls::Control, LOKI_TYPELIST(osgEarth::Util::Controls::Container, osgEarth::Util::Controls::LabelControl,
                                                                                   osgEarth::Util::Controls::ImageControl, osgEarth::Util::Controls::CheckBoxControl,
                                                                                   osgEarth::Util::Controls::HSliderControl))


SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Util::Controls::Container, LOKI_TYPELIST(osgEarth::Util::Controls::HBox, osgEarth::Util::Controls::VBox,
                                                                                     osgEarth::Util::Controls::Grid))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Util::Controls::LabelControl, LOKI_TYPELIST(osgEarth::Util::Controls::ButtonControl))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Util::Controls::ImageControl, LOKI_TYPELIST(osgEarth::Util::Controls::Frame))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Util::Controls::Frame, LOKI_TYPELIST(osgEarth::Util::Controls::RoundedFrame))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::TerrainDecorator, LOKI_TYPELIST(osgEarth::OverlayDecorator))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Layer, LOKI_TYPELIST(osgEarth::TerrainLayer, osgEarth::ModelLayer, osgEarth::MaskLayer))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::TerrainLayer, LOKI_TYPELIST(osgEarth::ImageLayer, osgEarth::ElevationLayer))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::ModelSource, LOKI_TYPELIST(osgEarth::Features::FeatureModelSource))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Features::FeatureSource, LOKI_TYPELIST(osgEarth::Features::FeatureModelSource))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::TileSource, LOKI_TYPELIST(osgEarth::Features::FeatureTileSource))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Config, ::Loki::NullType)

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::ConfigOptions, LOKI_TYPELIST(osgEarth::TerrainLayerOptions, osgEarth::ModelLayerOptions, osgEarth::MapOptions,
                                                                         osgEarth::MapNodeOptions, osgEarth::ProfileOptions, osgEarth::DriverConfigOptions,
																		 osgEarth::LevelDBOptions,
                                                                         osgEarth::Features::GeometryCompilerOptions
                                                                        ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::TerrainLayerOptions, LOKI_TYPELIST(osgEarth::ImageLayerOptions, osgEarth::ElevationLayerOptions))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::DriverConfigOptions, LOKI_TYPELIST(osgEarth::TileSourceOptions, osgEarth::CacheOptions, osgEarth::ModelSourceOptions,
                                                                               osgEarth::MaskSourceOptions, osgEarth::TerrainOptions,
                                                                               osgEarth::Features::FeatureSourceOptions,
                                                                               osgEarth::Features::ScriptEngineOptions
                                                                              ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::TileSourceOptions, LOKI_TYPELIST(osgEarth::Drivers::TMSOptions, osgEarth::Drivers::WMSOptions, osgEarth::Drivers::ArcGISOptions,
                                                                             osgEarth::Drivers::VPBOptions, osgEarth::Drivers::GDALOptions))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::CacheOptions, LOKI_TYPELIST(osgEarth::Drivers::FileSystemCacheOptions))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::ModelSourceOptions, LOKI_TYPELIST(osgEarth::Drivers::SimpleModelOptions, osgEarth::Features::FeatureModelSourceOptions))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Features::FeatureSourceOptions, LOKI_TYPELIST(osgEarth::Drivers::OGRFeatureOptions))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Features::FeatureModelSourceOptions, LOKI_TYPELIST(osgEarth::Drivers::FeatureGeomModelOptions,
                                                                                               osgEarth::Drivers::FeatureStencilModelOptions))

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::Decoration, LOKI_TYPELIST(osgEarth::Annotation::InjectionDecoration))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgEarth::Annotation::InjectionDecoration, LOKI_TYPELIST(osgEarth::Annotation::ScaleDecoration, osgEarth::Annotation::HighlightDecoration))
#endif

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

typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemOsg, SGIItemEarthConfig, SGIItemEarthConfigOptions),
                                        call_function_for_object_type,
                                        generateItemImpl,
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
