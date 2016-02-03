#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <sgi/plugins/SGIHostItemOsg.h>

namespace osg {
    class Vec2f;
    class Vec3f;
    class Vec4f;
    class Vec2d;
    class Vec3d;
    class Vec4d;
    typedef Vec2f Vec2;
    typedef Vec3f Vec3;
    typedef Vec4f Vec4;
    class Quat;
    class Matrixd;
    class Array;

    class Referenced;
    class Object;
    class UserDataContainer;
    class Node;
    typedef std::vector< Node* > NodePath;
    class ObserverNodePath;
    class NodeCallback;
    class Group;
    class Camera;
    class View;
    class StateSet;
    class StateAttribute;
    class Drawable;
    class Geode;
    class Geometry;
    class ShapeDrawable;
    class Image;
	class HeightField;
    class Texture;
    class Texture1D;
    class Texture2D;
    class Texture3D;
    class State;
    class BufferObject;
    class NodeVisitor;

    class LOD;
    class PagedLOD;
    class ProxyNode;

    class Transform;
    class MatrixTransform;
    class PositionAttitudeTransform;
    class AutoTransform;

    class EllipsoidModel;
    class CoordinateSystemNode;

    class Shader;
    class ClipNode;

    class Shape;
    class Box;
    class Cylinder;
    class Cone;

    class BufferData;
    class PrimitiveSet;
    class DrawElements;
    class DrawElementsUByte;
    class DrawElementsUShort;
    class DrawElementsUInt;
    class DrawArrays;
    class DrawArrayLengths;
    class IndexArray;

    class PolygonMode;
    class PolygonOffset;
    class Material;
    class TexEnv;
    class TexGen;
    class Light;
    class LightModel;
    class Stencil;
    class Viewport;
    class Program;
    class ClipPlane;
    class BlendFunc;
    class LineWidth;
    class LineStipple;
    class Point;
    class PolygonStipple;

    class OperationQueue;
    class OperationThread;
    class GraphicsThread;
    class GraphicsContext;

    class ShaderComposer;
    class ShaderComponent;
}

namespace osgViewer {
    class ViewerBase;
    class View;
    class GraphicsWindow;
    class Scene;
    class Renderer;
    class HelpHandler;
    class StatsHandler;
}

namespace osgQt {
    class GraphicsWindowQt;
}

namespace osgGA {
    class GUIEventHandler;
    class GUIEventAdapter;
    class CameraManipulator;
}

namespace osgDB {
    class Registry;
    class DatabasePager;
    class ImagePager;
    class ObjectWrapper;
    class ObjectWrapperManager;
    class Options;
    class ReaderWriter;
    class DatabaseRevision;
    class DatabaseRevisions;
    class FileCache;
    class FileList;
}

namespace osgUtil {
    class RenderBin;
    class RenderStage;
    class RenderLeaf;
    class StateGraph;
    class SceneView;
}

namespace osgTerrain {
    class Layer;
    class ImageLayer;
    class ContourLayer;
    class HeightFieldLayer;
    class ProxyLayer;
    class CompositeLayer;
    class Terrain;
    class TerrainTile;
}

namespace osgText {
    class Font;
    class TextBase;
    class Text;
}

namespace osgAnimation {
    class AnimationManagerBase;
    class Animation;
    class Channel;
}

namespace sgi {

class ReferencedPickerBase;
class ReferencedPicker;
class ReferencedLinePicker;

namespace osg_plugin {

class RenderInfoDrawable;

OBJECT_TREE_BUILD_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Referenced)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::ObserverSet)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Object)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::UserDataContainer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Node)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Group)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::StateSet)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Geode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Drawable)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::ShapeDrawable)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Geometry)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Transform)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Camera)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::View)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Program)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Shader)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Uniform)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::GraphicsContext)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::NodeVisitor)

OBJECT_TREE_BUILD_IMPL_DECLARE(osg::ProxyNode)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::PagedLOD)

OBJECT_TREE_BUILD_IMPL_DECLARE(osg::StateAttribute)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Texture)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Texture2D)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Texture3D)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Image)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::BufferData)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::HeightField)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::Array)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::BufferObject)

OBJECT_TREE_BUILD_IMPL_DECLARE(osg::State)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::ShaderComposer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osg::ShaderComponent)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::Registry)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::Options)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::ReaderWriter)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::DatabasePager)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::ImagePager)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::DatabaseRevision)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::DatabaseRevisions)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::FileCache)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::FileList)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::ObjectWrapper)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgDB::ObjectWrapperManager)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::ViewerBase)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::View)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::GraphicsWindow)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::Scene)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::Renderer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::HelpHandler)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgViewer::StatsHandler)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgQt::GraphicsWindowQt)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgGA::GUIEventHandler)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgGA::GUIEventAdapter)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgGA::CameraManipulator)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgUtil::RenderBin)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgUtil::RenderStage)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgUtil::RenderLeaf)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgUtil::StateGraph)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgUtil::SceneView)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::Terrain)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::TerrainTile)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::Layer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::ImageLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::ContourLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::HeightFieldLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::ProxyLayer)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgTerrain::CompositeLayer)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgText::Font)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgText::TextBase)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgText::Text)

OBJECT_TREE_BUILD_IMPL_DECLARE(osgAnimation::Animation)
OBJECT_TREE_BUILD_IMPL_DECLARE(osgAnimation::AnimationManagerBase)

OBJECT_TREE_BUILD_IMPL_DECLARE(RenderInfoDrawable)

OBJECT_TREE_BUILD_IMPL_DECLARE(sgi::ReferencedPickerBase)
OBJECT_TREE_BUILD_IMPL_DECLARE(sgi::ReferencedPicker)
OBJECT_TREE_BUILD_IMPL_DECLARE(sgi::ReferencedLinePicker)

OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()
OBJECT_TREE_BUILD_ROOT_IMPL_DECLARE(ISceneGraphDialog)

} // namespace osg_plugin
} // namespace sgi
