#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <sgi/plugins/SGIPluginImpl.h>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SettingsDialog>
#include "SGIItemOsg"

#include "writeHTMLOSG.h"
#include "writeHTMLOSGDB.h"
#include "writeHTMLOSGGA.h"
#include "writeHTMLOSGTerrain.h"
#include "writeHTMLOSGText.h"
#include "writeHTMLOSGViewer.h"

#include "getObjectInfoOSG.h"

#include "ObjectTreeOSG.h"

#include "ContextMenuOSG.h"
#include "MenuActionOSG.h"
#include "SettingsDialogOSG.h"

#include "ObjectLoggerOSG.h"
#include "GUIAdapterOSG.h"

// osg headers
#include <osg/Observer>
#include <osg/UserDataContainer>
#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>
#include <osg/ProxyNode>
#include <osg/ClipPlane>
#include <osg/PolygonStipple>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexEnv>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Point>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Stencil>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/OperationThread>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TextureRectangle>
#include <osg/TexGen>
#include <osg/Shape>
#include <osg/ClusterCullingCallback>

// osgDB headers
#include <osgDB/Registry>
#include <osgDB/ImagePager>
#define protected public
#include <osgDB/DatabasePager>
#undef protected

// osgFX headers
#include <osgFX/Outline>

// osgGA headers
#include <osgGA/GUIEventHandler>
#include <osgGA/CameraManipulator>

// osgUtil headers
#include <osgUtil/SceneView>

// osgViewer headers
#include <osgViewer/CompositeViewer>
#include <osgViewer/Renderer>

// osgQt headers
#include <osgQt/GraphicsWindowQt>

// osgTerrain headers
#include <osgTerrain/Terrain>
#include <osgTerrain/TerrainTile>
#include <osgTerrain/Layer>

// osgText headers
#include <osgText/Font>
#include <osgText/Text>
#include <osgText/Text3D>

// osgAnimation headers
#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/AnimationManagerBase>
#include <osgAnimation/AnimationUpdateCallback>

#include "DrawableHelper.h"
#include <sgi/helpers/osg>

#include <sgi/ReferencedPicker>

#ifndef GL_EXT_blend_color
#define GL_CONSTANT_COLOR_EXT             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT   0x8002
#define GL_CONSTANT_ALPHA_EXT             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT   0x8004
#define GL_BLEND_COLOR_EXT                0x8005
#endif

#ifndef GL_EXT_framebuffer_object
#define GL_EXT_framebuffer_object 1
#define GL_FRAMEBUFFER_EXT                     0x8D40
#define GL_RENDERBUFFER_EXT                    0x8D41
// #define GL_STENCIL_INDEX_EXT                   0x8D45  // removed in rev. #114 of the spec
#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49
#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44
#define GL_RENDERBUFFER_RED_SIZE_EXT           0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT         0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT          0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT         0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT         0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT       0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT          0x8CD4
#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT                0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT                0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT                0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT                0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT                0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT                0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT                0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT                0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT                0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT               0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT               0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT               0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT               0x8CED
#define GL_COLOR_ATTACHMENT14_EXT               0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT               0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20
#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD
#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT   0x0506
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT   0x8CD8
#endif

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#endif

#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE GL_TEXTURE_RECTANGLE_NV
#endif


namespace sgi {
namespace osg_plugin {

SGI_CALL_FUNCTION_FOR_OBJECT_TEMPLATE()

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Referenced, LOKI_TYPELIST(osg::Object, osg::ObserverSet, osgDB::Registry,
                                                                 osg::GraphicsContext::Traits, osg::OperationThread, osg::Operation,
                                                                 osg::FrameStamp, osg::State,
                                                                 osg::NodeVisitor::DatabaseRequestHandler,
                                                                 osg::NodeVisitor::ImageRequestHandler,
                                                                 osg::Program::PerContextProgram,
                                                                 osg::DisplaySettings,
                                                                 osg::GLBufferObject,
                                                                 osg::GLBufferObjectSet,
                                                                 osgAnimation::Channel,
                                                                 osgAnimation::Target,
                                                                 osgAnimation::Sampler,
                                                                 osgDB::DatabasePager::DatabaseThread,
																 osgDB::DatabasePager::DatabaseRequest,
																 osgDB::DatabasePager::RequestQueue,
																 osgDB::DatabasePager::ReadQueue,
                                                                 osgDB::ImagePager::ImageThread,
                                                                 osgDB::ObjectWrapper,
                                                                 osgDB::ObjectWrapperManager,
                                                                 osgDB::FileCache,
                                                                 osgDB::BaseSerializer,
                                                                 osgUtil::StateGraph, osgUtil::RenderLeaf,
                                                                 osgViewer::Scene,
                                                                 osgText::Font::FontImplementation,
                                                                 ReferencedSetViewNodeLookAt,
                                                                 sgi::ReferencedPickerBase,
                                                                 sgi::ISceneGraphDialogToolsMenu
                                                                ))

#if OSG_MIN_VERSION_REQUIRED(3,4,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Object, LOKI_TYPELIST(osg::Node, osg::Node, osg::Shape, osg::StateAttribute,
                                                             osg::StateSet, osg::View, osg::GraphicsContext, osg::Shader, osg::UserDataContainer,
                                                             osg::BufferData, osg::BufferObject, osg::View, osg::Uniform, osg::ShaderComposer,
                                                             osg::NodeVisitor, osg::RenderBuffer, osg::ShaderComponent,
                                                             osg::RefMatrixd, osg::RefMatrixf,
                                                             osgAnimation::Animation,
                                                             osgAnimation::AnimationUpdateCallbackBase,
                                                             osgDB::Options, osgDB::ReaderWriter, osgDB::DatabaseRevision, osgDB::DatabaseRevisions, osgDB::FileList,
                                                             osgViewer::ViewerBase, osgViewer::GraphicsWindow,
                                                             osgGA::GUIEventHandler, osgGA::GUIEventAdapter,
                                                             osgText::Font,
                                                             osgUtil::SceneView, osgUtil::RenderBin,
                                                             osgTerrain::Locator, osgTerrain::Layer, osgTerrain::TerrainTechnique))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Callback, LOKI_TYPELIST(osg::NodeCallback, osg::StateAttributeCallback, osg::UniformCallback))
#else // OSG_MIN_VERSION_REQUIRED(3,4,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Object, LOKI_TYPELIST(osg::Node, osg::NodeCallback, osg::Drawable, osg::Shape, osg::StateAttribute,
                                                             osg::StateSet, osg::View, osg::GraphicsContext, osg::Shader, osg::UserDataContainer,
                                                             osg::BufferData, osg::BufferObject, osg::View, osg::Uniform, osg::ShaderComposer,
                                                             osg::NodeVisitor, osg::RenderBuffer, osg::ShaderComponent,
                                                             osg::RefMatrixd, osg::RefMatrixf,
                                                             osgAnimation::Animation,
                                                             osgAnimation::AnimationUpdateCallbackBase,
                                                             osgDB::Options, osgDB::ReaderWriter, osgDB::DatabaseRevision, osgDB::DatabaseRevisions, osgDB::FileList,
                                                             osgViewer::ViewerBase, osgViewer::GraphicsWindow,
                                                             osgGA::GUIEventHandler, osgGA::GUIEventAdapter,
                                                             osgText::Font,
                                                             osgUtil::SceneView, osgUtil::RenderBin,
                                                             osgTerrain::Locator, osgTerrain::Layer, osgTerrain::TerrainTechnique))
#endif // OSG_MIN_VERSION_REQUIRED(3,4,0)

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::NodeVisitor::ImageRequestHandler, LOKI_TYPELIST(osgDB::ImagePager))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::NodeVisitor::DatabaseRequestHandler, LOKI_TYPELIST(osgDB::DatabasePager))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::DrawElements, LOKI_TYPELIST(osg::DrawElementsUByte, osg::DrawElementsUShort, osg::DrawElementsUInt,
                                                                   osg::DrawArrays, osg::DrawArrayLengths))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::PrimitiveSet, LOKI_TYPELIST(osg::DrawElements))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Array, LOKI_TYPELIST(osg::IndexArray))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::BufferData, LOKI_TYPELIST(osg::PrimitiveSet, osg::Image, osg::Array))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::BufferObject, LOKI_TYPELIST(osg::VertexBufferObject, osg::ElementBufferObject,
                                                                   osg::PixelBufferObject, osg::PixelDataBufferObject,
                                                                   osg::UniformBufferObject, osg::AtomicCounterBufferObject
                                                                  ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Drawable, LOKI_TYPELIST(osg::Geometry, osgText::TextBase, osg::ShapeDrawable, RenderInfoDrawable))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::GraphicsContext, LOKI_TYPELIST(osgViewer::GraphicsWindow))
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Node, LOKI_TYPELIST(osg::Group, osg::Drawable))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Group, LOKI_TYPELIST(osg::Geode, osg::Transform, osg::LOD, osg::ProxyNode, osg::CoordinateSystemNode, osgFX::Effect, osgTerrain::TerrainTile))
#else // OSG_MIN_VERSION_REQUIRED(3,4,0)
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Node, LOKI_TYPELIST(osg::Group, osg::Geode))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Group, LOKI_TYPELIST(osg::Transform, osg::LOD, osg::ProxyNode, osg::CoordinateSystemNode, osgFX::Effect, osgTerrain::TerrainTile))
#endif // OSG_MIN_VERSION_REQUIRED(3,4,0)

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::CoordinateSystemNode, LOKI_TYPELIST(osgTerrain::Terrain))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Transform, LOKI_TYPELIST(osg::Camera, osg::MatrixTransform, osg::PositionAttitudeTransform, osg::AutoTransform))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::LOD, LOKI_TYPELIST(osg::PagedLOD))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::StateAttribute, LOKI_TYPELIST(osg::FrameBufferObject, osg::Light, osg::Texture, osg::Viewport, osg::Program,
                                                                     osg::ClipPlane, osg::TexEnv, osg::TexMat, osg::TexGen, osg::Material, osg::ColorMask,
                                                                     osg::LightModel, osg::CullFace, osg::Point, osg::Depth, osg::PolygonStipple,
                                                                     osg::LineStipple, osg::LineWidth, osg::Stencil, osg::PolygonMode, osg::PolygonOffset,
                                                                     osg::BlendFunc, osg::BlendColor, osg::BlendEquation
                                                                    ))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Shape, LOKI_TYPELIST(osg::Sphere, osg::Box, osg::Cone, osg::Cylinder, osg::Capsule, osg::InfinitePlane,
                                                            osg::TriangleMesh, osg::HeightField, osg::CompositeShape))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::TriangleMesh, LOKI_TYPELIST(osg::ConvexHull))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::View, LOKI_TYPELIST(osgViewer::View))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Texture, LOKI_TYPELIST(osg::Texture1D, osg::Texture2D, osg::Texture3D, osg::TextureRectangle))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Operation, LOKI_TYPELIST(osg::GraphicsOperation, osg::BarrierOperation))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::GraphicsOperation, LOKI_TYPELIST(osgViewer::Renderer))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::NodeCallback, LOKI_TYPELIST(osgGA::GUIEventHandler, osg::ClusterCullingCallback,
                                                                   osg::AnimationPathCallback,
                                                                   osgAnimation::AnimationManagerBase,
                                                                   osgAnimation::AnimationUpdateCallback<osg::NodeCallback>,
                                                                   osgAnimation::Skeleton::UpdateSkeleton ))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgAnimation::AnimationUpdateCallback<osg::NodeCallback>, LOKI_TYPELIST(osgAnimation::UpdateMatrixTransform))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgAnimation::UpdateMatrixTransform, LOKI_TYPELIST(osgAnimation::UpdateBone))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::MatrixTransform, LOKI_TYPELIST(osgAnimation::Bone, osgAnimation::Skeleton))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgText::TextBase, LOKI_TYPELIST(osgText::Text, osgText::Text3D))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgFX::Effect, LOKI_TYPELIST(osgFX::Outline))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgGA::GUIEventHandler, LOKI_TYPELIST(osgGA::CameraManipulator))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgTerrain::Layer, LOKI_TYPELIST(osgTerrain::ImageLayer, osgTerrain::ContourLayer,
                                                                   osgTerrain::HeightFieldLayer, osgTerrain::ProxyLayer,
                                                                   osgTerrain::CompositeLayer
                                                                  ))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgUtil::RenderBin, LOKI_TYPELIST(osgUtil::RenderStage))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgViewer::ViewerBase, LOKI_TYPELIST(osgViewer::CompositeViewer))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osgViewer::GraphicsWindow, LOKI_TYPELIST(osgQt::GraphicsWindowQt))

SGI_CALL_FUNCTION_FOR_OBJECT_BASE(sgi::ReferencedPickerBase, LOKI_TYPELIST(sgi::ReferencedPicker, sgi::ReferencedLinePicker))

bool objectInfo_hasCallback(SGIPluginHostInterface * hostInterface, bool & result, const SGIItemBase * item)
{
    bool ret = false;
    do
    {
        ret = hasCallbackImplBaseT<hasCallbackImpl>::call(hostInterface, item, result);
        item = item->nextBase();
    }
    while(item != NULL && !ret);
    return ret;
}

typedef SGIPluginImplementationT< LOKI_TYPELIST(SGIItemOsg),
                                        call_function_for_object_type,
                                        defaultPluginGenerateItemImpl,
                                        writePrettyHTMLImpl,
                                        getObjectNameImpl,
                                        getObjectDisplayNameImpl,
                                        getObjectTypeImpl,
                                        getObjectPathImpl,
                                        getObjectSuggestedFilenameImpl,
                                        getObjectSuggestedFilenameExtensionImpl,
                                        getObjectFilenameFiltersImpl,
                                        defaultPluginGetObjectHasCallbackImpl,
                                        writeObjectFileImpl,
                                        objectTreeBuildImpl,
                                        objectTreeBuildRootImpl,
                                        contextMenuPopulateImpl,
                                        actionHandlerImpl,
                                        settingsDialogCreateImpl,
                                        defaultPluginGuiAdapterParentWidgetImpl,
                                        guiAdapterSetViewImpl,
                                        getOrCreateObjectLoggerImpl
                                        >
    SGIPluginImpl;
} // namespace osg_plugin


#define REGISTER_GLENUM(c) \
    registerNamedEnumValue<osg_helpers::GLEnum>(c, #c)

class SGIPlugin_osg_Implementation : public osg_plugin::SGIPluginImpl
{
public:
    SGIPlugin_osg_Implementation(SGIPluginHostInterface * hostInterface=NULL)
        : osg_plugin::SGIPluginImpl(hostInterface)
    {
        // register the osgNodeMask enum as bit field
        registerNamedEnum<osg_helpers::osgNodeMask>("node mask", true );
        registerGLConstants();

        registerNamedEnum<osg_helpers::GLModeValue>("GL mode value", true );
        registerNamedEnumValue<osg_helpers::GLModeValue>(osg::StateAttribute::ON, "ON");
        registerNamedEnumValue<osg_helpers::GLModeValue>(osg::StateAttribute::OFF, "OFF");
        registerNamedEnumValue<osg_helpers::GLModeValue>(osg::StateAttribute::OVERRIDE, "OVERRIDE");
        registerNamedEnumValue<osg_helpers::GLModeValue>(osg::StateAttribute::INHERIT, "INHERIT");
        registerNamedEnumValue<osg_helpers::GLModeValue>(osg::StateAttribute::PROTECTED, "PROTECTED");

        registerNamedEnum<osg_helpers::GLModeOverrideValue>("GL mode override value", true );
        registerNamedEnumValue<osg_helpers::GLModeOverrideValue>(osg::StateAttribute::ON, "ON");
        registerNamedEnumValue<osg_helpers::GLModeOverrideValue>(osg::StateAttribute::OFF, "OFF");
        registerNamedEnumValue<osg_helpers::GLModeOverrideValue>(osg::StateAttribute::OVERRIDE, "OVERRIDE");
        registerNamedEnumValue<osg_helpers::GLModeOverrideValue>(osg::StateAttribute::INHERIT, "INHERIT");
        registerNamedEnumValue<osg_helpers::GLModeOverrideValue>(osg::StateAttribute::PROTECTED, "PROTECTED");

        SGIITEMTYPE_NAME(SGIItemTypeStateSetEffective);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetModeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetAttributeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetTextureModeList);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetTextureAttributeLists);
        SGIITEMTYPE_NAME(SGIItemTypeStateSetUniformList);
        SGIITEMTYPE_NAME(SGIItemTypeParentalNodePath);
        SGIITEMTYPE_NAME(SGIItemTypeDrawableTexCoordsList);
        SGIITEMTYPE_NAME(SGIItemTypePrimitiveSetList);

        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextCameras);
        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextOperations);
        SGIITEMTYPE_NAME(SGIItemTypeGraphicsContextGLExtensions);
        SGIITEMTYPE_NAME(SGIItemTypeImage);
        SGIITEMTYPE_NAME(SGIItemTypeShaders);
        SGIITEMTYPE_NAME(SGIItemTypeShaderSource);
        SGIITEMTYPE_NAME(SGIItemTypeShaderCodeInjectionMap);
        SGIITEMTYPE_NAME(SGIItemTypePickerHits);
        SGIITEMTYPE_NAME(SGIItemTypePickerHit);

        SGIITEMTYPE_NAME(SGIItemTypeSlaves);

        SGIITEMTYPE_NAME(SGIItemTypeArrayData);
        SGIITEMTYPE_NAME(SGIItemTypeEventHandlers);
        SGIITEMTYPE_NAME(SGIItemTypeDevices);
        SGIITEMTYPE_NAME(SGIItemTypeActivePagedLODs);
        SGIITEMTYPE_NAME(SGIItemTypeThreads);
        SGIITEMTYPE_NAME(SGIItemTypeStateAttibutes);
        SGIITEMTYPE_NAME(SGIItemTypeStateSets);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseScenes);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseViews);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseCameras);
        SGIITEMTYPE_NAME(SGIItemTypeViewerBaseContexts);
        SGIITEMTYPE_NAME(SGIItemTypeLeaves);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoStateSetStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoRenderBinStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoCameraStack);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoAppliedProgramSet);
        SGIITEMTYPE_NAME(SGIItemTypeRenderInfoState);
        SGIITEMTYPE_NAME(SGIItemTypeReadersWriters);
        SGIITEMTYPE_NAME(SGIItemTypeImageProcessors);
        SGIITEMTYPE_NAME(SGIItemTypeDatabaseRevisions);
        SGIITEMTYPE_NAME(SGIItemTypeBufferDatas);
        SGIITEMTYPE_NAME(SGIItemTypePendingChild);
        SGIITEMTYPE_NAME(SGIItemTypeChannels);
        SGIITEMTYPE_NAME(SGIItemTypeAnimations);
        SGIITEMTYPE_NAME(SGIItemTypeSerializers);
        SGIITEMTYPE_NAME(SGIItemTypeWrappers);
        SGIITEMTYPE_NAME(SGIItemTypeCompressors);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerFileRequests);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerHttpRequests);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerDataToCompile);
        SGIITEMTYPE_NAME(SGIItemTypeDBPagerDataToMerge);
        SGIITEMTYPE_NAME(SGIItemTypeColorLayers);
        SGIITEMTYPE_NAME(SGIItemTypeCameaBufferAttachments);

    }
    SGIPlugin_osg_Implementation(const SGIPlugin_osg_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_osg, SGIPlugin_osg_Implementation);

    void registerGLConstants()
    {
        registerNamedEnum<osg_helpers::GLEnum>("GL enum", false );

        REGISTER_GLENUM(GL_BYTE);
        REGISTER_GLENUM(GL_UNSIGNED_BYTE);
        REGISTER_GLENUM(GL_SHORT);
        REGISTER_GLENUM(GL_UNSIGNED_SHORT);
        REGISTER_GLENUM(GL_INT);
        REGISTER_GLENUM(GL_UNSIGNED_INT);
        REGISTER_GLENUM(GL_FLOAT);
        REGISTER_GLENUM(GL_2_BYTES);
        REGISTER_GLENUM(GL_3_BYTES);
        REGISTER_GLENUM(GL_4_BYTES);
        REGISTER_GLENUM(GL_DOUBLE);

        REGISTER_GLENUM(GL_ALPHA_TEST);
        REGISTER_GLENUM(GL_AUTO_NORMAL);
        REGISTER_GLENUM(GL_BLEND);

        REGISTER_GLENUM(GL_EDGE_FLAG);
        REGISTER_GLENUM(GL_CULL_FACE);
        REGISTER_GLENUM(GL_CULL_FACE_MODE);
        REGISTER_GLENUM(GL_FRONT_FACE);

        REGISTER_GLENUM(GL_DEPTH_TEST);
        REGISTER_GLENUM(GL_DITHER);
        REGISTER_GLENUM(GL_FOG);
        REGISTER_GLENUM(GL_LIGHTING);
        REGISTER_GLENUM(GL_NORMALIZE);
        REGISTER_GLENUM(GL_RESCALE_NORMAL);

        REGISTER_GLENUM(GL_POINT_SMOOTH);
        REGISTER_GLENUM(GL_POINT_SIZE);
        REGISTER_GLENUM(GL_POINT_SIZE_GRANULARITY);
        REGISTER_GLENUM(GL_POINT_SIZE_RANGE);

        REGISTER_GLENUM(GL_POLYGON_MODE);
        REGISTER_GLENUM(GL_POLYGON_SMOOTH);
        REGISTER_GLENUM(GL_POLYGON_STIPPLE);

        REGISTER_GLENUM(GL_LINE_SMOOTH);
        REGISTER_GLENUM(GL_LINE_STIPPLE);
        REGISTER_GLENUM(GL_LINE_STIPPLE_PATTERN);
        REGISTER_GLENUM(GL_LINE_STIPPLE_REPEAT);
        REGISTER_GLENUM(GL_LINE_WIDTH);
        REGISTER_GLENUM(GL_LINE_WIDTH_GRANULARITY);
        REGISTER_GLENUM(GL_LINE_WIDTH_RANGE);

        REGISTER_GLENUM(GL_SCISSOR_BOX);
        REGISTER_GLENUM(GL_SCISSOR_TEST);
        REGISTER_GLENUM(GL_STENCIL_TEST);
        REGISTER_GLENUM(GL_LIGHT0);
        REGISTER_GLENUM(GL_LIGHT1);
        REGISTER_GLENUM(GL_LIGHT2);
        REGISTER_GLENUM(GL_LIGHT3);
        REGISTER_GLENUM(GL_LIGHT4);
        REGISTER_GLENUM(GL_LIGHT5);
        REGISTER_GLENUM(GL_LIGHT6);
        REGISTER_GLENUM(GL_LIGHT7);
        REGISTER_GLENUM(GL_TEXTURE_GEN_S);
        REGISTER_GLENUM(GL_TEXTURE_GEN_R);
        REGISTER_GLENUM(GL_TEXTURE_GEN_T);
        REGISTER_GLENUM(GL_TEXTURE_GEN_Q);
        REGISTER_GLENUM(GL_CLAMP);
        REGISTER_GLENUM(GL_CLAMP_TO_EDGE);
        REGISTER_GLENUM(GL_CLAMP_TO_BORDER_ARB);
        REGISTER_GLENUM(GL_REPEAT);
        REGISTER_GLENUM(GL_MIRRORED_REPEAT_IBM);

        REGISTER_GLENUM(GL_LINEAR);
        REGISTER_GLENUM(GL_LINEAR_MIPMAP_LINEAR);
        REGISTER_GLENUM(GL_LINEAR_MIPMAP_NEAREST);
        REGISTER_GLENUM(GL_NEAREST);
        REGISTER_GLENUM(GL_NEAREST_MIPMAP_LINEAR);
        REGISTER_GLENUM(GL_NEAREST_MIPMAP_NEAREST);

        REGISTER_GLENUM(GL_NEVER);
        REGISTER_GLENUM(GL_LESS);
        REGISTER_GLENUM(GL_EQUAL);
        REGISTER_GLENUM(GL_LEQUAL);
        REGISTER_GLENUM(GL_GREATER);
        REGISTER_GLENUM(GL_NOTEQUAL);
        REGISTER_GLENUM(GL_GEQUAL);
        REGISTER_GLENUM(GL_ALWAYS);

        REGISTER_GLENUM(GL_LUMINANCE);
        REGISTER_GLENUM(GL_INTENSITY);
        REGISTER_GLENUM(GL_ALPHA);

        REGISTER_GLENUM(GL_VERTEX_SHADER);
        REGISTER_GLENUM(GL_TESS_CONTROL_SHADER);
        REGISTER_GLENUM(GL_TESS_EVALUATION_SHADER);
        REGISTER_GLENUM(GL_GEOMETRY_SHADER_EXT);
        REGISTER_GLENUM(GL_FRAGMENT_SHADER);

        REGISTER_GLENUM(GL_CLIP_PLANE0);
        REGISTER_GLENUM(GL_CLIP_PLANE1);
        REGISTER_GLENUM(GL_CLIP_PLANE2);
        REGISTER_GLENUM(GL_CLIP_PLANE3);
        REGISTER_GLENUM(GL_CLIP_PLANE4);
        REGISTER_GLENUM(GL_CLIP_PLANE5);

        REGISTER_GLENUM(GL_TEXTURE_1D);
        REGISTER_GLENUM(GL_TEXTURE_2D);

        REGISTER_GLENUM(GL_POINTS);
        REGISTER_GLENUM(GL_LINES);
        REGISTER_GLENUM(GL_LINE_LOOP);
        REGISTER_GLENUM(GL_LINE_STRIP);
        REGISTER_GLENUM(GL_TRIANGLES);
        REGISTER_GLENUM(GL_TRIANGLE_STRIP);
        REGISTER_GLENUM(GL_TRIANGLE_FAN);
        REGISTER_GLENUM(GL_QUADS);
        REGISTER_GLENUM(GL_QUAD_STRIP);
        REGISTER_GLENUM(GL_POLYGON);

        REGISTER_GLENUM(GL_SRC_COLOR);
        REGISTER_GLENUM(GL_ONE_MINUS_SRC_COLOR);
        REGISTER_GLENUM(GL_SRC_ALPHA);
        REGISTER_GLENUM(GL_ONE_MINUS_SRC_ALPHA);
        REGISTER_GLENUM(GL_DST_ALPHA);
        REGISTER_GLENUM(GL_ONE_MINUS_DST_ALPHA);
        REGISTER_GLENUM(GL_DST_COLOR);
        REGISTER_GLENUM(GL_ONE_MINUS_DST_COLOR);
        REGISTER_GLENUM(GL_SRC_ALPHA_SATURATE);

        REGISTER_GLENUM(GL_AMBIENT);
        REGISTER_GLENUM(GL_DIFFUSE);
        REGISTER_GLENUM(GL_SPECULAR);
        REGISTER_GLENUM(GL_EMISSION);
        REGISTER_GLENUM(GL_AMBIENT_AND_DIFFUSE);

        REGISTER_GLENUM(GL_FRONT_LEFT);
        REGISTER_GLENUM(GL_FRONT_RIGHT);
        REGISTER_GLENUM(GL_BACK_LEFT);
        REGISTER_GLENUM(GL_BACK_RIGHT);
        REGISTER_GLENUM(GL_FRONT);
        REGISTER_GLENUM(GL_BACK);
        REGISTER_GLENUM(GL_LEFT);
        REGISTER_GLENUM(GL_RIGHT);
        REGISTER_GLENUM(GL_FRONT_AND_BACK);
        REGISTER_GLENUM(GL_AUX0);
        REGISTER_GLENUM(GL_AUX1);
        REGISTER_GLENUM(GL_AUX2);
        REGISTER_GLENUM(GL_AUX3);

        REGISTER_GLENUM(GL_COMPRESSED_ALPHA_ARB);
        REGISTER_GLENUM(GL_COMPRESSED_INTENSITY_ARB);
        REGISTER_GLENUM(GL_COMPRESSED_LUMINANCE_ALPHA_ARB);
        REGISTER_GLENUM(GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
        REGISTER_GLENUM(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

        REGISTER_GLENUM(GL_COMPRESSED_RGB_ARB);
        REGISTER_GLENUM(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
        REGISTER_GLENUM(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);

        REGISTER_GLENUM(GL_COMPRESSED_SIGNED_RED_RGTC1_EXT);
        REGISTER_GLENUM(GL_COMPRESSED_RED_RGTC1_EXT);
        REGISTER_GLENUM(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT);
        REGISTER_GLENUM(GL_COMPRESSED_RED_GREEN_RGTC2_EXT);

        REGISTER_GLENUM(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
        REGISTER_GLENUM(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);
        REGISTER_GLENUM(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
        REGISTER_GLENUM(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);

        REGISTER_GLENUM(GL_ETC1_RGB8_OES);

        REGISTER_GLENUM(GL_RGB);
        REGISTER_GLENUM(GL_RGBA);

        REGISTER_GLENUM(GL_RGB4);
        REGISTER_GLENUM(GL_RGB5);
        REGISTER_GLENUM(GL_RGB8);
        REGISTER_GLENUM(GL_RGB10);
        REGISTER_GLENUM(GL_RGB12);
        REGISTER_GLENUM(GL_RGB16);
        REGISTER_GLENUM(GL_RGBA2);
        REGISTER_GLENUM(GL_RGBA4);
        REGISTER_GLENUM(GL_RGB5_A1);
        REGISTER_GLENUM(GL_RGBA8);
        REGISTER_GLENUM(GL_RGB10_A2);
        REGISTER_GLENUM(GL_RGBA12);
        REGISTER_GLENUM(GL_RGBA16);

#ifdef GL_PROGRAM_POINT_SIZE
        REGISTER_GLENUM(GL_PROGRAM_POINT_SIZE);
#endif
        REGISTER_GLENUM(GL_POINT_SPRITE);

        REGISTER_GLENUM(GL_ARRAY_BUFFER_ARB);
        REGISTER_GLENUM(GL_ELEMENT_ARRAY_BUFFER_ARB);
        REGISTER_GLENUM(GL_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_VERTEX_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_NORMAL_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_COLOR_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_INDEX_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB);
        REGISTER_GLENUM(GL_STREAM_DRAW);
        REGISTER_GLENUM(GL_STREAM_READ);
        REGISTER_GLENUM(GL_STREAM_COPY);
        REGISTER_GLENUM(GL_STATIC_DRAW);
        REGISTER_GLENUM(GL_STATIC_READ);
        REGISTER_GLENUM(GL_STATIC_COPY);
        REGISTER_GLENUM(GL_DYNAMIC_DRAW);
        REGISTER_GLENUM(GL_DYNAMIC_READ);
        REGISTER_GLENUM(GL_DYNAMIC_COPY);

        REGISTER_GLENUM(GL_READ_ONLY_ARB);
        REGISTER_GLENUM(GL_WRITE_ONLY_ARB);
        REGISTER_GLENUM(GL_READ_WRITE_ARB);
        REGISTER_GLENUM(GL_BUFFER_SIZE_ARB);
        REGISTER_GLENUM(GL_BUFFER_USAGE_ARB);
        REGISTER_GLENUM(GL_BUFFER_ACCESS_ARB);
        REGISTER_GLENUM(GL_BUFFER_MAPPED_ARB);
        REGISTER_GLENUM(GL_BUFFER_MAP_POINTER_ARB);

        REGISTER_GLENUM(GL_PIXEL_PACK_BUFFER);
        REGISTER_GLENUM(GL_PIXEL_UNPACK_BUFFER);
        REGISTER_GLENUM(GL_PIXEL_PACK_BUFFER_BINDING);
        REGISTER_GLENUM(GL_PIXEL_UNPACK_BUFFER_BINDING);

        // EXT_blend_color
        // http://www.msi.unilim.fr/~porquet/glexts/GL_EXT_blend_color.txt.html
        REGISTER_GLENUM(GL_CONSTANT_COLOR_EXT);
        REGISTER_GLENUM(GL_ONE_MINUS_CONSTANT_COLOR_EXT);
        REGISTER_GLENUM(GL_CONSTANT_ALPHA_EXT);
        REGISTER_GLENUM(GL_ONE_MINUS_CONSTANT_ALPHA_EXT);
        REGISTER_GLENUM(GL_BLEND_COLOR_EXT);

		REGISTER_GLENUM(GL_FRAMEBUFFER_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_EXT);
		REGISTER_GLENUM(GL_STENCIL_INDEX1_EXT);
		REGISTER_GLENUM(GL_STENCIL_INDEX4_EXT);
		REGISTER_GLENUM(GL_STENCIL_INDEX8_EXT);
		REGISTER_GLENUM(GL_STENCIL_INDEX16_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_WIDTH_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_HEIGHT_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_INTERNAL_FORMAT_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_RED_SIZE_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_GREEN_SIZE_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_BLUE_SIZE_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_ALPHA_SIZE_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_DEPTH_SIZE_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_STENCIL_SIZE_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT0_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT1_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT2_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT3_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT4_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT5_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT6_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT7_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT8_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT9_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT10_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT11_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT12_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT13_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT14_EXT);
		REGISTER_GLENUM(GL_COLOR_ATTACHMENT15_EXT);
		REGISTER_GLENUM(GL_DEPTH_ATTACHMENT_EXT);
		REGISTER_GLENUM(GL_STENCIL_ATTACHMENT_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_COMPLETE_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_UNSUPPORTED_EXT);
		REGISTER_GLENUM(GL_FRAMEBUFFER_BINDING_EXT);
		REGISTER_GLENUM(GL_RENDERBUFFER_BINDING_EXT);
		REGISTER_GLENUM(GL_MAX_COLOR_ATTACHMENTS_EXT);
		REGISTER_GLENUM(GL_MAX_RENDERBUFFER_SIZE_EXT);
		REGISTER_GLENUM(GL_INVALID_FRAMEBUFFER_OPERATION_EXT);

        REGISTER_GLENUM(GL_TEXTURE_RECTANGLE);
    }

};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(osg)
