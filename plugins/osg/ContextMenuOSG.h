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
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Referenced)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Object)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Node)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Group)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::StateSet)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Geode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Transform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::MatrixTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::PositionAttitudeTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::AutoTransform)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Billboard)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::CameraView)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Camera)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::View)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::ProxyNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::LOD)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::PagedLOD)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Drawable)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::ShapeDrawable)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Geometry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Sphere)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Box)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Cone)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Cylinder)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Capsule)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::InfinitePlane)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::TriangleMesh)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::HeightField)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::CompositeShape)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Image)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::StateAttribute)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Texture)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Texture2D)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Texture3D)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::ClipNode)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::GraphicsContext)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Shader)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Program)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Depth)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Uniform)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::BufferData)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osg::Array)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgDB::Registry)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgDB::ImagePager)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgDB::DatabasePager)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgGA::GUIEventHandler)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgGA::GUIEventAdapter)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgGA::CameraManipulator)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgViewer::View)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgViewer::Scene)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgViewer::GraphicsWindow)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgQt::GraphicsWindowQt)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgText::TextBase)
CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgText::Text)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(osgAnimation::AnimationManagerBase)

CONTEXT_MENU_POPULATE_IMPL_DECLARE(sgi::ISceneGraphDialogToolsMenu)

} // namespace osg_plugin
} // namespace sgi
