#pragma once

#include <sgi/plugins/SGIPluginMacros.h>
#include <typeinfo>
#include <sgi/plugins/SGIHostItemOsg.h>

namespace osg {
    class NodeCallback;
    class Group;
    class Camera;
    class View;
    class FrameStamp;
    class RefMatrixd;
    class RefMatrixf;
    class StateSet;
    class StateAttribute;
    class Drawable;
    class Geode;
    class Geometry;
    class ShapeDrawable;
    class Image;
    class Texture;
    class Texture1D;
    class Texture2D;
    class Texture3D;
    class Uniform;
    class DisplaySettings;
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
    class Sphere;
    class Box;
    class Cylinder;
    class Cone;
    class Capsule;
    class InfinitePlane;
    class TriangleMesh;
    class HeightField;
    class CompositeShape;
    class ConvexHull;

    class BufferData;
    class PrimitiveSet;
    class DrawElements;
    class DrawElementsUByte;
    class DrawElementsUShort;
    class DrawElementsUInt;
    class DrawArrays;
    class DrawArrayLengths;
    class IndexArray;
    class BufferObject;
    class VertexBufferObject;

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
    class BlendEquation;
    class LineWidth;
    class LineStipple;
    class Point;
    class PolygonStipple;
    class Depth;

    class Operation;
    struct GraphicsOperation;
    struct BarrierOperation;

    class OperationQueue;
    class OperationThread;
    class GraphicsThread;
    class GraphicsContext;
    class ShaderComposer;
    class ShaderComponent;
}

namespace OpenThreads {
    class Thread;
}

namespace sgi {

class ReferencedPickerBase;
class ReferencedPicker;
class ReferencedLinePicker;

class SGIItemOsg;

namespace osg_plugin {

class RenderInfoDrawable;

WRITE_PRETTY_HTML_IMPL_TEMPLATE()
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Referenced)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ObserverSet)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Object)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::UserDataContainer)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Node)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Group)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Camera)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::FrameStamp)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::RefMatrixd)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::RefMatrixf)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Geode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::LOD)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PagedLOD)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ProxyNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::CoordinateSystemNode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Transform)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::MatrixTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PositionAttitudeTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::AutoTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DisplaySettings)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::NodeVisitor)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Uniform)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BufferData)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Array)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::IndexArray)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BufferObject)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::VertexBufferObject)

void writePrettyHTMLGeometryVertices(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLGeometryNormals(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLColorArray(std::basic_ostream<char>& os, const osg::Array * colors_, bool brief);
//void writePrettyHTMLGeometryVertexIndicies(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLGeometryTexCoords(std::basic_ostream<char>& os, const osg::Geometry * object, int number, bool brief);

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Shader)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ClipNode)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Geometry)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Drawable)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ShapeDrawable)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Shape)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Sphere)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Box)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Cylinder)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Cone)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Capsule)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::InfinitePlane)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::TriangleMesh)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::HeightField)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::CompositeShape)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ConvexHull)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PrimitiveSet)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawElements)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawElementsUByte)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawElementsUShort)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawElementsUInt)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawArrays)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::DrawArrayLengths)

//void writePrettyStateAttributeHTML(std::basic_ostream<char>& os, const osg::StateAttribute * object, bool table, bool brief=true);

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::View)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::NodeCallback)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::EllipsoidModel)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::StateSet)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::State)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::StateAttribute)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Image)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Texture)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Texture1D)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Texture2D)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Texture3D)


WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PolygonMode)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PolygonOffset)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Material)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::TexEnv)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Light)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::LightModel)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Stencil)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Viewport)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Program)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::TexGen)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ClipPlane)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BlendFunc)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BlendColor)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BlendEquation)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::LineWidth)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::LineStipple)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Point)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::PolygonStipple)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Depth)

WRITE_PRETTY_HTML_IMPL_DECLARE(osg::Operation)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::GraphicsOperation)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::BarrierOperation)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::OperationQueue)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::OperationThread)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::GraphicsThread)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::GraphicsContext)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ShaderComposer)
WRITE_PRETTY_HTML_IMPL_DECLARE(osg::ShaderComponent)

WRITE_PRETTY_HTML_IMPL_DECLARE(RenderInfoDrawable)

WRITE_PRETTY_HTML_IMPL_DECLARE(OpenThreads::Thread)

WRITE_PRETTY_HTML_IMPL_DECLARE(sgi::ReferencedPickerBase)
WRITE_PRETTY_HTML_IMPL_DECLARE(sgi::ReferencedPicker)
WRITE_PRETTY_HTML_IMPL_DECLARE(sgi::ReferencedLinePicker)

/*
void writePrettyHTML(std::basic_ostream<char>& os, const osgUtil::PolytopeIntersector::Intersection & hit, bool table, bool brief=false);
void writePrettyHTML(std::basic_ostream<char>& os, const osgUtil::PolytopeIntersector::Intersections & hits, bool table, bool brief=false);

void writePrettyHTML(std::basic_ostream<char>& os, const osgUtil::LineSegmentIntersector::Intersection & hit, bool table, bool brief=false);
void writePrettyHTML(std::basic_ostream<char>& os, const osgUtil::LineSegmentIntersector::Intersections & hits, bool table, bool brief=false);
*/

} // namespace osg_plugin
} // namespace sgi
