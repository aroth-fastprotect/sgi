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
    class CameraView;
    class View;
    class StateSet;
    class StateAttribute;
    class Drawable;
    class Geode;
    class Billboard;
    class Geometry;
    class ShapeDrawable;
    class Image;
    class HeightField;
    class Texture;
    class Texture1D;
    class Texture2D;
    class Texture3D;

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
    class Sphere;
    class Capsule;
    class InfinitePlane;
    class TriangleMesh;
    class CompositeShape;

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
    class BlendColor;
    class LineWidth;
    class LineStipple;
    class Point;
    class PolygonStipple;
    class Depth;

    class OperationQueue;
    class OperationThread;
    class GraphicsThread;
    class GraphicsContext;
}

namespace osgGA {
    class GUIEventHandler;
    class GUIEventAdapter;
    class CameraManipulator;
}

namespace osgViewer {
    class View;
	class ViewerBase;
    class GraphicsWindow;
    class Scene;
}

namespace osgDB {
    class Registry;
    class ImagePager;
    class DatabasePager;
}

namespace osgQt {
    class GraphicsWindowQt;
}

namespace osgUtil {
	class IncrementalCompileOperation;
}

namespace osgText {
    class TextBase;
    class Text;
}

namespace osgAnimation {
    class AnimationManagerBase;
}

namespace sgi {

class ISceneGraphDialogToolsMenu;

namespace osg_plugin {

CONTEXT_MENU_POPULATE_IMPL_TEMPLATE()

} // namespace osg_plugin
} // namespace sgi
