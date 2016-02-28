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


void writePrettyHTMLGeometryVertices(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLGeometryNormals(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLColorArray(std::basic_ostream<char>& os, const osg::Array * colors_, bool brief);
//void writePrettyHTMLGeometryVertexIndicies(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief);
void writePrettyHTMLGeometryTexCoords(std::basic_ostream<char>& os, const osg::Geometry * object, int number, bool brief);

} // namespace osg_plugin
} // namespace sgi
