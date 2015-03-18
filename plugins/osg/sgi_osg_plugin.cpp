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
#include <osgDB/DatabasePager>

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
                                                                 osg::FBOExtensions,
                                                                 osgAnimation::Channel,
                                                                 osgAnimation::Target,
                                                                 osgAnimation::Sampler,
                                                                 osgDB::DatabasePager::DatabaseThread,
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
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Node, LOKI_TYPELIST(osg::Group, osg::Geode))
SGI_CALL_FUNCTION_FOR_OBJECT_BASE(osg::Group, LOKI_TYPELIST(osg::Transform, osg::LOD, osg::ProxyNode, osg::CoordinateSystemNode, osgFX::Effect, osgTerrain::TerrainTile))
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


#define GLCONSTANT(c) \
    registerNamedEnumValue<osg_helpers::GLConstant>(c, #c)

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

    }
    SGIPlugin_osg_Implementation(const SGIPlugin_osg_Implementation & rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : osg_plugin::SGIPluginImpl(rhs, copyop)
    {
    }

    META_Object(sgi_osg, SGIPlugin_osg_Implementation);

    void registerGLConstants()
    {
        registerNamedEnum<osg_helpers::GLConstant>("GL constant", false );

        GLCONSTANT(GL_BYTE);
        GLCONSTANT(GL_UNSIGNED_BYTE);
        GLCONSTANT(GL_SHORT);
        GLCONSTANT(GL_UNSIGNED_SHORT);
        GLCONSTANT(GL_INT);
        GLCONSTANT(GL_UNSIGNED_INT);
        GLCONSTANT(GL_FLOAT);
        GLCONSTANT(GL_2_BYTES);
        GLCONSTANT(GL_3_BYTES);
        GLCONSTANT(GL_4_BYTES);
        GLCONSTANT(GL_DOUBLE);

        GLCONSTANT(GL_ALPHA_TEST);
        GLCONSTANT(GL_AUTO_NORMAL);
        GLCONSTANT(GL_BLEND);

        GLCONSTANT(GL_EDGE_FLAG);
        GLCONSTANT(GL_CULL_FACE);
        GLCONSTANT(GL_CULL_FACE_MODE);
        GLCONSTANT(GL_FRONT_FACE);

        GLCONSTANT(GL_DEPTH_TEST);
        GLCONSTANT(GL_DITHER);
        GLCONSTANT(GL_FOG);
        GLCONSTANT(GL_LIGHTING);
        GLCONSTANT(GL_NORMALIZE);
        GLCONSTANT(GL_RESCALE_NORMAL);

        GLCONSTANT(GL_POINT_SMOOTH);
        GLCONSTANT(GL_POINT_SIZE);
        GLCONSTANT(GL_POINT_SIZE_GRANULARITY);
        GLCONSTANT(GL_POINT_SIZE_RANGE);

        GLCONSTANT(GL_POLYGON_MODE);
        GLCONSTANT(GL_POLYGON_SMOOTH);
        GLCONSTANT(GL_POLYGON_STIPPLE);

        GLCONSTANT(GL_LINE_SMOOTH);
        GLCONSTANT(GL_LINE_STIPPLE);
        GLCONSTANT(GL_LINE_STIPPLE_PATTERN);
        GLCONSTANT(GL_LINE_STIPPLE_REPEAT);
        GLCONSTANT(GL_LINE_WIDTH);
        GLCONSTANT(GL_LINE_WIDTH_GRANULARITY);
        GLCONSTANT(GL_LINE_WIDTH_RANGE);

        GLCONSTANT(GL_SCISSOR_BOX);
        GLCONSTANT(GL_SCISSOR_TEST);
        GLCONSTANT(GL_STENCIL_TEST);
        GLCONSTANT(GL_LIGHT0);
        GLCONSTANT(GL_LIGHT1);
        GLCONSTANT(GL_LIGHT2);
        GLCONSTANT(GL_LIGHT3);
        GLCONSTANT(GL_LIGHT4);
        GLCONSTANT(GL_LIGHT5);
        GLCONSTANT(GL_LIGHT6);
        GLCONSTANT(GL_LIGHT7);
        GLCONSTANT(GL_TEXTURE_GEN_S);
        GLCONSTANT(GL_TEXTURE_GEN_R);
        GLCONSTANT(GL_TEXTURE_GEN_T);
        GLCONSTANT(GL_TEXTURE_GEN_Q);
        GLCONSTANT(GL_CLAMP);
        GLCONSTANT(GL_CLAMP_TO_EDGE);
        GLCONSTANT(GL_CLAMP_TO_BORDER_ARB);
        GLCONSTANT(GL_REPEAT);
        GLCONSTANT(GL_MIRRORED_REPEAT_IBM);

        GLCONSTANT(GL_LINEAR);
        GLCONSTANT(GL_LINEAR_MIPMAP_LINEAR);
        GLCONSTANT(GL_LINEAR_MIPMAP_NEAREST);
        GLCONSTANT(GL_NEAREST);
        GLCONSTANT(GL_NEAREST_MIPMAP_LINEAR);
        GLCONSTANT(GL_NEAREST_MIPMAP_NEAREST);

        GLCONSTANT(GL_NEVER);
        GLCONSTANT(GL_LESS);
        GLCONSTANT(GL_EQUAL);
        GLCONSTANT(GL_LEQUAL);
        GLCONSTANT(GL_GREATER);
        GLCONSTANT(GL_NOTEQUAL);
        GLCONSTANT(GL_GEQUAL);
        GLCONSTANT(GL_ALWAYS);

        GLCONSTANT(GL_LUMINANCE);
        GLCONSTANT(GL_INTENSITY);
        GLCONSTANT(GL_ALPHA);

        GLCONSTANT(GL_VERTEX_SHADER);
        GLCONSTANT(GL_TESS_CONTROL_SHADER);
        GLCONSTANT(GL_TESS_EVALUATION_SHADER);
        GLCONSTANT(GL_GEOMETRY_SHADER_EXT);
        GLCONSTANT(GL_FRAGMENT_SHADER);

        GLCONSTANT(GL_CLIP_PLANE0);
        GLCONSTANT(GL_CLIP_PLANE1);
        GLCONSTANT(GL_CLIP_PLANE2);
        GLCONSTANT(GL_CLIP_PLANE3);
        GLCONSTANT(GL_CLIP_PLANE4);
        GLCONSTANT(GL_CLIP_PLANE5);

        GLCONSTANT(GL_TEXTURE_1D);
        GLCONSTANT(GL_TEXTURE_2D);

        GLCONSTANT(GL_POINTS);
        GLCONSTANT(GL_LINES);
        GLCONSTANT(GL_LINE_LOOP);
        GLCONSTANT(GL_LINE_STRIP);
        GLCONSTANT(GL_TRIANGLES);
        GLCONSTANT(GL_TRIANGLE_STRIP);
        GLCONSTANT(GL_TRIANGLE_FAN);
        GLCONSTANT(GL_QUADS);
        GLCONSTANT(GL_QUAD_STRIP);
        GLCONSTANT(GL_POLYGON);

        GLCONSTANT(GL_SRC_COLOR);
        GLCONSTANT(GL_ONE_MINUS_SRC_COLOR);
        GLCONSTANT(GL_SRC_ALPHA);
        GLCONSTANT(GL_ONE_MINUS_SRC_ALPHA);
        GLCONSTANT(GL_DST_ALPHA);
        GLCONSTANT(GL_ONE_MINUS_DST_ALPHA);
        GLCONSTANT(GL_DST_COLOR);
        GLCONSTANT(GL_ONE_MINUS_DST_COLOR);
        GLCONSTANT(GL_SRC_ALPHA_SATURATE);

        GLCONSTANT(GL_AMBIENT);
        GLCONSTANT(GL_DIFFUSE);
        GLCONSTANT(GL_SPECULAR);
        GLCONSTANT(GL_EMISSION);
        GLCONSTANT(GL_AMBIENT_AND_DIFFUSE);

        GLCONSTANT(GL_FRONT_LEFT);
        GLCONSTANT(GL_FRONT_RIGHT);
        GLCONSTANT(GL_BACK_LEFT);
        GLCONSTANT(GL_BACK_RIGHT);
        GLCONSTANT(GL_FRONT);
        GLCONSTANT(GL_BACK);
        GLCONSTANT(GL_LEFT);
        GLCONSTANT(GL_RIGHT);
        GLCONSTANT(GL_FRONT_AND_BACK);
        GLCONSTANT(GL_AUX0);
        GLCONSTANT(GL_AUX1);
        GLCONSTANT(GL_AUX2);
        GLCONSTANT(GL_AUX3);

        GLCONSTANT(GL_COMPRESSED_ALPHA_ARB);
        GLCONSTANT(GL_COMPRESSED_INTENSITY_ARB);
        GLCONSTANT(GL_COMPRESSED_LUMINANCE_ALPHA_ARB);
        GLCONSTANT(GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
        GLCONSTANT(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);

        GLCONSTANT(GL_COMPRESSED_RGB_ARB);
        GLCONSTANT(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
        GLCONSTANT(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);

        GLCONSTANT(GL_COMPRESSED_SIGNED_RED_RGTC1_EXT);
        GLCONSTANT(GL_COMPRESSED_RED_RGTC1_EXT);
        GLCONSTANT(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT);
        GLCONSTANT(GL_COMPRESSED_RED_GREEN_RGTC2_EXT);

        GLCONSTANT(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
        GLCONSTANT(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);
        GLCONSTANT(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
        GLCONSTANT(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);

        GLCONSTANT(GL_ETC1_RGB8_OES);

        GLCONSTANT(GL_RGB);
        GLCONSTANT(GL_RGBA);

        GLCONSTANT(GL_RGB4);
        GLCONSTANT(GL_RGB5);
        GLCONSTANT(GL_RGB8);
        GLCONSTANT(GL_RGB10);
        GLCONSTANT(GL_RGB12);
        GLCONSTANT(GL_RGB16);
        GLCONSTANT(GL_RGBA2);
        GLCONSTANT(GL_RGBA4);
        GLCONSTANT(GL_RGB5_A1);
        GLCONSTANT(GL_RGBA8);
        GLCONSTANT(GL_RGB10_A2);
        GLCONSTANT(GL_RGBA12);
        GLCONSTANT(GL_RGBA16);

#ifdef GL_PROGRAM_POINT_SIZE
        GLCONSTANT(GL_PROGRAM_POINT_SIZE);
#endif
        GLCONSTANT(GL_POINT_SPRITE);

        GLCONSTANT(GL_ARRAY_BUFFER_ARB);
        GLCONSTANT(GL_ELEMENT_ARRAY_BUFFER_ARB);
        GLCONSTANT(GL_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_VERTEX_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_NORMAL_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_COLOR_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_INDEX_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB);
        GLCONSTANT(GL_STREAM_DRAW);
        GLCONSTANT(GL_STREAM_READ);
        GLCONSTANT(GL_STREAM_COPY);
        GLCONSTANT(GL_STATIC_DRAW);
        GLCONSTANT(GL_STATIC_READ);
        GLCONSTANT(GL_STATIC_COPY);
        GLCONSTANT(GL_DYNAMIC_DRAW);
        GLCONSTANT(GL_DYNAMIC_READ);
        GLCONSTANT(GL_DYNAMIC_COPY);

        GLCONSTANT(GL_READ_ONLY_ARB);
        GLCONSTANT(GL_WRITE_ONLY_ARB);
        GLCONSTANT(GL_READ_WRITE_ARB);
        GLCONSTANT(GL_BUFFER_SIZE_ARB);
        GLCONSTANT(GL_BUFFER_USAGE_ARB);
        GLCONSTANT(GL_BUFFER_ACCESS_ARB);
        GLCONSTANT(GL_BUFFER_MAPPED_ARB);
        GLCONSTANT(GL_BUFFER_MAP_POINTER_ARB);

        GLCONSTANT(GL_PIXEL_PACK_BUFFER);
        GLCONSTANT(GL_PIXEL_UNPACK_BUFFER);
        GLCONSTANT(GL_PIXEL_PACK_BUFFER_BINDING);
        GLCONSTANT(GL_PIXEL_UNPACK_BUFFER_BINDING);

    }

};

} // namespace sgi

SGI_PLUGIN_IMPLEMENT(osg)
