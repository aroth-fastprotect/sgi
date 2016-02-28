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
    class Stats;

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
OBJECT_TREE_BUILD_ROOT_IMPL_TEMPLATE()

} // namespace osg_plugin
} // namespace sgi
