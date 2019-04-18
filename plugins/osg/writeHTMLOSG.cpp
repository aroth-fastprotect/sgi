#include "stdafx.h"
#include <ostream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <stdint.h>
#include "writeHTMLOSG.h"
#include "SGIItemOsg"

#include <osg/ObserverNodePath>
#include <osg/UserDataContainer>
#include <osg/ValueObject>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/Light>
#include <osg/LightModel>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/Stencil>
#include <osg/Viewport>
#include <osg/ClipNode>
#include <osg/Texture1D>
#include <osg/Texture3D>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/BlendEquation>
#include <osg/LineStipple>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/PolygonStipple>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/GraphicsContext>
#include <osg/GLExtensions>
#include <osg/OperationThread>
#include <osg/CameraView>
#include <osg/NodeVisitor>
#include <osg/CullStack>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/AutoTransform>

#include <osg/PagedLOD>
#include <osg/ProxyNode>

#include <osg/CoordinateSystemNode>

#include <osg/Shape>
#include <osg/Drawable>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/ImageSequence>

#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>

#include <osg/io_utils>

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/WriteFile>

#include <osgAnimation/AnimationManagerBase>
#include <osgAnimation/Skeleton>
#include <osgAnimation/Bone>

#include <osgText/Text>

#include <osgUtil/RenderBin>

#include <sgi/helpers/osg>
#include <sgi/helpers/osg_drawable_helpers>
#include <sgi/helpers/osg_statistics>
#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/ReferencedPicker>

#include "osg_accessor.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Referenced)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ObserverSet)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Object)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::UserDataContainer)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Node)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Group)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Camera)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::FrameStamp)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::RefMatrixd)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::RefMatrixf)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Geode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::LOD)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PagedLOD)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ProxyNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::CoordinateSystemNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Transform)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::MatrixTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PositionAttitudeTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::AutoTransform)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DisplaySettings)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::NodeVisitor)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Stats)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Uniform)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BufferData)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Array)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::IndexArray)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BufferObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::VertexBufferObject)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Shader)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ClipNode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::CullFace)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Geometry)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Drawable)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ShapeDrawable)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Shape)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Sphere)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Box)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Cylinder)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Cone)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Capsule)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::InfinitePlane)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::TriangleMesh)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::HeightField)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::CompositeShape)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ConvexHull)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PrimitiveSet)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawElements)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawElementsUByte)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawElementsUShort)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawElementsUInt)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawArrays)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DrawArrayLengths)

//void writePrettyStateAttributeHTML(std::basic_ostream<char>& os, const osg::StateAttribute * object, bool table, bool brief=true);

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::View)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::NodeCallback)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::EllipsoidModel)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::StateSet)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::State)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::StateAttribute)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Image)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ImageStream)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ImageSequence)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObject)
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::TextureObjectSet)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::TextureObjectManager)
#else
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObjectSet)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture::TextureObjectManager)
#endif
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture1D)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture2D)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Texture3D)


WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PolygonMode)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PolygonOffset)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Material)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::TexEnv)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Light)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::LightModel)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::LightSource)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Stencil)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Viewport)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Program)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Program::PerContextProgram)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Program::ProgramBinary)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::TexGen)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ClipPlane)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BlendFunc)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BlendColor)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BlendEquation)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::LineWidth)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::LineStipple)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Point)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PolygonStipple)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Depth)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Operation)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::GraphicsOperation)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BarrierOperation)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::OperationQueue)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::OperationThread)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::GraphicsThread)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::GraphicsContext)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::GraphicsContext::Traits)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ShaderComposer)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ShaderComponent)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::VertexArrayState)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::StringValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BoolValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::CharValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::UCharValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::ShortValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::UShortValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::IntValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::UIntValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::FloatValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::DoubleValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec2fValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec3fValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec4fValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec2dValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec3dValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::Vec4dValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::QuatValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::PlaneValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::MatrixfValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::MatrixdValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BoundingBoxfValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BoundingBoxdValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BoundingSpherefValueObject)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg::BoundingSpheredValueObject)


WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg_helpers::RenderInfoDrawable)
#if OSG_VERSION_LESS_THAN(3,5,0)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg_helpers::RenderInfoGeometry)
#endif
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg_helpers::RenderInfoDrawCallback)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osg_helpers::RenderInfoData::VertexArrayStateSnapshot)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(OpenThreads::Thread)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedPickerBase)
#ifdef SGI_USE_OSGEARTH
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedPicker)
#endif
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(sgi::ReferencedLinePicker)

using namespace sgi::osg_helpers;

namespace {

    std::string getObjectSuggestedFilenameExtension(const osg::Image * image)
    {
        std::string ret;
        const std::string & filename = image->getFileName();
        if(filename.empty())
            ret = "dds";
        else
        {
            ret = osgDB::getFileExtension(filename);
            if(ret.empty())
                ret = "png";
        }
        return ret;
    }

}


inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingBoxf & b)
{
    return output << std::setprecision(12) << '[' << b._min << ',' << b._max << ']';
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingBoxd & b)
{
    return output << std::setprecision(12) << '[' << b._min << ',' << b._max << ']';
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingSpheref & b)
{
    return output << std::setprecision(12) << '[' << b._center << ",r=" << b._radius << ']';
}

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingSphered & b)
{
    return output << std::setprecision(12) << '[' << b._center << ",r=" << b._radius << ']';
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::PrimitiveSet::Mode & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::PrimitiveSet::Type & t);

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Transform::ReferenceFrame & t)
{
    switch(t)
    {
    case osg::Transform::RELATIVE_RF: os << "RELATIVE_RF"; break;
    case osg::Transform::ABSOLUTE_RF: os << "ABSOLUTE_RF"; break;
    case osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT: os << "ABSOLUTE_RF_INHERIT_VIEWPOINT"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl_OpenThreads_Thread(std::basic_ostream<char>& os, OpenThreads::Thread * object)
{
    os << "<tr><td>running</td><td>" << (object->isRunning() ? "true" : "false") << "</td></tr>" << std::endl;
    os << "<tr><td>pid</td><td>" << object->getProcessId() << "</td></tr>" << std::endl;
    os << "<tr><td>tid</td><td>" << object->getThreadId() << "</td></tr>" << std::endl;
    os << "<tr><td>testCancel</td><td>" << object->testCancel() << "</td></tr>" << std::endl;
    os << "<tr><td>schedulePriority</td><td>" << object->getSchedulePriority() << "</td></tr>" << std::endl;
    os << "<tr><td>schedulePolicy</td><td>" << object->getSchedulePolicy() << "</td></tr>" << std::endl;
    os << "<tr><td>stackSize</td><td>" << object->getStackSize() << "</td></tr>" << std::endl;
    return true;
}

bool writePrettyHTMLImpl<osg::Transform>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Transform * object = getObject<osg::Transform,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add group properties first
            callNextHandler(os);

            // add remaining Transform properties
            os << "<tr><td>ref</td><td>" << object->getReferenceFrame() << "</td></tr>" << std::endl;
            os << "<tr><td>localToWorldMatrix</td><td>";
            {
                osg::Matrixd m;
                object->computeLocalToWorldMatrix(m,NULL);
                writePrettyHTML(os, m, MatrixUsageTypeModelView, object);
            }
            os << "</td></tr>" << std::endl;

            os << "<tr><td>worldToLocalMatrix</td><td>";
            {
                osg::Matrixd m;
                object->computeWorldToLocalMatrix(m,NULL);
                writePrettyHTML(os, m, MatrixUsageTypeModelView, object);
            }
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::MatrixTransform>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::MatrixTransform * object = getObject<osg::MatrixTransform,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Transform properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>matrix</td><td>";
            writePrettyHTML(os, object->getMatrix(), MatrixUsageTypeModelView, object);
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::PositionAttitudeTransform>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PositionAttitudeTransform * object = getObject<osg::PositionAttitudeTransform,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Transform properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>translate</td><td>";
            writePrettyPositionHTML(os, object->getPosition(), object);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>attitude</td><td>";
            writePrettyHTML(os, object->getAttitude());
            os << "</td></tr>" << std::endl;

            os << "<tr><td>scale</td><td>" << object->getScale() << "</td></tr>" << std::endl;

            os << "<tr><td>pivot</td><td>";
            writePrettyPositionHTML(os, object->getPivotPoint(), object);
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::AutoTransform::AutoRotateMode & t)
{
    switch(t)
    {
    case osg::AutoTransform::NO_ROTATION: os << "NO_ROTATION"; break;
    case osg::AutoTransform::ROTATE_TO_SCREEN: os << "ROTATE_TO_SCREEN"; break;
    case osg::AutoTransform::ROTATE_TO_CAMERA: os << "ROTATE_TO_CAMERA"; break;
    case osg::AutoTransform::ROTATE_TO_AXIS: os << "ROTATE_TO_AXIS"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::AutoTransform>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::AutoTransform * object = getObject<osg::AutoTransform,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Transform properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>position</td><td>";
            writePrettyPositionHTML(os, object->getPosition(), object);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>scale</td><td>" << object->getScale() << "</td></tr>" << std::endl;
            os << "<tr><td>min scale</td><td>" << object->getMinimumScale() << "</td></tr>" << std::endl;
            os << "<tr><td>max scale</td><td>" << object->getMaximumScale() << "</td></tr>" << std::endl;
            os << "<tr><td>pivot point</td><td>";
            writePrettyPositionHTML(os, object->getPivotPoint(), object);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>auto update eye movement tolerance</td><td>" << object->getAutoUpdateEyeMovementTolerance() << "</td></tr>" << std::endl;
            os << "<tr><td>auto rotate mode</td><td>" << object->getAutoRotateMode() << "</td></tr>" << std::endl;
            os << "<tr><td>axis</td><td>" << object->getAxis() << "</td></tr>" << std::endl;
            os << "<tr><td>normal</td><td>" << object->getNormal() << "</td></tr>" << std::endl;
            os << "<tr><td>auto scale to screen</td><td>" << (object->getAutoScaleToScreen()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>auto scale transition width ratio</td><td>" << object->getAutoScaleTransitionWidthRatio() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DisplaySettings>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DisplaySettings * object = getObject<osg::DisplaySettings,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Transform properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>displayType</td><td>" << object->getDisplayType() << "</td></tr>" << std::endl;
            os << "<tr><td>stereo</td><td>" << (object->getStereo()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>displayType</td><td>" << object->getStereoMode() << "</td></tr>" << std::endl;
            os << "<tr><td>eyeSeparation</td><td>" << object->getEyeSeparation() << "</td></tr>" << std::endl;
            os << "<tr><td>splitStereoHorizontalEyeMapping</td><td>" << object->getSplitStereoHorizontalEyeMapping() << "</td></tr>" << std::endl;
            os << "<tr><td>splitStereoHorizontalSeparation</td><td>" << object->getSplitStereoHorizontalSeparation() << "</td></tr>" << std::endl;
            os << "<tr><td>splitStereoVerticalEyeMapping</td><td>" << object->getSplitStereoVerticalEyeMapping() << "</td></tr>" << std::endl;
            os << "<tr><td>splitStereoVerticalSeparation</td><td>" << object->getSplitStereoVerticalSeparation() << "</td></tr>" << std::endl;
            os << "<tr><td>splitStereoAutoAdjustAspectRatio</td><td>" << (object->getSplitStereoAutoAdjustAspectRatio()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>screenWidth</td><td>" << object->getScreenWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>screenHeight</td><td>" << object->getScreenHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>screenDistance</td><td>" << object->getScreenDistance() << "</td></tr>" << std::endl;
            os << "<tr><td>doubleBuffer</td><td>" << (object->getDoubleBuffer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasRGB</td><td>" << (object->getRGB()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasDepthBuffer</td><td>" << (object->getDepthBuffer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasAlphaBuffer</td><td>" << (object->getAlphaBuffer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasStencilBuffer</td><td>" << (object->getStencilBuffer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasAccumBuffer</td><td>" << (object->getAccumBuffer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>hasMultiSamples</td><td>" << (object->getMultiSamples()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>minimumNumberAlphaBits</td><td>" << object->getMinimumNumAlphaBits() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumNumberStencilBits</td><td>" << object->getMinimumNumStencilBits() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumNumberAccumRedBits</td><td>" << object->getMinimumNumAccumRedBits() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumNumberAccumGreenBits</td><td>" << object->getMinimumNumAccumGreenBits() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumNumberAccumBlueBits</td><td>" << object->getMinimumNumAccumBlueBits() << "</td></tr>" << std::endl;
            os << "<tr><td>minimumNumberAccumAlphaBits</td><td>" << object->getMinimumNumAccumAlphaBits() << "</td></tr>" << std::endl;

            os << "<tr><td>maxNumberOfGraphicsContexts</td><td>" << object->getMaxNumberOfGraphicsContexts() << "</td></tr>" << std::endl;
            os << "<tr><td>numMultiSamples</td><td>" << object->getNumMultiSamples() << "</td></tr>" << std::endl;

            os << "<tr><td>compileContextsHint</td><td>" << (object->getCompileContextsHint()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>serializeDrawDispatch</td><td>" << (object->getSerializeDrawDispatch()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>useSceneViewForStereoHint</td><td>" << (object->getUseSceneViewForStereoHint()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>numOfDatabaseThreadsHint</td><td>" << object->getNumOfDatabaseThreadsHint() << "</td></tr>" << std::endl;
            os << "<tr><td>numOfHttpDatabaseThreadsHint</td><td>" << object->getNumOfHttpDatabaseThreadsHint() << "</td></tr>" << std::endl;
            os << "<tr><td>application</td><td>" << object->getApplication() << "</td></tr>" << std::endl;
            os << "<tr><td>maxTexturePoolSize</td><td>" << object->getMaxTexturePoolSize() << "</td></tr>" << std::endl;
            os << "<tr><td>maxBufferObjectPoolSize</td><td>" << object->getMaxBufferObjectPoolSize() << "</td></tr>" << std::endl;

            os << "<tr><td>implicitBufferAttachmentRenderMask</td><td>" << object->getImplicitBufferAttachmentRenderMask() << "</td></tr>" << std::endl;
            os << "<tr><td>implicitBufferAttachmentResolveMask</td><td>" << object->getImplicitBufferAttachmentResolveMask() << "</td></tr>" << std::endl;
            
            os << "<tr><td>getSwapMethod</td><td>" << object->getSwapMethod() << "</td></tr>" << std::endl;
            os << "<tr><td>GLContextVersion</td><td>" << object->getGLContextVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>GLContextFlags</td><td>" << object->getGLContextFlags() << "</td></tr>" << std::endl;
            os << "<tr><td>GLContextProfileMask</td><td>" << object->getGLContextProfileMask() << "</td></tr>" << std::endl;
            os << "<tr><td>hasKeystoneHint</td><td>" << (object->getKeystoneHint()?"true":"false") << "</td></tr>" << std::endl;
            
            os << "<tr><td>keystoneFileNames</td><td>" << object->getKeystoneFileNames().size() << " files</td></tr>" << std::endl;
            os << "<tr><td>keystones</td><td>" << object->getKeystones().size() << " objects</td></tr>" << std::endl;
            
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::NodeVisitor::VisitorType & t)
{
    switch(t)
    {
    case osg::NodeVisitor::NODE_VISITOR: os << "NODE_VISITOR"; break;
    case osg::NodeVisitor::CULL_VISITOR: os << "CULL_VISITOR"; break;
    case osg::NodeVisitor::UPDATE_VISITOR: os << "UPDATE_VISITOR"; break;
    case osg::NodeVisitor::EVENT_VISITOR: os << "EVENT_VISITOR"; break;
    case osg::NodeVisitor::COLLECT_OCCLUDER_VISITOR: os << "COLLECT_OCCLUDER_VISITOR"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::NodeVisitor::TraversalMode & t)
{
    switch(t)
    {
    case osg::NodeVisitor::TRAVERSE_NONE: os << "TRAVERSE_NONE"; break;
    case osg::NodeVisitor::TRAVERSE_PARENTS: os << "TRAVERSE_PARENTS"; break;
    case osg::NodeVisitor::TRAVERSE_ALL_CHILDREN: os << "TRAVERSE_ALL_CHILDREN"; break;
    case osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN: os << "TRAVERSE_ACTIVE_CHILDREN"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::NodeVisitor>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::NodeVisitor * object = getObject<osg::NodeVisitor,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);
            
            os << "<tr><td>visitorType</td><td>" << object->getVisitorType() << "</td></tr>" << std::endl;
            os << "<tr><td>traversalMode</td><td>" << object->getTraversalMode() << "</td></tr>" << std::endl;
            os << "<tr><td>traversalNumber</td><td>" << object->getTraversalNumber() << "</td></tr>" << std::endl;
            os << "<tr><td>traversalMask</td><td>" << castToEnumValueString<osgNodeMask>(object->getTraversalMask()) << "</td></tr>" << std::endl;
            os << "<tr><td>nodeMaskOverride</td><td>0x" << std::hex << object->getNodeMaskOverride() << std::dec << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Stats>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    StatsAccess * object = static_cast<StatsAccess*>(getObject<osg::Stats, SGIItemOsg>());
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>earliest frame</td><td>" << object->getEarliestFrameNumber() << "</td></tr>" << std::endl;
            os << "<tr><td>latest frame</td><td>" << object->getLatestFrameNumber() << "</td></tr>" << std::endl;

            osg::Stats::CollectMap stats;
            object->getCollectStats(stats);
            os << "<tr><td>collect stats</td><td><ul>";
            for (auto it = stats.begin(); it != stats.end(); ++it)
            {
                os << "<li>" << it->first << "=" << (it->second ? "true" : "false") << "</li>";
            }
            os << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStatsFrame:
        {
            unsigned frame = itemNumber();
            osg::Stats::AttributeMap attributes = object->getAttributeMap(frame);
            if (attributes.empty())
                os << "<i>N/A</i>";
            else
            {
                os << "<table border=\'1\' align=\'left\'><tr><th>Attribute</th><th>Value</th></tr>" << std::endl;
                for (auto it = attributes.begin(); it != attributes.end(); ++it)
                {
                    os << "<tr><td>" << it->first << "</td><td>" << it->second << "</td></tr>" << std::endl;
                }
                os << "</table>" << std::endl;
            }
            ret = true;
        }
        break;

    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::LOD::RangeMode & t)
{
    switch(t)
    {
    case osg::LOD::DISTANCE_FROM_EYE_POINT: os << "DISTANCE_FROM_EYE_POINT"; break;
    case osg::LOD::PIXEL_SIZE_ON_SCREEN: os << "PIXEL_SIZE_ON_SCREEN"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::LOD::CenterMode & t)
{
    switch(t)
    {
    case osg::LOD::USE_BOUNDING_SPHERE_CENTER: os << "USE_BOUNDING_SPHERE_CENTER"; break;
    case osg::LOD::USER_DEFINED_CENTER: os << "USER_DEFINED_CENTER"; break;
    case osg::LOD::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED: os << "UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::LOD>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::LOD * object = getObject<osg::LOD,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

            os << "<tr><td>rangeMode</td><td>" << object->getRangeMode() << "</td></tr>" << std::endl;
            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>centerMode</td><td>" << object->getCenterMode() << "</td></tr>" << std::endl;
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), object);
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::PagedLOD>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PagedLOD * object = getObject<osg::PagedLOD,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);


            os << "<tr><td>frameNumberOfLastTraversal</td><td>" << object->getFrameNumberOfLastTraversal() << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenThatCannotBeExpired</td><td>" << object->getNumChildrenThatCannotBeExpired() << "</td></tr>" << std::endl;
            os << "<tr><td>disableExternalChildrenPaging</td><td>" << (object->getDisableExternalChildrenPaging()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>databasePath</td><td>" << object->getDatabasePath() << "</td></tr>" << std::endl;
            os << "<tr><td>databaseOptions</td><td>" << osg_helpers::getObjectNameAndType(object->getDatabaseOptions()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::ProxyNode::LoadingExternalReferenceMode & t)
{
    switch(t)
    {
    case osg::ProxyNode::LOAD_IMMEDIATELY: os << "LOAD_IMMEDIATELY"; break;
    case osg::ProxyNode::DEFER_LOADING_TO_DATABASE_PAGER: os << "DEFER_LOADING_TO_DATABASE_PAGER"; break;
    case osg::ProxyNode::NO_AUTOMATIC_LOADING: os << "NO_AUTOMATIC_LOADING"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::ProxyNode::CenterMode & t)
{
    switch(t)
    {
    case osg::ProxyNode::USE_BOUNDING_SPHERE_CENTER: os << "USE_BOUNDING_SPHERE_CENTER"; break;
    case osg::ProxyNode::USER_DEFINED_CENTER: os << "USER_DEFINED_CENTER"; break;
    case osg::ProxyNode::UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED: os << "UNION_OF_BOUNDING_SPHERE_AND_USER_DEFINED"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::ProxyNode>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ProxyNode * object = getObject<osg::ProxyNode,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Node properties first
            callNextHandler(os);

            // skip the group properties because we include all group properties in the
            // here (with more information)
            // add remaining LOD properties
            os << "<tr><td>loading external ref mode</td><td>" << object->getLoadingExternalReferenceMode() << "</td></tr>" << std::endl;
            os << "<tr><td>centerMode</td><td>" << object->getCenterMode() << "</td></tr>" << std::endl;
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), object);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>databasePath</td><td>" << object->getDatabasePath() << "</td></tr>" << std::endl;
            os << "<tr><td>databaseOptions</td><td>" << osg_helpers::getObjectNameAndType(object->getDatabaseOptions()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::EllipsoidModel>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::EllipsoidModel * object = getObject<osg::EllipsoidModel,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>isWGS84</td><td>" << (object->isWGS84()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>radius equator</td><td>" << object->getRadiusEquator() << "</td></tr>" << std::endl;
            os << "<tr><td>radius polar</td><td>" << object->getRadiusPolar() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::CoordinateSystemNode>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::CoordinateSystemNode * object = getObject<osg::CoordinateSystemNode,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Group properties first
            callNextHandler(os);

            // add remaining properties
            os << "<tr><td>cs</td><td>" << object->getCoordinateSystem() << "</td></tr>" << std::endl;
            os << "<tr><td>ellipsoid</td><td>" << osg_helpers::getObjectNameAndType(object->getEllipsoidModel(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->getFormat() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

void writePrettyHTMLImplGroupChilds(osg::Group * object, std::basic_ostream<char>& os, unsigned selectedChild)
{
    bool onlyOneItem = (selectedChild != ~0u);
    if(!onlyOneItem)
        os << "<ol>";
    unsigned num_children = object->getNumChildren();
    if(osg::PagedLOD* pagedlod = dynamic_cast<osg::PagedLOD*>(object))
    {
        unsigned num_filenames = pagedlod->getNumFileNames();
        unsigned num_ranges = pagedlod->getNumRanges();
        unsigned num_items = std::max(num_children, std::max(num_filenames, num_ranges));
        for(unsigned n = 0; n < num_items; n++)
        {
            if(onlyOneItem && n != selectedChild)
                continue;
            const osg::Node * child = (n < num_children)?pagedlod->getChild(n):NULL;
            float minRange = (n < num_ranges)?pagedlod->getMinRange(n):0;
            float maxRange = (n < num_ranges)?pagedlod->getMaxRange(n):0;
            const std::string & filename = (n < num_filenames)?pagedlod->getFileName(n):std::string();
            osg::ref_ptr<osg::Referenced> req = (n < num_filenames)?pagedlod->getDatabaseRequest(n):NULL;
            float priorityOffset = (n < num_filenames)?pagedlod->getPriorityOffset(n):0;
            float priorityScale = (n < num_filenames)?pagedlod->getPriorityScale(n):0;
            double minExpiryTime = (n < num_filenames)?pagedlod->getPriorityScale(n):0;
            unsigned minExpiryFrames = (n < num_filenames)?pagedlod->getMinimumExpiryFrames(n):0;
            double timestamp = (n < num_filenames)?pagedlod->getTimeStamp(n):0;
            unsigned frameNo = (n < num_filenames)?pagedlod->getFrameNumber(n):0;

            if(!onlyOneItem)
                os << "<li>";
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>child</td><td>" << osg_helpers::getObjectNameAndType(child, true) << "</td></tr>" << std::endl;
            os << "<tr><td>range</td><td>" << minRange << "/" << maxRange << "</td></tr>" << std::endl;
            os << "<tr><td>filename</td><td>" << filename << "</td></tr>" << std::endl;
            os << "<tr><td>priorityOffset</td><td>" << priorityOffset << "</td></tr>" << std::endl;
            os << "<tr><td>priorityScale</td><td>" << priorityScale << "</td></tr>" << std::endl;
            os << "<tr><td>minExpiryFrames</td><td>" << minExpiryFrames << "</td></tr>" << std::endl;
            os << "<tr><td>minExpiryTime</td><td>" << minExpiryTime << "</td></tr>" << std::endl;
            os << "<tr><td>frameNo</td><td>" << frameNo << "</td></tr>" << std::endl;
            os << "<tr><td>timestamp</td><td>" << timestamp << "</td></tr>" << std::endl;
            os << "<tr><td>dbRequest</td><td>" << osg_helpers::getObjectNameAndType(req.get()) << "</td></tr>" << std::endl;
            os << "</table>";
            if(!onlyOneItem)
                os << "</li>";
        }
    }
    else if(osg::LOD* lod = dynamic_cast<osg::LOD*>(object))
    {
        unsigned num_ranges = lod->getNumRanges();
        unsigned num_items = std::max(num_children, num_ranges);
        for(unsigned n = 0; n < num_items; n++)
        {
            if(onlyOneItem && n != selectedChild)
                continue;
            const osg::Node * child = (n < num_children)?lod->getChild(n):NULL;
            float minRange = (n < num_ranges)?lod->getMinRange(n):0;
            float maxRange = (n < num_ranges)?lod->getMaxRange(n):0;
            if(!onlyOneItem)
                os << "<li>";
            os << "<li><table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>child</td><td>" << osg_helpers::getObjectNameAndType(child, true) << "</td></tr>" << std::endl;
            os << "<tr><td>range</td><td>" << minRange << "/" << maxRange << "</td></tr>" << std::endl;
            os << "</table>";
            if(!onlyOneItem)
                os << "</li>";
        }
    }
    else if(osg::ProxyNode * proxynode = dynamic_cast<osg::ProxyNode*>(object))
    {
        unsigned num_filenames = proxynode->getNumFileNames();
        unsigned num_items = std::max(num_children, num_filenames);
        for(unsigned n = 0; n < num_items; n++)
        {
            if(onlyOneItem && n != selectedChild)
                continue;
            const osg::Node * child = (n < num_children)?proxynode->getChild(n):NULL;
            const std::string & filename = (n < num_filenames)?proxynode->getFileName(n):std::string();
            osg::ref_ptr<osg::Referenced> req = (n < num_filenames)?proxynode->getDatabaseRequest(n):NULL;

            if(!onlyOneItem)
                os << "<li>";
            os << "<li><table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>child</td><td>" << osg_helpers::getObjectNameAndType(child, true) << "</td></tr>" << std::endl;
            os << "<tr><td>filename</td><td>" << filename << "</td></tr>" << std::endl;
            os << "<tr><td>dbRequest</td><td>" << osg_helpers::getObjectNameAndType(req.get()) << "</td></tr>" << std::endl;
            os << "</table>";
            if(!onlyOneItem)
                os << "</li>";
        }
    }
    else
    {
        for(unsigned n = 0; n < num_children; n++)
        {
            if(onlyOneItem && n != selectedChild)
                continue;
            const osg::Node * child = object->getChild(n);
            if(!onlyOneItem)
                os << "<li>";
            os << osg_helpers::getObjectNameAndType(child, true);
            if(!onlyOneItem)
                os << "</li>";
        }
    }
    if(!onlyOneItem)
        os << "</ol>" << std::endl;
}

bool writePrettyHTMLImpl<osg::Group>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Group * object = getObject<osg::Group,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

            // add remaining group properties
            os << "<tr><td>childs</td><td>";
            writePrettyHTMLImplGroupChilds(object, os, ~0u);
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            writePrettyHTMLImplGroupChilds(object, os, ~0u);
            ret = true;
        }
        break;
    case SGIItemTypePendingChild:
        {
            writePrettyHTMLImplGroupChilds(object, os, _item->number());
            ret = true;
        }
        break;
    default:
        // add node properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Shape>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Shape * object = getObject<osg::Shape,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Object properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Sphere>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Sphere * object = getObject<osg::Sphere,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;

            // add remaining Sphere properties
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Box>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Box * object = getObject<osg::Box,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;

            // add remaining Box properties
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>half length</td><td>" << object->getHalfLengths() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Cylinder>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Cylinder * object = getObject<osg::Cylinder,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;
            // add remaining Cylinder properties
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->getHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Cone>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Cone * object = getObject<osg::Cone,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;
            // add remaining Cone properties
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->getHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>base offset factor</td><td>" << object->getBaseOffsetFactor() << "</td></tr>" << std::endl;
            os << "<tr><td>base offset</td><td>" << object->getBaseOffset() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Capsule>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Capsule * object = getObject<osg::Capsule,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;
            // add remaining Capsule properties
            os << "<tr><td>center</td><td>";
            writePrettyPositionHTML(os, object->getCenter(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>radius</td><td>" << object->getRadius() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->getHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::InfinitePlane>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::InfinitePlane * object = getObject<osg::InfinitePlane,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            // add remaining InfinitePlane properties
            os << "<tr><td>normal</td><td>" << object->getNormal() << "</td></tr>" << std::endl;
            os << "<tr><td>asVec4</td><td>" << object->asVec4() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::TriangleMesh>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::TriangleMesh * object = getObject<osg::TriangleMesh,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            // add remaining TriangleMesh properties
            os << "<tr><td>vertices</td><td>";
            osg::Vec3Array * vertices = object->getVertices();
            if(vertices)
                os << vertices->getNumElements() << " elements";
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>vertices</td><td>";
            osg::IndexArray * indices = object->getIndices();
            if(indices)
                os << indices->getNumElements() << " elements";
            else
                os << "(null)";
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::HeightField>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::HeightField * object = getObject<osg::HeightField,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            const osg::Drawable * refDrawable = NULL;
            // add remaining HeightField properties
            os << "<tr><td>rows</td><td>" << object->getNumRows() << "</td></tr>" << std::endl;
            os << "<tr><td>columns</td><td>" << object->getNumColumns() << "</td></tr>" << std::endl;
            os << "<tr><td>origin</td><td>";
            writePrettyPositionHTML(os, object->getOrigin(), refDrawable);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>rotation</td><td>";
            writePrettyHTML(os, object->getRotation());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>x interval</td><td>" << object->getXInterval() << "</td></tr>" << std::endl;
            os << "<tr><td>y interval</td><td>" << object->getYInterval() << "</td></tr>" << std::endl;
            os << "<tr><td>skirtHeight</td><td>" << object->getSkirtHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>borderWidth</td><td>" << object->getBorderWidth() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
	case SGIItemTypeArrayData:
		{
			osg_helpers::heightFieldDumpHTML(os, object);
			ret = true;
		}
		break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::CompositeShape>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::CompositeShape * object = getObject<osg::CompositeShape,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Shape properties first
            callNextHandler(os);

            // add remaining CompositeShape properties
            os << "<tr><td>children</td><td><ol>";
            for(unsigned n = 0; n < object->getNumChildren(); n++)
            {
                osg::Shape * child = object->getChild(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(child, true) << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            os << "<ol>";
            for(unsigned n = 0; n < object->getNumChildren(); n++)
            {
                osg::Shape * child = object->getChild(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(child, true) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ConvexHull>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ConvexHull * object = getObject<osg::ConvexHull,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add TriangleMesh properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // add Shape properties first
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

namespace {
    class DrawableAccess : public osg::Drawable
    {
    public:
        const osg::BoundingBox& getBoundingBoxNoCompute() const {
            return _boundingBox;
        }
        unsigned int getNumChildrenRequiringUpdateTraversal() const { return _numChildrenRequiringUpdateTraversal; }
        unsigned int getNumChildrenRequiringEventTraversal() const { return _numChildrenRequiringEventTraversal; }
    };
}

bool writePrettyHTMLImpl<osg::Drawable>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Drawable * object = getObject<osg::Drawable,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining drawable properties
            DrawableAccess * access = (DrawableAccess *)object;
            os << "<tr><td>bounding box</td><td>" << access->getBoundingBoxNoCompute() << "</td></tr>" << std::endl;

            os << "<tr><td>supports display list</td><td>" << (object->getSupportsDisplayList()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>use display list</td><td>" << (object->getUseDisplayList()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>use VBO</td><td>" << (object->getUseVertexBufferObjects()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>use VAO</td><td>" << (object->getUseVertexArrayObject() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>shape</td><td>" << osg_helpers::getObjectNameAndType(object->getShape(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenRequiringUpdateTraversal</td><td>" << access->getNumChildrenRequiringUpdateTraversal() << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenRequiringEventTraversal</td><td>" << access->getNumChildrenRequiringEventTraversal() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>" << std::endl;
            for(unsigned n = 0; n < object->getNumParents(); n++)
            {
                const osg::Node * parent = object->getParent(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ShapeDrawable>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ShapeDrawable * object = getObject<osg::ShapeDrawable,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Drawable properties first
            callNextHandler(os);

            // add remaining ShapeDrawable properties
            os << "<tr><td>color</td><td>"
                << vec4fToHtmlColor(object->getColor())
                << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

void writePrettyHTMLGeometryVertices(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief)
{
    const osg::Vec3Array * vertices = (object!=NULL)?(const osg::Vec3Array *)object->getVertexArray():NULL;
    os << (vertices?vertices->size():0) << " vertices";
    if(vertices && !brief)
    {
        os << "<ol>" << std::endl;
        for(unsigned n = 0; n < vertices->size(); n++)
        {
            os << "<li>";
            const osg::Vec3 & v = (*vertices)[n];
            writePrettyHTML(os, v);
            os << "</li>";
        }
        os << "</ol>" << std::endl;
    }
}

void writePrettyHTMLGeometryNormals(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief)
{
    const osg::Vec3Array * normals = (object!=NULL)?(const osg::Vec3Array *)object->getNormalArray():NULL;
    os << (normals?normals->size():0) << " normals";
    if(normals && !brief)
    {
        os << "<br/><ol>" << std::endl;
        for(unsigned n = 0; n < normals->size(); n++)
        {
            os << "<li>";
            const osg::Vec3 & v = (*normals)[n];
            writePrettyHTML(os, v);
            os << "</li>";
        }
        os << "</ol>" << std::endl;
    }
}

void writePrettyHTMLColorArray(std::basic_ostream<char>& os, const osg::Array * colors_, bool brief)
{
    const osg::Vec4Array * colors = (const osg::Vec4Array *)colors_;
    os << (colors?colors->size():0) << " colors";
    if(colors && !brief)
    {
        os << "<br/><ol>" << std::endl;
        for(unsigned n = 0; n < colors->size(); n++)
        {
            os << "<li>"
                << vec4fToHtmlColor((*colors)[n])
                << "</li>";
        }
        os << "</ol>" << std::endl;
    }
}

#if 0
void writePrettyHTMLGeometryVertexIndicies(std::basic_ostream<char>& os, const osg::Geometry * object, bool brief)
{
    const osg::Vec3Array * vertices = (object!=NULL)?(const osg::Vec3Array *)object->getVertexArray():NULL;
    const osg::IndexArray * indices = (object!=NULL)?object->getVertexIndices():NULL;
    os << (indices?indices->getNumElements():0) << " indices";
    if(indices && !brief)
    {
        os << "<br/><ol>" << std::endl;
        for(unsigned n = 0; n < indices->getNumElements(); n++)
        {
            os << "<li>";
            unsigned idx = indices->index(n);
            if(vertices)
            {
                os << idx << "&nbsp;@&nbsp;";
                const osg::Vec3 & v = (*vertices)[idx];
                writePrettyHTML(os, v);
            }
            else
            {
                os << idx << " (no vertices)";
            }
            os << "</li>";
        }
        os << "</ol>" << std::endl;
    }
}
#endif // 0

void writePrettyHTMLGeometryTexCoords(std::basic_ostream<char>& os, const osg::Geometry * object, int number, bool brief)
{
    for(unsigned textureUnit = 0; textureUnit < object->getNumTexCoordArrays(); textureUnit++)
    {
        if(textureUnit == number || number < 0)
        {
            const osg::Vec2Array * texcoords = (const osg::Vec2Array *)object->getTexCoordArray(textureUnit);
            size_t numcoords = (texcoords?texcoords->size():0);
            os << helpers::str_plus_number("Texture", textureUnit) << " " << numcoords << " elements<br/>" << std::endl;
            if(!brief)
            {
                os << "<ol>";
                for(unsigned n = 0; n < numcoords; n++)
                {
                    os << "<li>";
                    const osg::Vec2 & v = (*texcoords)[n];
                    writePrettyHTML(os, v);
                    os << "</li>";
                }
                os << "</ol>" << std::endl;
            }
        }
    }
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Geometry::AttributeBinding & t)
{
    switch(t)
    {
    case osg::Geometry::BIND_OFF: os << "off"; break;
    case osg::Geometry::BIND_PER_PRIMITIVE_SET: os << "per primitive set"; break;
    //case osg::Geometry::BIND_PER_PRIMITIVE: os << "per primitive"; break;
    case osg::Geometry::BIND_PER_VERTEX: os << "per vertex"; break;
    case osg::Geometry::BIND_OVERALL: os << "overall"; break;
    default: os << "unknown" << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Geometry>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Geometry * object = getObject<osg::Geometry,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add drawable properties first
            callNextHandler(os);

			os << "<tr><td>contains deprecated data</td><td>" << (object->containsDeprecatedData() ? "true" : "false") << "</td></tr>" << std::endl;

            os << "<tr><td>vertices</td><td>";
            writePrettyHTMLGeometryVertices(os, object, true);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>normal binding</td><td>" << object->getNormalBinding() << "</td></tr>" << std::endl;
            os << "<tr><td>normals</td><td>";
            writePrettyHTMLGeometryNormals(os, object, true);
            os << "</td></tr>" << std::endl;

            unsigned numTexCoordArrays = object->getNumTexCoordArrays();
            os << "<tr><td>num tex coord arrays</td><td>" << numTexCoordArrays << "</td></tr>" << std::endl;
            if(numTexCoordArrays != 0)
            {
                os << "<tr><td>texture coords</td><td>";
                writePrettyHTMLGeometryTexCoords(os, object, -1, true);
                os << "</td></tr>" << std::endl;
            }

            os << "<tr><td>color binding</td><td>" << object->getColorBinding() << "</td></tr>" << std::endl;
            os << "<tr><td>colors</td><td>";
            writePrettyHTMLColorArray(os, object->getColorArray(), true);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>2nd color binding</td><td>" << object->getSecondaryColorBinding() << "</td></tr>" << std::endl;
            os << "<tr><td>2nd colors</td><td>";
            writePrettyHTMLColorArray(os, object->getSecondaryColorArray(), true);
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDrawableVertexAttribArrays:
        {
            os << "<ol>";
            unsigned numVertexAttribArrays = object->getNumVertexAttribArrays();
            for (unsigned i = 0; i < numVertexAttribArrays; ++i)
            {
                osg::Array * attr = object->getVertexAttribArray(i);
                os << "<li>" << helpers::str_plus_number("VertexAttribArray", i) << ':';
                if (attr)
                {
                    os << " size=" << attr->getNumElements();
                    os << " type=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(attr->getDataType());
                }
                os << ' ' << osg_helpers::getObjectNameAndType(attr, true);
                os << "</li>";
            }
            os << "</ol>";
            ret = true;
    }
        break;
    case SGIItemTypeDrawableTexCoordsList:
        {
            os << "<ol>";
            unsigned numTexCoordArrays = object->getNumTexCoordArrays();
            for(unsigned textureUnit = 0; textureUnit < numTexCoordArrays; textureUnit++)
            {
                osg::Array * texcoords = object->getTexCoordArray(textureUnit);
                os << "<li>" << helpers::str_plus_number("Texture", textureUnit) << ':';
                if (texcoords)
                {
                    os << " size=" << texcoords->getNumElements();
                    os << " type=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(texcoords->getDataType());
                }
                os << ' ' << osg_helpers::getObjectNameAndType(texcoords, true);
                os << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypePrimitiveSetList:
        {
            os << "<ol>";
            unsigned numPrimitiveSets = object->getNumPrimitiveSets();
            for(unsigned i = 0; i < numPrimitiveSets; i++)
            {
                os << "<li>";
                osg::PrimitiveSet * primset = object->getPrimitiveSet(i);
                if (primset)
                {
                    os << " mode=" << (osg::PrimitiveSet::Mode)primset->getMode();
                    os << " type=" << primset->getType();
                    os << " num=" << primset->getNumIndices();
                    os << " ";
                }
                os << osg_helpers::getObjectNameAndType(primset, true) << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Uniform::Type & t)
{
    return os << osg::Uniform::getTypename(t);
}

bool writePrettyHTMLImpl<osg::Uniform>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Uniform * object = getObject<osg::Uniform, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            // add remaining Uniform properties
            os << "<tr><td>type</td><td>" << object->getType() << "</td></tr>" << std::endl;
            os << "<tr><td>numElements</td><td>" << object->getNumElements() << "</td></tr>" << std::endl;
            os << "<tr><td>internalArrayNumElements</td><td>" << object->getInternalArrayNumElements() << "</td></tr>" << std::endl;
            os << "<tr><td>modifiedCount</td><td>" << object->getModifiedCount() << "</td></tr>" << std::endl;
            os << "<tr><td>nameID</td><td>" << object->getNameID() << "</td></tr>" << std::endl;
            
            os << "<tr><td>data</td><td>" << osg_helpers::uniformToHTML(object) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>" << std::endl;
            for(unsigned n = 0; n < object->getNumParents(); n++)
            {
                const osg::Object * parent = object->getParent(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>update callback</td><td>" << osg_helpers::getObjectNameAndType(object->getUpdateCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>event callback</td><td>" << osg_helpers::getObjectNameAndType(object->getEventCallback(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::BufferData>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BufferData * object = getObject<osg::BufferData,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Object properties first
            callNextHandler(os);

            // add remaining BufferData properties
            os << "<tr><td>bufferIndex</td><td>" << object->getBufferIndex() << "</td></tr>" << std::endl;
            os << "<tr><td>modifiedCount</td><td>" << object->getModifiedCount() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>modified callback</td><td>" << osg_helpers::getObjectNameAndType(object->getModifiedCallback(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeArrayData:
        {
            os << "<pre>";
            sgi::helpers::hexDumpMemory(os, object->getDataPointer(), object->getTotalDataSize());
            os << "</pre>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::PrimitiveSet::Type & t)
{
    switch(t)
    {
    case osg::PrimitiveSet::PrimitiveType: os << "PrimitiveType"; break;
    case osg::PrimitiveSet::DrawArraysPrimitiveType: os << "DrawArraysPrimitiveType"; break;
    case osg::PrimitiveSet::DrawArrayLengthsPrimitiveType: os << "DrawArrayLengthsPrimitiveType"; break;
    case osg::PrimitiveSet::DrawElementsUBytePrimitiveType: os << "DrawElementsUBytePrimitiveType"; break;
    case osg::PrimitiveSet::DrawElementsUShortPrimitiveType: os << "DrawElementsUShortPrimitiveType"; break;
    case osg::PrimitiveSet::DrawElementsUIntPrimitiveType: os << "DrawElementsUIntPrimitiveType"; break;
    default: os << "unknown" << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::PrimitiveSet::Mode & t)
{
    switch (t)
    {
    case osg::PrimitiveSet::POINTS: os << "GL_POINTS"; break;
    case osg::PrimitiveSet::LINES: os << "GL_LINES"; break;
    case osg::PrimitiveSet::LINE_STRIP: os << "GL_LINE_STRIP"; break;
    case osg::PrimitiveSet::LINE_LOOP: os << "GL_LINE_LOOP"; break;
    case osg::PrimitiveSet::TRIANGLES: os << "GL_TRIANGLES"; break;
    case osg::PrimitiveSet::TRIANGLE_STRIP: os << "GL_TRIANGLE_STRIP"; break;
    case osg::PrimitiveSet::TRIANGLE_FAN: os << "GL_TRIANGLE_FAN"; break;
    case osg::PrimitiveSet::QUADS: os << "GL_QUADS"; break;
    case osg::PrimitiveSet::QUAD_STRIP: os << "GL_QUAD_STRIP"; break;
    case osg::PrimitiveSet::POLYGON: os << "GL_POLYGON"; break;
    case osg::PrimitiveSet::LINES_ADJACENCY: os << "GL_LINES_ADJACENCY"; break;
    case osg::PrimitiveSet::TRIANGLES_ADJACENCY: os << "GL_TRIANGLES_ADJACENCY"; break;
    case osg::PrimitiveSet::TRIANGLE_STRIP_ADJACENCY: os << "GL_TRIANGLE_STRIP_ADJACENCY"; break;
    case osg::PrimitiveSet::PATCHES: os << "GL_PATCHES"; break;
    default: os << "unknown" << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::PrimitiveSet>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PrimitiveSet * object = getObject<osg::PrimitiveSet,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add BufferData properties first
            callNextHandler(os);

            // add remaining PrimitiveSet properties
            os << "<tr><td>type</td><td>" << object->getType() << "</td></tr>" << std::endl;
            os << "<tr><td>numInstances</td><td>" << object->getNumInstances() << "</td></tr>" << std::endl;
            os << "<tr><td>mode</td><td>" << (osg::PrimitiveSet::Mode)object->getMode() << "</td></tr>" << std::endl;
            os << "<tr><td>data ptr</td><td>" << (void*)object->getDataPointer() << "</td></tr>" << std::endl;
            os << "<tr><td>total data size</td><td>" << object->getTotalDataSize() << "</td></tr>" << std::endl;
            os << "<tr><td>supportsBufferObject</td><td>" << (object->supportsBufferObject()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>numPrimitives</td><td>" << object->getNumPrimitives() << "</td></tr>" << std::endl;
            os << "<tr><td>numIndices</td><td>" << object->getNumIndices() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDrawElementsIndicies:
        {
            os << "<ol start=\"0\">";
            unsigned numIndices = object->getNumIndices();
            for(unsigned i = 0; i < numIndices; ++i)
            {
                unsigned idx = object->index(i);
                os << "<li>" << idx << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
bool writePrettyHTMLImpl<osg::DrawElements>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawElements * object = getObject<osg::DrawElements,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add PrimitiveSet properties first
            callNextHandler(os);

            // add remaining shader properties

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDrawElementsIndicies:
        {
            os << "<ol start=\"0\">";
            unsigned numIndices = object->getNumIndices();
            for(unsigned i = 0; i < numIndices; ++i)
            {
                unsigned idx = object->index(i);
                os << "<li>" << idx << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DrawElementsUByte>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawElementsUByte * object = getObject<osg::DrawElementsUByte,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add DrawElements properties first
            callNextHandler(os);

            // add remaining DrawElementsUByte properties
            os << "<tr><td>getNumIndices</td><td>" << object->getNumIndices() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DrawElementsUShort>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawElementsUShort * object = getObject<osg::DrawElementsUShort,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add DrawElements properties first
            callNextHandler(os);

            // add remaining DrawElementsUShort properties
            os << "<tr><td>getNumIndices</td><td>" << object->getNumIndices() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DrawElementsUInt>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawElementsUInt * object = getObject<osg::DrawElementsUInt,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add DrawElements properties first
            callNextHandler(os);

            // add remaining DrawElementsUInt properties
            os << "<tr><td>getNumIndices</td><td>" << object->getNumIndices() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DrawArrays>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawArrays * object = getObject<osg::DrawArrays,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add DrawElements properties first
            callNextHandler(os);

            // add remaining DrawArrays properties
            os << "<tr><td>first</td><td>" << object->getFirst() << "</td></tr>" << std::endl;
            os << "<tr><td>count</td><td>" << object->getCount() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::DrawArrayLengths>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::DrawArrayLengths * object = getObject<osg::DrawArrayLengths,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add DrawElements properties first
            callNextHandler(os);

            // add remaining DrawArrayLengths properties
            os << "<tr><td>first</td><td>" << object->getFirst() << "</td></tr>" << std::endl;
            os << "<tr><td>numPrimitives</td><td>" << object->getNumPrimitives() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Shader>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Shader * object = getObject<osg::Shader,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining shader properties
            os << "<tr><td>type</td><td>" << object->getTypename() << "</td></tr>" << std::endl;
            os << "<tr><td>filename</td><td>" << object->getFileName() << "</td></tr>" << std::endl;
            os << "<tr><td>source</td><td>";
            const std::string & source = object->getShaderSource();
            if(source.empty())
                os << "&lt;empty&gt;";
            else
                os << source.size() << " chars";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>code injection map</td><td>";
            const osg::Shader::CodeInjectionMap& codeinjmap = object->getCodeInjectionMap();
            if(codeinjmap.empty())
                os << "&lt;empty&gt;";
            else
            {
                os << "<ul>";
                for(osg::Shader::CodeInjectionMap::const_iterator it = codeinjmap.begin(); it != codeinjmap.end(); it++)
                {
                    const float & position = it->first;
                    const std::string & code = it->second;
                    os << "<li>" << position << "<br/><pre>" << helpers::html_encode(code) << "</pre></li>" << std::endl;
                }
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaderSource:
        {
            os << "<b>" << object->getTypename() << ":</b><br/>";
            os << "<pre>" << std::endl;
            os << helpers::html_encode(object->getShaderSource());
            os << "</pre>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaderCodeInjectionMap:
        {
            os << "<ul>";
            const osg::Shader::CodeInjectionMap & codeInjectionMap = object->getCodeInjectionMap();
            for(osg::Shader::CodeInjectionMap::const_iterator it = codeInjectionMap.begin(); it != codeInjectionMap.end(); it++)
            {
                float position = it->first;
                const std::string & code = it->second;
                os << "<li>" << position << "<br/><pre>" << helpers::html_encode(code) << "</pre></li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ClipNode>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ClipNode * object = getObject<osg::ClipNode,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Group properties first
            callNextHandler(os);

            // add remaining ClipNode properties
            os << "<tr><td>ref frame</td><td>" << object->getReferenceFrame() << "</td></tr>" << std::endl;
            os << "<tr><td>num planes</td><td>" << object->getNumClipPlanes() << "</td></tr>" << std::endl;
            os << "<tr><td>planes</td><td><ol>";
            const osg::ClipNode::ClipPlaneList & list = object->getClipPlaneList();
            for(osg::ClipNode::ClipPlaneList::const_iterator it = list.begin(); it != list.end(); it++)
            {
                const osg::ref_ptr<osg::ClipPlane> clipplane = *it;
                os << "<li>" << osg_helpers::getObjectNameAndType(clipplane.get(), true) << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::CullFace::Mode & t)
{
    switch (t)
    {
    case osg::CullFace::FRONT: os << "FRONT"; break;
    case osg::CullFace::BACK: os << "BACK"; break;
    case osg::CullFace::FRONT_AND_BACK: os << "FRONT_AND_BACK"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::CullFace>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::CullFace * object = getObject<osg::CullFace,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Group properties first
            callNextHandler(os);

            // add remaining CullFace properties
            os << "<tr><td>mode</td><td>" << object->getMode() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
bool writePrettyHTMLImpl<OpenThreads::Thread>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    /// @todo re-implement writePrettyHTMLImpl OpenThreads::Thread
    OpenThreads::Thread * object = NULL; // static_cast<OpenThreads::Thread,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>running</td><td>" << (const_cast<OpenThreads::Thread *>(object)->isRunning()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>pid</td><td>" << const_cast<OpenThreads::Thread *>(object)->getProcessId() << "</td></tr>" << std::endl;
            os << "<tr><td>tid</td><td>" << const_cast<OpenThreads::Thread *>(object)->getThreadId() << "</td></tr>" << std::endl;
            os << "<tr><td>schedulePriority</td><td>" << const_cast<OpenThreads::Thread *>(object)->getSchedulePriority() << "</td></tr>" << std::endl;
            os << "<tr><td>schedulePolicy</td><td>" << const_cast<OpenThreads::Thread *>(object)->getSchedulePolicy() << "</td></tr>" << std::endl;
            os << "<tr><td>stackSize</td><td>" << const_cast<OpenThreads::Thread *>(object)->getStackSize() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Image::WriteHint & t)
{
    switch(t)
    {
    case osg::Image::NO_PREFERENCE: os << "NO_PREFERENCE"; break;
    case osg::Image::STORE_INLINE: os << "STORE_INLINE"; break;
    case osg::Image::EXTERNAL_FILE: os << "EXTERNAL_FILE"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Image::AllocationMode & t)
{
    switch(t)
    {
    case osg::Image::NO_DELETE: os << "NO_DELETE"; break;
    case osg::Image::USE_NEW_DELETE: os << "USE_NEW_DELETE"; break;
    case osg::Image::USE_MALLOC_FREE: os << "USE_MALLOC_FREE"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Image::Origin  & t)
{
    switch(t)
    {
    case osg::Image::BOTTOM_LEFT: os << "BOTTOM_LEFT"; break;
    case osg::Image::TOP_LEFT: os << "TOP_LEFT"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Image>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Image * object = getObject<osg::Image,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining image properties
            os << "<tr><td>filename</td><td>" << object->getFileName() << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << object->s() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->t() << "</td></tr>" << std::endl;
            os << "<tr><td>depth</td><td>" << object->r() << "</td></tr>" << std::endl;
            os << "<tr><td>writeHint</td><td>" << object->getWriteHint() << "</td></tr>" << std::endl;
            os << "<tr><td>allocationMode</td><td>" << object->getAllocationMode() << "</td></tr>" << std::endl;
            os << "<tr><td>origin</td><td>" << object->getOrigin() << "</td></tr>" << std::endl;

            os << "<tr><td>rowLength</td><td>" << object->getRowLength() << "</td></tr>" << std::endl;
            os << "<tr><td>internalTextureFormat</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getInternalTextureFormat()) << "</td></tr>" << std::endl;
            os << "<tr><td>isCompressedInternalFormat</td><td>" << (osg::Texture::isCompressedInternalFormat(object->getInternalTextureFormat())?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>pixelFormat</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getPixelFormat()) << "</td></tr>" << std::endl;
            os << "<tr><td>dataType</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDataType()) << "</td></tr>" << std::endl;
            os << "<tr><td>packing</td><td>" << object->getPacking() << "</td></tr>" << std::endl;
            os << "<tr><td>compressed</td><td>" << (object->isCompressed()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>pixelAspectRatio</td><td>" << object->getPixelAspectRatio() << "</td></tr>" << std::endl;
            os << "<tr><td>pixelSizeInBits</td><td>" << object->getPixelSizeInBits() << "</td></tr>" << std::endl;
            os << "<tr><td>rowSizeInBytes</td><td>" << object->getRowSizeInBytes() << "</td></tr>" << std::endl;
            os << "<tr><td>rowStepInBytes</td><td>" << object->getRowStepInBytes() << "</td></tr>" << std::endl;
            os << "<tr><td>imageSizeInBytes</td><td>" << object->getImageSizeInBytes() << "</td></tr>" << std::endl;
            os << "<tr><td>totalSizeInBytes</td><td>" << object->getTotalSizeInBytes() << "</td></tr>" << std::endl;
            os << "<tr><td>totalSizeInBytesIncludingMipmaps</td><td>" << object->getTotalSizeInBytesIncludingMipmaps() << "</td></tr>" << std::endl;
            os << "<tr><td>isDataContiguous</td><td>" << (object->isDataContiguous()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isMipmap</td><td>" << (object->isMipmap()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>numMipmapLevels</td><td>" << object->getNumMipmapLevels() << "</td></tr>" << std::endl;

            os << "<tr><td>supportsTextureSubloading</td><td>" << (object->supportsTextureSubloading()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isImageTranslucent</td><td>" << (object->isImageTranslucent()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>requiresUpdateCall</td><td>" << (object->requiresUpdateCall()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>pixelBufferObject</td><td>" << osg_helpers::getObjectNameAndType(object->getPixelBufferObject(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeImageMipMap:
        {
            if (itemNumber() == ~0u)
            {
                os << "<ol>";
                for (unsigned i = 0; i < object->getNumMipmapLevels(); ++i)
                {
                    unsigned offset = object->getMipmapOffset(i);
                    unsigned next_offset = (i + 1 < object->getNumMipmapLevels()) ? object->getMipmapOffset(i + 1) : object->getTotalSizeInBytesIncludingMipmaps();
                    unsigned size = next_offset - offset;
                    os << "<li>" << helpers::str_plus_number("Level", i) << " offset=" << offset << " size=" << size << "</li>" << std::endl;
                }
                os << "</ol>";
            }
            else
            {
                unsigned i = itemNumber();
                if (_table)
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

                unsigned offset = object->getMipmapOffset(i);
                unsigned next_offset = (i + 1 < object->getNumMipmapLevels()) ? object->getMipmapOffset(i + 1) : object->getTotalSizeInBytesIncludingMipmaps();
                unsigned size = next_offset - offset;
                
                os << "<tr><td>Level</td><td>" << i << "</td></tr>" << std::endl;
                os << "<tr><td>offset</td><td>" << offset << "</td></tr>" << std::endl;
                os << "<tr><td>size</td><td>" << size << "</td></tr>" << std::endl;

                if (_table)
                    os << "</table>" << std::endl;
            }
            ret = true;
        }
        break;

    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::ImageStream::StreamStatus  & t)
{
    switch (t)
    {
    case osg::ImageStream::INVALID: os << "INVALID"; break;
    case osg::ImageStream::PLAYING: os << "PLAYING"; break;
    case osg::ImageStream::PAUSED: os << "PAUSED"; break;
    case osg::ImageStream::REWINDING: os << "REWINDING"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::ImageStream::LoopingMode  & t)
{
    switch (t)
    {
    case osg::ImageStream::NO_LOOPING: os << "NO_LOOPING"; break;
    case osg::ImageStream::LOOPING: os << "LOOPING"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::ImageStream>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ImageStream * object = getObject<osg::ImageStream, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            os << "<tr><td>status</td><td>" << object->getStatus() << "</td></tr>" << std::endl;
            os << "<tr><td>loopingMode</td><td>" << object->getLoopingMode() << "</td></tr>" << std::endl;

            os << "<tr><td>creationTime</td><td>" << object->getCreationTime() << "</td></tr>" << std::endl;
            os << "<tr><td>length</td><td>" << object->getLength() << "</td></tr>" << std::endl;
            os << "<tr><td>frameRate</td><td>" << object->getFrameRate() << "</td></tr>" << std::endl;
            os << "<tr><td>currentTime</td><td>" << object->getCurrentTime() << "</td></tr>" << std::endl;
            os << "<tr><td>referenceTime</td><td>" << object->getReferenceTime() << "</td></tr>" << std::endl;
            os << "<tr><td>timeMultiplier</td><td>" << object->getTimeMultiplier() << "</td></tr>" << std::endl;
            os << "<tr><td>volume</td><td>" << object->getVolume() << "</td></tr>" << std::endl;
            os << "<tr><td>audioBalance</td><td>" << object->getAudioBalance() << "</td></tr>" << std::endl;

            os << "<tr><td>audioStreams</td><td><ol>";
            for (auto s : object->getAudioStreams())
                os << "<li>" << osg_helpers::getObjectNameAndType(s.get()) << "</li>";
            os << "<ol></td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::ImageSequence::Mode  & t)
{
    switch (t)
    {
    case osg::ImageSequence::PRE_LOAD_ALL_IMAGES: os << "PRE_LOAD_ALL_IMAGES"; break;
    case osg::ImageSequence::PAGE_AND_RETAIN_IMAGES: os << "PAGE_AND_RETAIN_IMAGES"; break;
    case osg::ImageSequence::PAGE_AND_DISCARD_USED_IMAGES: os << "PAGE_AND_DISCARD_USED_IMAGES"; break;
    case osg::ImageSequence::LOAD_AND_RETAIN_IN_UPDATE_TRAVERSAL: os << "LOAD_AND_RETAIN_IN_UPDATE_TRAVERSAL"; break;
    case osg::ImageSequence::LOAD_AND_DISCARD_IN_UPDATE_TRAVERSAL: os << "LOAD_AND_DISCARD_IN_UPDATE_TRAVERSAL"; break;
    default: os << (int)t; break;
    }
    return os;
}


bool writePrettyHTMLImpl<osg::ImageSequence>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ImageSequence* object = getObject<osg::ImageSequence, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            os << "<tr><td>mode</td><td>" << object->getMode() << "</td></tr>" << std::endl;
            os << "<tr><td>readOptions</td><td>" << osg_helpers::getObjectNameAndType(object->getReadOptions()) << "</td></tr>" << std::endl;

            os << "<tr><td>imageData</td><td><ol>";
            for (const auto & imgdata : object->getImageDataList())
            {
                os << "<li>" 
                    << imgdata._filename 
                    << " img=" << osg_helpers::getObjectNameAndType(imgdata._image.get()) 
                    << " req=" << osg_helpers::getObjectNameAndType(imgdata._imageRequest.get())
                    << "</li>";
            }
            os << "<ol></td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


#if 0
void writePrettyStateAttributeHTML(std::basic_ostream<char>& os, const osg::StateAttribute * object, bool table, bool brief)
{
    switch(object->getType())
    {
    case osg::StateAttribute::TEXTURE:
        {
            const osg::Texture1D * texture1d = dynamic_cast<const osg::Texture1D*>(object);
            const osg::Texture2D * texture2d = dynamic_cast<const osg::Texture2D*>(object);
            const osg::Texture3D * texture3d = dynamic_cast<const osg::Texture3D*>(object);

            if(texture1d)
                writePrettyHTML(os, texture1d, true, brief);
            else if(texture2d)
                writePrettyHTML(os, texture2d, true, brief);
            else if(texture3d)
                writePrettyHTML(os, texture3d, true, brief);
            else
                writePrettyHTML(os, (const osg::Texture*)object, true, brief);
        }
        break;
    case osg::StateAttribute::POLYGONMODE:
        writePrettyHTML(os, (const osg::PolygonMode*)object, table);
        break;
    case osg::StateAttribute::POLYGONOFFSET:
        writePrettyHTML(os, (const osg::PolygonOffset*)object, table);
        break;
    case osg::StateAttribute::MATERIAL:
        writePrettyHTML(os, (const osg::Material*)object, table);
        break;
    case osg::StateAttribute::LIGHT:
        writePrettyHTML(os, (const osg::Light*)object, table);
        break;
    case osg::StateAttribute::LIGHTMODEL:
        writePrettyHTML(os, (const osg::LightModel*)object, table);
        break;
    case osg::StateAttribute::TEXENV:
        writePrettyHTML(os, (const osg::TexEnv*)object, table);
        break;
    case osg::StateAttribute::TEXGEN:
        writePrettyHTML(os, (const osg::TexGen*)object, table);
        break;
    case osg::StateAttribute::STENCIL:
        writePrettyHTML(os, (const osg::Stencil*)object, table);
        break;
    case osg::StateAttribute::VIEWPORT:
        writePrettyHTML(os, (const osg::Viewport*)object, table);
        break;
    case osg::StateAttribute::CLIPPLANE:
        writePrettyHTML(os, (const osg::ClipPlane*)object, table);
        break;
    case osg::StateAttribute::BLENDFUNC:
        writePrettyHTML(os, (const osg::BlendFunc*)object, table);
        break;
    case osg::StateAttribute::VERTEXPROGRAM:
    case osg::StateAttribute::FRAGMENTPROGRAM:
    case osg::StateAttribute::PROGRAM:
        {
            const osg::Program * program = dynamic_cast<const osg::Program*>(object);
            /*
            const osgEarth::VirtualProgram * virtualprogram = dynamic_cast<const osgEarth::VirtualProgram*>(object);
            if(virtualprogram)
                writePrettyHTML(os, virtualprogram, table);
            else */if(program)
                writePrettyHTML(os, program, table);
            else
                writePrettyHTML(os, (const osg::StateAttribute*)object, table);
        }
        break;
    case osg::StateAttribute::POINT:
        writePrettyHTML(os, (const osg::Point*)object, table);
        break;
    case osg::StateAttribute::LINEWIDTH:
        writePrettyHTML(os, (const osg::LineWidth*)object, table);
        break;
    case osg::StateAttribute::LINESTIPPLE:
        writePrettyHTML(os, (const osg::LineStipple*)object, table);
        break;
    case osg::StateAttribute::POLYGONSTIPPLE:
        writePrettyHTML(os, (const osg::PolygonStipple*)object, table);
        break;
    default:
        // fallback
        writePrettyHTML(os, (const osg::StateAttribute*)object, table);
        break;
    }
}
#endif // 0

bool writePrettyHTMLImpl<osg::StateAttribute>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::StateAttribute * object = getObject<osg::StateAttribute,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>type</td><td>" << object->getType() << "</td></tr>" << std::endl;
            os << "<tr><td>member</td><td>" << object->getMember() << "</td></tr>" << std::endl;
            os << "<tr><td>isTextureAttribute</td><td>" << (object->isTextureAttribute()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>shader component</td><td>" << osg_helpers::getObjectNameAndType(object->getShaderComponent(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>" << std::endl;
            for(unsigned n = 0; n < object->getNumParents(); n++)
            {
                const osg::Object * parent = object->getParent(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>update callback</td><td>" << osg_helpers::getObjectNameAndType(object->getUpdateCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>event callback</td><td>" << osg_helpers::getObjectNameAndType(object->getEventCallback(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::WrapMode & t)
{
    switch(t)
    {
    case osg::Texture::CLAMP: os << "clamp"; break;
    case osg::Texture::CLAMP_TO_EDGE: os << "clamp to edge"; break;
    case osg::Texture::CLAMP_TO_BORDER: os << "clamp to border"; break;
    case osg::Texture::REPEAT: os << "repeat"; break;
    case osg::Texture::MIRROR: os << "mirror"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::FilterMode & t)
{
    switch(t)
    {
    case osg::Texture::LINEAR: os << "LINEAR"; break;
    case osg::Texture::LINEAR_MIPMAP_LINEAR: os << "LINEAR_MIPMAP_LINEAR"; break;
    case osg::Texture::LINEAR_MIPMAP_NEAREST: os << "LINEAR_MIPMAP_NEAREST"; break;
    case osg::Texture::NEAREST: os << "NEAREST"; break;
    case osg::Texture::NEAREST_MIPMAP_LINEAR: os << "NEAREST_MIPMAP_LINEAR"; break;
    case osg::Texture::NEAREST_MIPMAP_NEAREST: os << "NEAREST_MIPMAP_NEAREST"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::WrapParameter & t)
{
    switch(t)
    {
    case osg::Texture::WRAP_S: os << "WRAP_S"; break;
    case osg::Texture::WRAP_T: os << "WRAP_T"; break;
    case osg::Texture::WRAP_R: os << "WRAP_R"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::InternalFormatMode & t)
{
    switch(t)
    {
    case osg::Texture::USE_IMAGE_DATA_FORMAT: os << "IMAGE_DATA_FORMAT"; break;
    case osg::Texture::USE_USER_DEFINED_FORMAT: os << "USER_DEFINED_FORMAT"; break;
    case osg::Texture::USE_ARB_COMPRESSION: os << "ARB_COMPRESSION"; break;
    case osg::Texture::USE_S3TC_DXT1_COMPRESSION: os << "S3TC_DXT1_COMPRESSION"; break;
    case osg::Texture::USE_S3TC_DXT3_COMPRESSION: os << "S3TC_DXT3_COMPRESSION"; break;
    case osg::Texture::USE_S3TC_DXT5_COMPRESSION: os << "S3TC_DXT5_COMPRESSION"; break;
    case osg::Texture::USE_PVRTC_2BPP_COMPRESSION: os << "PVRTC_2BPP_COMPRESSION"; break;
    case osg::Texture::USE_PVRTC_4BPP_COMPRESSION: os << "PVRTC_4BPP_COMPRESSION"; break;
    case osg::Texture::USE_ETC_COMPRESSION: os << "ETC_COMPRESSION"; break;
    case osg::Texture::USE_RGTC1_COMPRESSION: os << "RGTC1_COMPRESSION"; break;
    case osg::Texture::USE_RGTC2_COMPRESSION: os << "RGTC2_COMPRESSION"; break;
    case osg::Texture::USE_S3TC_DXT1c_COMPRESSION: os << "S3TC_DXT1c_COMPRESSION"; break;
    case osg::Texture::USE_S3TC_DXT1a_COMPRESSION: os << "S3TC_DXT1a_COMPRESSION"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::InternalFormatType & t)
{
    switch(t)
    {
    case osg::Texture::NORMALIZED: os << "NORMALIZED"; break;
    case osg::Texture::FLOAT: os << "FLOAT"; break;
    case osg::Texture::SIGNED_INTEGER: os << "SIGNED_INTEGER"; break;
    case osg::Texture::UNSIGNED_INTEGER: os << "UNSIGNED_INTEGER"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Texture::ShadowTextureMode & t)
{
    switch(t)
    {
    case osg::Texture::LUMINANCE: os << "LUMINANCE"; break;
    case osg::Texture::INTENSITY: os << "INTENSITY"; break;
    case osg::Texture::ALPHA: os << "ALPHA"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Texture>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    TextureAccess * object = static_cast<TextureAccess*>(getObject<osg::Texture, SGIItemOsg, DynamicCaster>());
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            bool brief = true;
            // add state attribute properties first
            callNextHandler(os);

            // add remaining texture properties
            os << "<tr><td>texture target</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getTextureTarget()) << "</td></tr>" << std::endl;
            os << "<tr><td>images</td><td>" << object->getNumImages() << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << object->getTextureWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->getTextureHeight() << "</td></tr>" << std::endl;
            os << "<tr><td>depth</td><td>" << object->getTextureDepth() << "</td></tr>" << std::endl;
            os << "<tr><td>wrap mode s</td><td>" << object->getWrap(osg::Texture::WRAP_S) << "</td></tr>" << std::endl;
            os << "<tr><td>wrap mode t</td><td>" << object->getWrap(osg::Texture::WRAP_T) << "</td></tr>" << std::endl;
            os << "<tr><td>wrap mode r</td><td>" << object->getWrap(osg::Texture::WRAP_R) << "</td></tr>" << std::endl;

            os << "<tr><td>internal format mode</td><td>" << object->getInternalFormatMode() << "</td></tr>" << std::endl;
            os << "<tr><td>internal format type</td><td>" << object->getInternalFormatType() << "</td></tr>" << std::endl;
            os << "<tr><td>internal format</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getInternalFormat()) << "</td></tr>" << std::endl;
            os << "<tr><td>source format</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSourceFormat()) << "</td></tr>" << std::endl;
            os << "<tr><td>source type</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSourceType()) << "</td></tr>" << std::endl;

            os << "<tr><td>shadowComparison</td><td>" << (object->getShadowComparison()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>shadow texture mode</td><td>" << object->getShadowTextureMode() << "</td></tr>" << std::endl;
            os << "<tr><td>shadow compare function</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getShadowCompareFunc()) << "</td></tr>" << std::endl;
            os << "<tr><td>shadow ambient</td><td>" << object->getShadowAmbient() << "</td></tr>" << std::endl;

            os << "<tr><td>areAllTextureObjectsLoaded</td><td>" << (object->areAllTextureObjectsLoaded()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>resizeNonPowerOfTwoHint</td><td>" << (object->getResizeNonPowerOfTwoHint()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>clientStorageHint</td><td>" << (object->getClientStorageHint()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>unRefImageDataAfterApply</td><td>" << (object->getUnRefImageDataAfterApply()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>swizzle</td><td>" 
                << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSwizzle().x()) << ","
                << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSwizzle().y()) << ","
                << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSwizzle().z()) << ","
                << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSwizzle().w()) << ","
                << "</td></tr>" << std::endl;
            os << "<tr><td>useHardwareMipMapGeneration</td><td>" << (object->getUseHardwareMipMapGeneration()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>maxAnisotropy</td><td>" << object->getMaxAnisotropy() << "</td></tr>" << std::endl;
            os << "<tr><td>min filter</td><td>" << object->getFilter(osg::Texture::MIN_FILTER) << "</td></tr>" << std::endl;
            os << "<tr><td>mag filter</td><td>" << object->getFilter(osg::Texture::MAG_FILTER) << "</td></tr>" << std::endl;

            os << "<tr><td>border width</td><td>" << object->getBorderWidth() << "</td></tr>" << std::endl;
            os << "<tr><td>border color</td><td>"
                << vec4fToHtmlColor(osg::Vec4f(object->getBorderColor())) << "</td></tr>" << std::endl;

            os << "<tr><td>numImages</td><td>" << object->getNumImages() << "</td></tr>" << std::endl;
            for(unsigned n = 0; n < object->getNumImages(); n++)
            {
                const osg::Image * image = object->getImage(n);
                os << "<tr><td>image " << n << "</td><td>" << osg_helpers::getObjectNameAndType(image, true) << "</td></tr>" << std::endl;
            }

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeTextureObjects:
        {
            TextureAccess::TextureObjectList list = object->getTextureObjectList();
            if (itemNumber() == ~0u)
            {
                os << "<ol>";
                for (unsigned n = 0; n < list.size(); ++n)
                {
                    os << "<li>" << helpers::str_plus_number("Context", n) << "&nbsp;" << osg_helpers::getObjectNameAndType(list[n], true) << "</li>" << std::endl;
                }
                os << "</ol>";
            }
            else
            {
            }
            ret = true;
        }
        break;

    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


void writePrettyHTMLTextureProfile(std::basic_ostream<char>& os, const osg::Texture::TextureProfile & profile)
{
    os << "<ul>";
    os << "<li>numMipmapLevels=" << profile._numMipmapLevels << "</li>";
    os << "<li>internalFormat=" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(profile._internalFormat) << "</li>";
    os << "<li>width=" << profile._width << "</li>";
    os << "<li>height=" << profile._height << "</li>";
    os << "<li>depth=" << profile._depth << "</li>";
    os << "<li>border=" << profile._border << "</li>";
    os << "</ul>" << std::endl;
}

bool writePrettyHTMLImpl<osg::Texture::TextureObject>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Texture::TextureObject * object = getObject<osg::Texture::TextureObject, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>id</td><td>" << object->id() << "</td></tr>" << std::endl;
            os << "<tr><td>target</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->target()) << "</td></tr>" << std::endl;
            os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;
            os << "<tr><td>timestamp</td><td>" << object->getTimeStamp() << "</td></tr>" << std::endl;
            os << "<tr><td>allocated</td><td>" << (object->isAllocated() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>reusable</td><td>" << (object->isReusable() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>texture</td><td>" << osg_helpers::getObjectNameAndType(object->getTexture(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>profile</td><td>";
            writePrettyHTMLTextureProfile(os, object->_profile);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>next</td><td>" << osg_helpers::getObjectNameAndType(object->_next) << "</td></tr>" << std::endl;
            os << "<tr><td>previous</td><td>" << osg_helpers::getObjectNameAndType(object->_previous) << "</td></tr>" << std::endl;
            os << "<tr><td>set</td><td>" << osg_helpers::getObjectNameAndType(object->_set) << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#if OSG_MIN_VERSION_REQUIRED(3,5,0)
bool writePrettyHTMLImpl<osg::TextureObjectSet>::process(std::basic_ostream<char>& os)
#else
bool writePrettyHTMLImpl<osg::Texture::TextureObjectSet>::process(std::basic_ostream<char>& os)
#endif
{
    bool ret = false;
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    TextureObjectSetAccess * object = static_cast<TextureObjectSetAccess*>(getObject<osg::TextureObjectSet, SGIItemOsg>());
#else
    TextureObjectSetAccess * object = static_cast<TextureObjectSetAccess*>(getObject<osg::Texture::TextureObjectSet, SGIItemOsg>());
#endif
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;
            os << "<tr><td>total objects</td><td>" << object->computeNumTextureObjectsInList() << "</td></tr>" << std::endl;
            os << "<tr><td>num texture objects</td><td>" << object->getNumOfTextureObjects() << "</td></tr>" << std::endl;
            os << "<tr><td>num orphans</td><td>" << object->getNumOrphans() << "</td></tr>" << std::endl;
            os << "<tr><td>num pending orphans</td><td>" << object->getNumPendingOrphans() << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeTextureObjects:
        {
            os << "<ul>";
            TextureObjectSetAccess::TextureObjectList list = object->getTextureObjects();
            for (auto to : list)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(to.get()) << "</li>" << std::endl;
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#if OSG_MIN_VERSION_REQUIRED(3,5,0)
bool writePrettyHTMLImpl<osg::TextureObjectManager>::process(std::basic_ostream<char>& os)
#else
bool writePrettyHTMLImpl<osg::Texture::TextureObjectManager>::process(std::basic_ostream<char>& os)
#endif
{
    bool ret = false;
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
    TextureObjectManagerAccess * object = static_cast<TextureObjectManagerAccess *>(getObject<osg::TextureObjectManager, SGIItemOsg>());
#else
    TextureObjectManagerAccess * object = static_cast<TextureObjectManagerAccess *>(getObject<osg::Texture::TextureObjectManager, SGIItemOsg>());
#endif
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>contextID</td><td>" << object->getContextID() << "</td></tr>" << std::endl;
            os << "<tr><td>num active texture objects</td><td>" << object->getNumberActiveTextureObjects() << "</td></tr>" << std::endl;
            os << "<tr><td>num orphaned texture objects</td><td>" << object->getNumberOrphanedTextureObjects() << "</td></tr>" << std::endl;
            os << "<tr><td>current pool size</td><td>" << object->getCurrTexturePoolSize() << "</td></tr>" << std::endl;
            os << "<tr><td>max pool size</td><td>" << object->getMaxTexturePoolSize() << "</td></tr>" << std::endl;

            os << "<tr><td>frame number</td><td>" << object->getFrameNumber() << "</td></tr>" << std::endl;
            os << "<tr><td>number of frames</td><td>" << object->getNumberFrames() << "</td></tr>" << std::endl;
            os << "<tr><td>number deleted</td><td>" << object->getNumberDeleted() << "</td></tr>" << std::endl;
            os << "<tr><td>delete time</td><td>" << object->getDeleteTime() << "</td></tr>" << std::endl;
            os << "<tr><td>number generated</td><td>" << object->getNumberGenerated() << "</td></tr>" << std::endl;
            os << "<tr><td>generate time</td><td>" << object->getGenerateTime() << "</td></tr>" << std::endl;

#if OSG_VERSION_LESS_OR_EQUAL(3,4,0)
            os << "<tr><td>number applied</td><td>" << object->getNumberApplied() << "</td></tr>" << std::endl;
            os << "<tr><td>apply time</td><td>" << object->getApplyTime() << "</td></tr>" << std::endl;
#endif

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStatistics:
        {
            object->reportStats(os);
            ret = true;
        }
        break;
    case SGIItemTypeTextureSetMap:
        {
            const TextureObjectManagerAccess::TextureSetMap & textureSetMap = object->getTextureSetMap();
            if (itemNumber() == ~0u)
            {
                os << "<table border=\'1\' align=\'left\'><tr>";
                os << "<th>target</th>";
                os << "<th>MipMap</th>";
                os << "<th>internalFormat</th>";
                os << "<th>width</th>";
                os << "<th>height</th>";
                os << "<th>depth</th>";
                os << "<th>border</th>";
                os << "<th>size</th>";
                os << "</tr>" << std::endl;
                for (auto it = textureSetMap.begin(); it != textureSetMap.end(); ++it)
                {
                    const osg::Texture::TextureProfile & p = it->first;
                    os << "<tr>";
                    os << "<td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(p._target) << "</td>";
                    os << "<td>" << p._numMipmapLevels << "</td>";
                    os << "<td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(p._internalFormat) << "</td>";
                    os << "<td>" << p._width << "</td>";
                    os << "<td>" << p._height << "</td>";
                    os << "<td>" << p._depth << "</td>";
                    os << "<td>" << p._border << "</td>";
                    os << "<td>" << p._size << "</td>";
                    os << "</tr>";
                }
                os << "</table>";
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Texture1D>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Texture1D * object = getObject<osg::Texture1D,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>numMipmapLevels</td><td>" << object->getNumMipmapLevels() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Texture2D>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Texture2D * object = getObject<osg::Texture2D,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>numMipmapLevels</td><td>" << object->getNumMipmapLevels() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Texture3D>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Texture3D * object = getObject<osg::Texture3D,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Texture properties first
            callNextHandler(os);

            os << "<tr><td>numMipmapLevels</td><td>" << object->getNumMipmapLevels() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::PolygonMode::Mode & t)
{
    switch(t)
    {
    case osg::PolygonMode::POINT: os << "point"; break;
    case osg::PolygonMode::LINE: os << "line"; break;
    case osg::PolygonMode::FILL: os << "fill"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::PolygonMode>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PolygonMode * object = getObject<osg::PolygonMode,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining PolygonMode properties
            os << "<tr><td>mode front</td><td>" << object->getMode(osg::PolygonMode::FRONT) << "</td></tr>" << std::endl;
            os << "<tr><td>mode back</td><td>" << object->getMode(osg::PolygonMode::BACK) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::PolygonOffset>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PolygonOffset * object = getObject<osg::PolygonOffset,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining PolygonOffset properties
            os << "<tr><td>factor</td><td>" << object->getFactor() << "</td></tr>" << std::endl;
            os << "<tr><td>units</td><td>" << object->getUnits() << "</td></tr>" << std::endl;
            os << "<tr><td>factor multiplier</td><td>" << object->getFactorMultiplier() << "</td></tr>" << std::endl;
            os << "<tr><td>units multiplier</td><td>" << object->getUnitsMultiplier() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Material::ColorMode & t)
{
    switch (t)
    {
    case osg::Material::AMBIENT: os << "AMBIENT"; break;
    case osg::Material::DIFFUSE: os << "DIFFUSE"; break;
    case osg::Material::SPECULAR: os << "SPECULAR"; break;
    case osg::Material::EMISSION: os << "EMISSION"; break;
    case osg::Material::AMBIENT_AND_DIFFUSE: os << "AMBIENT_AND_DIFFUSE"; break;
    case osg::Material::OFF: os << "OFF"; break;
    default: os << (int)t; break;
    }
    return os;
}
bool writePrettyHTMLImpl<osg::Material>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Material * object = getObject<osg::Material,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining Material properties
            os << "<tr><td>color mode</td><td>" << object->getColorMode() << "</td></tr>" << std::endl;

            osg::Material::Face faces[3] = { osg::Material::FRONT, osg::Material::BACK, osg::Material::FRONT_AND_BACK };
            const char * faceNames[3] = { "front", "back", "front+back" };
            for(int i = 0; i < sizeof(faces)/sizeof(faces[0]); i++)
            {
                os << "<tr><td>" << faceNames[i] << " ambient</td><td>"
                    << vec4fToHtmlColor(object->getAmbient(faces[i])) << "</td></tr>" << std::endl;
                os << "<tr><td>" << faceNames[i] << " diffuse</td><td>"
                    << vec4fToHtmlColor(object->getDiffuse(faces[i])) << "</td></tr>" << std::endl;
                os << "<tr><td>" << faceNames[i] << " specular</td><td>"
                    << vec4fToHtmlColor(object->getSpecular(faces[i])) << "</td></tr>" << std::endl;
                os << "<tr><td>" << faceNames[i] << " emission</td><td>"
                    << vec4fToHtmlColor(object->getEmission(faces[i])) << "</td></tr>" << std::endl;
                os << "<tr><td>" << faceNames[i] << " shininess</td><td>"
                    << object->getShininess(faces[i]) << "</td></tr>" << std::endl;
            }

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Light>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Light * object = getObject<osg::Light,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining Light properties
            os << "<tr><td>ambient</td><td>"
                << vec4fToHtmlColor(object->getAmbient()) << "</td></tr>" << std::endl;
            os << "<tr><td>diffuse</td><td>"
                << vec4fToHtmlColor(object->getDiffuse()) << "</td></tr>" << std::endl;
            os << "<tr><td>specular</td><td>"
                << vec4fToHtmlColor(object->getSpecular()) << "</td></tr>" << std::endl;
                
            os << "<tr><td>constantAttenuation</td><td>" << object->getConstantAttenuation() << "</td></tr>" << std::endl;
            os << "<tr><td>linearAttenuation</td><td>" << object->getLinearAttenuation() << "</td></tr>" << std::endl;
            os << "<tr><td>quadraticAttenuation</td><td>" << object->getQuadraticAttenuation() << "</td></tr>" << std::endl;
            os << "<tr><td>spotExponent</td><td>" << object->getSpotExponent() << "</td></tr>" << std::endl;
            os << "<tr><td>spotCutoff</td><td>" << object->getSpotCutoff() << "</td></tr>" << std::endl;

            os << "<tr><td>position</td><td>";
            writePrettyHTML(os, object->getPosition());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>direction</td><td>" << object->getDirection() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::LightModel::ColorControl & t)
{
    bool ret = false;
    switch(t)
    {
    case osg::LightModel::SEPARATE_SPECULAR_COLOR: os << "SEPARATE_SPECULAR_COLOR"; break;
    case osg::LightModel::SINGLE_COLOR: os << "SINGLE_COLOR"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::LightModel>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::LightModel * object = getObject<osg::LightModel,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining LightModel properties
            os << "<tr><td>ambient intensity</td><td>"
                << vec4fToHtmlColor(object->getAmbientIntensity()) << "</td></tr>" << std::endl;

            os << "<tr><td>color control</td><td>" << object->getColorControl() << "</td></tr>" << std::endl;
            os << "<tr><td>local viewer</td><td>" << (object->getLocalViewer()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>two sided</td><td>" << (object->getTwoSided()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::LightSource>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::LightSource * object = getObject<osg::LightSource,SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
    {
        if (_table)
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

        // add state attribute properties first
        callNextHandler(os);

        // add remaining LightSource properties
        os << "<tr><td>light</td><td>" << osg_helpers::getObjectNameAndType(object->getLight()) << "</td></tr>" << std::endl;
        os << "<tr><td>ref</td><td>" << object->getReferenceFrame() << "</td></tr>" << std::endl;
        
        if (_table)
            os << "</table>" << std::endl;
        ret = true;
    }
    break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::TexEnv::Mode & t)
{
    switch(t)
    {
    case osg::TexEnv::DECAL: os << "DECAL"; break;
    case osg::TexEnv::MODULATE: os << "MODULATE"; break;
    case osg::TexEnv::BLEND: os << "BLEND"; break;
    case osg::TexEnv::REPLACE: os << "REPLACE"; break;
    case osg::TexEnv::ADD: os << "ADD"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::TexEnv>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::TexEnv * object = getObject<osg::TexEnv,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining TexEnv properties
            os << "<tr><td>color</td><td>"
                << vec4fToHtmlColor(object->getColor()) << "</td></tr>" << std::endl;
            os << "<tr><td>mode</td><td>" << object->getMode() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::TexGen::Mode & t)
{
    switch(t)
    {
    case osg::TexGen::OBJECT_LINEAR: os << "OBJECT_LINEAR"; break;
    case osg::TexGen::EYE_LINEAR: os << "EYE_LINEAR"; break;
    case osg::TexGen::SPHERE_MAP: os << "SPHERE_MAP"; break;
    case osg::TexGen::NORMAL_MAP: os << "NORMAL_MAP"; break;
    case osg::TexGen::REFLECTION_MAP: os << "REFLECTION_MAP"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::TexGen>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::TexGen * object = getObject<osg::TexGen,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining TexGen properties
            os << "<tr><td>mode</td><td>" << object->getMode() << "</td></tr>" << std::endl;
            os << "<tr><td>plane S</td><td>" << object->getPlane(osg::TexGen::S) << "</td></tr>" << std::endl;
            os << "<tr><td>plane T</td><td>" << object->getPlane(osg::TexGen::T) << "</td></tr>" << std::endl;
            os << "<tr><td>plane R</td><td>" << object->getPlane(osg::TexGen::R) << "</td></tr>" << std::endl;
            os << "<tr><td>plane Q</td><td>" << object->getPlane(osg::TexGen::Q) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Stencil::Function & t)
{
    switch(t)
    {
    case osg::Stencil::NEVER: os << "NEVER"; break;
    case osg::Stencil::LESS: os << "LESS"; break;
    case osg::Stencil::EQUAL: os << "EQUAL"; break;
    case osg::Stencil::LEQUAL: os << "LEQUAL"; break;
    case osg::Stencil::GREATER: os << "GREATER"; break;
    case osg::Stencil::NOTEQUAL: os << "NOTEQUAL"; break;
    case osg::Stencil::GEQUAL: os << "GEQUAL"; break;
    case osg::Stencil::ALWAYS: os << "ALWAYS"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Stencil::Operation & t)
{
    switch(t)
    {
    case osg::Stencil::KEEP: os << "KEEP"; break;
    case osg::Stencil::ZERO: os << "ZERO"; break;
    case osg::Stencil::REPLACE: os << "REPLACE"; break;
    case osg::Stencil::INCR: os << "INCR"; break;
    case osg::Stencil::DECR: os << "DECR"; break;
    case osg::Stencil::INVERT: os << "INVERT"; break;
    case osg::Stencil::INCR_WRAP: os << "INCR_WRAP"; break;
    case osg::Stencil::DECR_WRAP: os << "DECR_WRAP"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Stencil>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Stencil * object = getObject<osg::Stencil,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining Stencil properties
            os << "<tr><td>function</td><td>" << object->getFunction() << "</td></tr>" << std::endl;
            os << "<tr><td>function ref</td><td>" << object->getFunctionRef() << "</td></tr>" << std::endl;
            os << "<tr><td>function mask</td><td>" << object->getFunctionMask() << "</td></tr>" << std::endl;
            os << "<tr><td>fail operation</td><td>" << object->getStencilFailOperation() << "</td></tr>" << std::endl;
            os << "<tr><td>pass and depth fail operation</td><td>" << object->getStencilPassAndDepthFailOperation() << "</td></tr>" << std::endl;
            os << "<tr><td>pass and depth pass operation</td><td>" << object->getStencilPassAndDepthPassOperation() << "</td></tr>" << std::endl;
            os << "<tr><td>pass and depth pass operation</td><td>" << object->getWriteMask() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Viewport>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Viewport * object = getObject<osg::Viewport,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining Viewport properties
            osg_helpers::writePrettyHTML(os, object);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Program>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Program * object = getObject<osg::Program,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining program properties
            os << "<tr><td>numShaders</td><td>" << object->getNumShaders() << "</td></tr>" << std::endl;
            os << "<tr><td>binary</td><td>" << osg_helpers::getObjectNameAndType(object->getProgramBinary()) << "</td></tr>" << std::endl;
            os << "<tr><td>isFixedFunction</td><td>" << (object->isFixedFunction()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>transformFeedBackMode</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getTransformFeedBackMode()) << "</td></tr>" << std::endl;

            os << "<tr><td>transformFeedBackVaryings</td><td><ul>";
            for (unsigned i = 0; i != object->getNumTransformFeedBackVaryings(); ++i)
            {
                os << "<li>" << object->getTransformFeedBackVarying(i) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

#if OSG_VERSION_LESS_THAN(3,5,9)
            GLint numGroupsX, numGroupsY, numGroupsZ;
            object->getComputeGroups(numGroupsX, numGroupsY, numGroupsZ);
            os << "<tr><td>computeGroups</td><td>x=" << numGroupsX << ", y=" << numGroupsY << ", z=" << numGroupsZ << "</td></tr>" << std::endl;
#endif

            os << "<tr><td>attribBindingList</td><td><ul>";
            const osg::Program::AttribBindingList& attribBindingList = object->getAttribBindingList();
            for(osg::Program::AttribBindingList::const_iterator it = attribBindingList.begin(); it != attribBindingList.end(); it++)
            {
                const std::string & name = it->first;
                const GLuint & id = it->second;
                os << "<li>" << name << "=" << id << "</li>" << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>fragDataBindingList</td><td><ul>";
            const osg::Program::FragDataBindingList& fragDataBindingList = object->getFragDataBindingList();
            for(osg::Program::FragDataBindingList::const_iterator it = fragDataBindingList.begin(); it != fragDataBindingList.end(); it++)
            {
                const std::string & name = it->first;
                const GLuint & id = it->second;
                os << "<li>" << name << "=" << id << "</li>" << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>uniformBlockBindingList</td><td><ul>";
            const osg::Program::UniformBlockBindingList& uniformBlockBindingList = object->getUniformBlockBindingList();
            for(osg::Program::UniformBlockBindingList::const_iterator it = uniformBlockBindingList.begin(); it != uniformBlockBindingList.end(); it++)
            {
                const std::string & name = it->first;
                const GLuint & id = it->second;
                os << "<li>" << name << "=" << id << "</li>" << std::endl;
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaders:
        {
            os << "<ol>";
            unsigned numShaders = object->getNumShaders();
            for(unsigned i = 0; i < numShaders; i++)
            {
                osg::Shader * shader = object->getShader(i);
                os << "<li>" << shader->getName() << '(' << shader->getTypename() << ')' << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeShaderDefines:
        {
            const osg::ShaderDefines & defines = object->getShaderDefines();
            os << "<ol>";
            for(osg::ShaderDefines::const_iterator it = defines.begin(); it != defines.end(); ++it)
            {
                os << "<li>" << *it << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeProgramLog:
        {
            unsigned contextId = _item->number();
            std::string log;
            if (object->getGlProgramInfoLog(contextId, log))
            {
                os << "<b>Log for context ID " << contextId << "</b><br/>";
                if (log.empty())
                    os << "<i>empty</i>";
                else
                    os << "<pre>" << log << "</pre>";
            }
            else
            {
                os << "<i>No log available for context ID</i>" << contextId;
            }
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Program::PerContextProgram>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Program::PerContextProgram * object = getObject<osg::Program::PerContextProgram,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining program properties
            os << "<tr><td>needsLink</td><td>" << (object->needsLink()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isLinked</td><td>" << (object->isLinked()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>infoLog</td><td><pre>";
            std::string infoLog;
            object->getInfoLog(infoLog);
            os << helpers::html_encode(infoLog) << "</pre></td></tr>" << std::endl;

            os << "<tr><td>loadedBinary</td><td>" << (object->loadedBinary()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Program::ProgramBinary>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Program::ProgramBinary * object = getObject<osg::Program::ProgramBinary, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add state attribute properties first
            callNextHandler(os);

            // add remaining program properties
            os << "<tr><td>format</td><td>" << object->getFormat() << "</td></tr>" << std::endl;
            os << "<tr><td>size</td><td>" << object->getSize() << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ClipPlane>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ClipPlane * object = getObject<osg::ClipPlane,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining ClipPlane properties
            os << "<tr><td>plane num</td><td>" << object->getClipPlaneNum() << "</td></tr>" << std::endl;
            os << "<tr><td>plane</td><td>" << object->getClipPlane() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::BlendFunc>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BlendFunc * object = getObject<osg::BlendFunc,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining BlendFunc properties
            os << "<tr><td>source</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSource()) << "</td></tr>" << std::endl;
            os << "<tr><td>source rgb</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSourceRGB()) << "</td></tr>" << std::endl;
            os << "<tr><td>source alpha</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getSourceAlpha()) << "</td></tr>" << std::endl;
            os << "<tr><td>destination</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDestination()) << "</td></tr>" << std::endl;
            os << "<tr><td>destination rgb</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDestinationRGB()) << "</td></tr>" << std::endl;
            os << "<tr><td>destination alpha</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDestinationAlpha()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::BlendColor>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BlendColor * object = getObject<osg::BlendColor,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining BlendColor properties
            os << "<tr><td>constantColor</td><td>" << object->getConstantColor() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BlendEquation::Equation & t)
{
    switch(t)
    {
    case osg::BlendEquation::RGBA_MIN: os << "RGBA_MIN"; break;
    case osg::BlendEquation::RGBA_MAX: os << "RGBA_MAX"; break;
    case osg::BlendEquation::ALPHA_MIN: os << "ALPHA_MIN"; break;
    case osg::BlendEquation::ALPHA_MAX: os << "ALPHA_MAX"; break;
    case osg::BlendEquation::LOGIC_OP: os << "LOGIC_OP"; break;
    case osg::BlendEquation::FUNC_ADD: os << "FUNC_ADD"; break;
    case osg::BlendEquation::FUNC_SUBTRACT: os << "FUNC_SUBTRACT"; break;
    case osg::BlendEquation::FUNC_REVERSE_SUBTRACT: os << "FUNC_REVERSE_SUBTRACT"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::BlendEquation>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BlendEquation * object = getObject<osg::BlendEquation,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining BlendEquation properties
            os << "<tr><td>equation</td><td>" << object->getEquation() << "</td></tr>" << std::endl;
            os << "<tr><td>equationRGB</td><td>" << object->getEquationRGB() << "</td></tr>" << std::endl;
            os << "<tr><td>equationAlpha</td><td>" << object->getEquationAlpha() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<osg::LineWidth>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::LineWidth * object = getObject<osg::LineWidth,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining LineWidth properties
            os << "<tr><td>width</td><td>" << object->getWidth() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::LineStipple>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::LineStipple * object = getObject<osg::LineStipple,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining LineStipple properties
            os << "<tr><td>factor</td><td>" << object->getFactor() << "</td></tr>" << std::endl;
            os << "<tr><td>pattern</td><td>0x" << std::hex << object->getPattern() << std::dec << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Point>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Point * object = getObject<osg::Point,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining Point properties
            os << "<tr><td>size</td><td>" << object->getSize() << "</td></tr>" << std::endl;
            os << "<tr><td>minSize</td><td>" << object->getMinSize() << "</td></tr>" << std::endl;
            os << "<tr><td>maxSize</td><td>" << object->getMaxSize() << "</td></tr>" << std::endl;
            os << "<tr><td>fade threshold size</td><td>" << object->getFadeThresholdSize() << "</td></tr>" << std::endl;
            os << "<tr><td>distance attenuation</td><td>" << object->getDistanceAttenuation() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::PolygonStipple>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::PolygonStipple * object = getObject<osg::PolygonStipple,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining PolygonStipple properties
            os << "<tr><td>mask</td><td>";
            const GLubyte* mask = object->getMask();
            os << "<pre>";
            for(unsigned row = 0; row < 32; row++)
            {
                for(unsigned col = 0; col < 32; col++)
                {
                    unsigned bitno = row * 32 + col;
                    GLubyte byte = mask[bitno / 8];
                    if((byte & (1u << (bitno%8))) != 0)
                        os << '1';
                    else
                        os << '0';
                }
                os << std::endl;
            }
            os << "</pre>";
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::Depth::Function & t)
{
    switch(t)
    {
    case osg::Depth::NEVER: os << "GL_NEVER"; break;
    case osg::Depth::LESS: os << "GL_LESS"; break;
    case osg::Depth::EQUAL: os << "GL_EQUAL"; break;
    case osg::Depth::LEQUAL: os << "GL_LEQUAL"; break;
    case osg::Depth::GREATER: os << "GL_GREATER"; break;
    case osg::Depth::NOTEQUAL: os << "GL_NOTEQUAL"; break;
    case osg::Depth::GEQUAL: os << "GL_GEQUAL"; break;
    case osg::Depth::ALWAYS: os << "GL_ALWAYS"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Depth>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Depth * object = getObject<osg::Depth,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add StateAttribute properties first
            callNextHandler(os);

            // add remaining Depth properties
            os << "<tr><td>function</td><td>" << object->getFunction() << "</td></tr>" << std::endl;
            os << "<tr><td>near</td><td>" << object->getZNear() << "</td></tr>" << std::endl;
            os << "<tr><td>far</td><td>" << object->getZFar() << "</td></tr>" << std::endl;
            os << "<tr><td>writeMask</td><td>" << (object->getWriteMask()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::GraphicsContext::Traits>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            osg::GraphicsContext::Traits * object = getObject<osg::GraphicsContext::Traits,SGIItemOsg>();
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>x</td><td>" << object->x << "</td></tr>" << std::endl;
            os << "<tr><td>y</td><td>" << object->y << "</td></tr>" << std::endl;
            os << "<tr><td>width</td><td>" << object->width << "</td></tr>" << std::endl;
            os << "<tr><td>height</td><td>" << object->height << "</td></tr>" << std::endl;

            os << "<tr><td>windowName</td><td>" << object->windowName << "</td></tr>" << std::endl;
            os << "<tr><td>windowDecoration</td><td>" << (object->windowDecoration?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>supportsResize</td><td>" << (object->supportsResize?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>red</td><td>" << object->red << "</td></tr>" << std::endl;
            os << "<tr><td>blue</td><td>" << object->blue << "</td></tr>" << std::endl;
            os << "<tr><td>green</td><td>" << object->green << "</td></tr>" << std::endl;
            os << "<tr><td>alpha</td><td>" << object->alpha << "</td></tr>" << std::endl;
            os << "<tr><td>depth</td><td>" << object->depth << "</td></tr>" << std::endl;
            os << "<tr><td>stencil</td><td>" << object->stencil << "</td></tr>" << std::endl;

            os << "<tr><td>sampleBuffers</td><td>" << object->sampleBuffers << "</td></tr>" << std::endl;
            os << "<tr><td>samples</td><td>" << object->samples << "</td></tr>" << std::endl;

            os << "<tr><td>pbuffer</td><td>" << (object->pbuffer?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>quadBufferStereo</td><td>" << (object->quadBufferStereo?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>doubleBuffer</td><td>" << (object->doubleBuffer?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>target</td><td>" << object->target << "</td></tr>" << std::endl;
            os << "<tr><td>format</td><td>" << object->format << "</td></tr>" << std::endl;

            os << "<tr><td>level</td><td>" << object->level << "</td></tr>" << std::endl;
            os << "<tr><td>face</td><td>" << object->face << "</td></tr>" << std::endl;
            os << "<tr><td>mipMapGeneration</td><td>" << object->mipMapGeneration << "</td></tr>" << std::endl;

            os << "<tr><td>vsync</td><td>" << (object->vsync?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>swapGroupEnabled</td><td>" << (object->swapGroupEnabled?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>swapGroup</td><td>" << object->swapGroup << "</td></tr>" << std::endl;
            os << "<tr><td>swapBarrier</td><td>" << object->swapBarrier << "</td></tr>" << std::endl;

            os << "<tr><td>useMultiThreadedOpenGLEngine</td><td>" << (object->useMultiThreadedOpenGLEngine?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>useCursor</td><td>" << (object->useCursor?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>glContextVersion</td><td>" << object->glContextVersion << "</td></tr>" << std::endl;

            os << "<tr><td>glContextFlags</td><td>0x" << std::hex << object->glContextFlags << "</td></tr>" << std::endl;
            os << "<tr><td>glContextProfileMask</td><td>0x" << std::hex << object->glContextProfileMask << "</td></tr>" << std::endl;

            os << "<tr><td>sharedContext</td><td>0x" << std::hex << (void*)object->sharedContext.get() << "</td></tr>" << std::endl;
            os << "<tr><td>inheritedWindowData</td><td>0x" << std::hex << (void*)object->inheritedWindowData.get() << "</td></tr>" << std::endl;

            os << "<tr><td>setInheritedWindowPixelFormat</td><td>" << (object->setInheritedWindowPixelFormat?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>overrideRedirect</td><td>" << (object->overrideRedirect?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>swapMethod</td><td>" << object->swapMethod << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Operation>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Operation * object = getObject<osg::Operation,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>keep</td><td>" << (object->getKeep()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::GraphicsOperation>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::GraphicsOperation * object = getObject<osg::GraphicsOperation,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::BarrierOperation::PreBlockOp & t)
{
    switch(t)
    {
    case osg::BarrierOperation::NO_OPERATION: os << "NO_OPERATION"; break;
    case osg::BarrierOperation::GL_FLUSH: os << "GL_FLUSH"; break;
    case osg::BarrierOperation::GL_FINISH: os << "GL_FINISH"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::BarrierOperation>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BarrierOperation * object = getObject<osg::BarrierOperation,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);
            
            os << "<tr><td>name</td><td>" << object->_preBlockOp << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}



bool writePrettyHTMLImpl<osg::OperationQueue>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    OperationQueueAccess * object = static_cast<OperationQueueAccess*>(getObject<osg::OperationQueue,SGIItemOsg>());
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            OperationQueueAccess::Operations operations;
            object->getOperations(operations);

            os << "<tr><td>numOperationsInQueue</td><td>" << operations.size()  << "</td></tr>" << std::endl;
            os << "<tr><td>operations</td><td><ol>";
            for (const auto & op : operations)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(op.get()) << "</li>";
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::OperationThread>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::OperationThread * object = getObject<osg::OperationThread,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>done</td><td>" << (object->getDone()?"true":"false") << "</td></tr>" << std::endl;
            writePrettyHTMLImpl_OpenThreads_Thread(os, object);

            os << "<tr><td>queue</td><td>";
            const osg::OperationQueue * queue = object->getOperationQueue();
            os << (queue?const_cast<osg::OperationQueue *>(queue)->getNumOperationsInQueue():0) << " operations";
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::GraphicsThread>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::GraphicsThread * object = getObject<osg::GraphicsThread,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            writePrettyHTMLImpl_OpenThreads_Thread(os, object);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

#ifndef GL_CONTEXT_PROFILE_MASK
#define GL_CONTEXT_PROFILE_MASK           0x9126
#endif
#ifndef GL_CONTEXT_FLAGS
#define GL_CONTEXT_FLAGS                  0x821E
#endif
#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION                  0x821B
#endif
#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION                  0x821C
#endif

struct PerContextInfo {
    std::string glversion;
    std::string glvendor;
    std::string glrenderer;
    int majorVersion;
    int minorVersion;
    unsigned contextProfileMask;
    unsigned contextFlags;
};
static std::map<unsigned, PerContextInfo> s_contextInfo;
struct CollectPerContextInfo : public osg::Operation {

    /** Do the actual task of this operation.*/
    void operator () (osg::Object* object) override
    {
        GraphicsContextAccess* ctx = static_cast<GraphicsContextAccess*>(object);
        unsigned ctxId = ctx->getContextID();
        PerContextInfo & info = s_contextInfo[ctxId];
        info.glvendor = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        info.glrenderer = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        info.glversion = std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        glGetIntegerv(GL_MAJOR_VERSION, &info.majorVersion);
        glGetIntegerv(GL_MINOR_VERSION, &info.minorVersion);
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, (GLint*)&info.contextProfileMask);
        glGetIntegerv(GL_CONTEXT_FLAGS, (GLint*)&info.contextProfileMask);
    }

};

bool writePrettyHTMLImpl<osg::GraphicsContext>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    GraphicsContextAccess * object = static_cast<GraphicsContextAccess*>(getObject<osg::GraphicsContext, SGIItemOsg>());

    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            // add object properties first
            callNextHandler(os);

            // add remaining GraphicsContext properties
            os << "<tr><td>valid</td><td>" << (object->valid()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isRealized</td><td>" << (object->isRealized()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isCurrent</td><td>" << (object->isCurrent()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>contextId</td><td>" << object->getContextID();
            auto itContextInfo = s_contextInfo.find(object->getContextID());
            if (itContextInfo == s_contextInfo.end())
            {
                os << "<i>(additional info pending)</i></td></tr>" << std::endl;
                object->add(new CollectPerContextInfo());
            }
            else
            {
                os << "<i>(info available)</i></td></tr>" << std::endl;
                const PerContextInfo & info = itContextInfo->second;
                os << "<tr><td>GL vendor</td><td>" << info.glvendor << "</td></tr>" << std::endl;
                os << "<tr><td>GL renderer</td><td>" << info.glrenderer << "</td></tr>" << std::endl;
                os << "<tr><td>GL version string</td><td>" << info.glversion << "</td></tr>" << std::endl;
                os << "<tr><td>GL version number</td><td>" << info.majorVersion << "." << info.minorVersion << "</td></tr>" << std::endl;
                os << "<tr><td>GL profile mask</td><td>0x" << std::hex << info.contextProfileMask << "</td></tr>" << std::endl;
                os << "<tr><td>GL context flags</td><td>0x" << std::hex << info.contextFlags << "</td></tr>" << std::endl;
                os << std::dec;
            }

            os << "<tr><td>clear color</td><td>"
                << vec4fToHtmlColor(object->getClearColor())
                << "</td></tr>" << std::endl;
            os << "<tr><td>clear mask</td><td>" << clearMaskToString(object->getClearMask()) << "</td></tr>" << std::endl;
            os << "<tr><td>timeSinceLastClear</td><td>" << object->getTimeSinceLastClear() << "</td></tr>" << std::endl;

            os << "<tr><td>cameras</td><td><ul>";
            const osg::GraphicsContext::Cameras & cameras = object->getCameras();
            for(osg::GraphicsContext::Cameras::const_iterator it = cameras.begin(); it != cameras.end(); it++)
            {
                osg::Camera * camera = *it;
                os << "<li>" << osg_helpers::getObjectNameAndType(camera, true) << "</li>" << std::endl;
            }
            os << "<ul></td></tr>" << std::endl;
            os << "<tr><td>traits</td><td>" << osg_helpers::getObjectNameAndType(object->getTraits()) << "</td></tr>" << std::endl;
            os << "<tr><td>thread</td><td>" << osg_helpers::getObjectNameAndType(object->getGraphicsThread()) << "</td></tr>" << std::endl;

            

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object callbacks first
            callNextHandler(os);

            // add osg::GraphicsContext callbacks now
            os << "<tr><td>swap callback</td><td>" << osg_helpers::getObjectNameAndType(object->getSwapCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>resized callback</td><td>" << osg_helpers::getObjectNameAndType(object->getResizedCallback()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextCameras:
        {
            os << "<ol>";
            const osg::GraphicsContext::Cameras & cameras = object->getCameras();
            for(osg::GraphicsContext::Cameras::const_iterator it = cameras.begin(); it != cameras.end(); it++)
            {
                osg::Camera * camera = *it;
                os << "<li>";
                os << osg_helpers::getObjectNameAndType(camera) << std::endl;
                os << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextOperations:
        {
            os << "<ol>";
            const osg::GraphicsContext::GraphicsOperationQueue & queue = object->getOperationsQueue();
            for(osg::GraphicsContext::GraphicsOperationQueue::const_iterator it = queue.begin(); it != queue.end(); it++)
            {
                osg::Operation * operation = *it;
                os << "<li>" << operation->getName() << " keep=" << (operation->getKeep()?"true":"false") << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeGraphicsContextGLExtensions:
        {
            osg::State * state = object->getState();
            unsigned int contextID = (state)?state->getContextID():~0u;
            if(contextID != ~0u)
            {
#ifdef OSG_HAS_GETGLEXTENSIONSET
                typedef std::set<std::string> StringSet;
                StringSet extensions;
                osg::getGLExtensionSet(contextID, extensions);
                os << "Total number of extensions: " << extensions.size() << "<br/>" << std::endl;

                StringSet amd_ati_extensions;
                StringSet arb_extensions;
                StringSet ext_extensions;
                StringSet intel_extensions;
                StringSet nvidia_extensions;
                StringSet s3_extensions;
                StringSet sgi_extensions;
                StringSet other_extensions;
                for(StringSet::const_iterator it = extensions.begin(); it != extensions.end(); it++)
                {
                    const std::string & extension = *it;
                    if(extension.compare(0, 6, "GL_AMD") == 0 || extension.compare(0, 6, "GL_ATI") == 0)
                        amd_ati_extensions.insert(extension);
                    else if(extension.compare(0, 6, "GL_ARB") == 0)
                        arb_extensions.insert(extension);
                    else if(extension.compare(0, 6, "GL_EXT") == 0)
                        ext_extensions.insert(extension);
                    else if(extension.compare(0, 8, "GL_INTEL") == 0)
                        intel_extensions.insert(extension);
                    else if(extension.compare(0, 5, "GL_NV") == 0)
                        nvidia_extensions.insert(extension);
                    else if(extension.compare(0, 5, "GL_S3") == 0)
                        s3_extensions.insert(extension);
                    else if(extension.compare(0, 6, "GL_SGI") == 0)
                        sgi_extensions.insert(extension);
                    else
                        other_extensions.insert(extension);
                }
                os << "ARB extensions: " << arb_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = arb_extensions.begin(); it != arb_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "EXT extensions: " << ext_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = ext_extensions.begin(); it != ext_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "AMD/ATI extensions: " << amd_ati_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = amd_ati_extensions.begin(); it != amd_ati_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "NVIDIA extensions: " << nvidia_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = nvidia_extensions.begin(); it != nvidia_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "Intel extensions: " << intel_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = intel_extensions.begin(); it != intel_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "S3 extensions: " << s3_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = s3_extensions.begin(); it != s3_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "SGI extensions: " << sgi_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = sgi_extensions.begin(); it != sgi_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
                os << "Other extensions: " << other_extensions.size() << "<ol>" << std::endl;
                for(StringSet::const_iterator it = other_extensions.begin(); it != other_extensions.end(); it++)
                    os << "<li>" << *it << "</li>" << std::endl;
                os << "</ol>" << std::endl;
#else
                os << "<i>OSG compiled without the getGLExtensionSet function.</i>" << std::endl;
#endif
            }
            else
            {
                os << "<i>Graphics context has an invalid state.</i>" << std::endl;
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateSet::RenderingHint & t)
{
    switch(t)
    {
    case osg::StateSet::DEFAULT_BIN: os << "default"; break;
    case osg::StateSet::OPAQUE_BIN: os << "opaque"; break;
    case osg::StateSet::TRANSPARENT_BIN: os << "transparent"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateSet::RenderBinMode & t)
{
    switch(t)
    {
    case osg::StateSet::INHERIT_RENDERBIN_DETAILS: os << "inherit"; break;
    case osg::StateSet::USE_RENDERBIN_DETAILS: os << "use"; break;
    case osg::StateSet::OVERRIDE_RENDERBIN_DETAILS: os << "override"; break;
    default: os << (int)t; break;
    }
    return os;
}

void writePrettyHTMLStateSetModeList(std::basic_ostream<char>& os, const SGIItemOsg * item)
{
    const osg::StateSet * object = static_cast<const osg::StateSet*>(item->object());
    const osg::StateSet::ModeList & modeList = object->getModeList();
    if(!modeList.empty())
    {
        os << "<ol>" << std::endl;
        for(osg::StateSet::ModeList::const_iterator it = modeList.begin(); it != modeList.end(); it++)
        {
            const osg::StateAttribute::GLMode & mode = it->first;
            const osg::StateAttribute::GLModeValue & value = it->second;
            os << "<li>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode) << "=" << glValueName(value) << "</li>";
        }
        os << "</ol>" << std::endl;
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

void writePrettyHTMLStateSetTextureModeList(std::basic_ostream<char>& os, const SGIItemOsg * item, unsigned index)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item->object());
    const osg::StateSet::TextureModeList & textureModeList = object->getTextureModeList();
    if(!textureModeList.empty())
    {
        unsigned count = 0;
        os << "<ol>";
        for(osg::StateSet::TextureModeList::const_iterator it = textureModeList.begin(); it != textureModeList.end(); it++, count++)
        {
            if(index == ~0u || index == count)
            {
                const osg::StateSet::ModeList & modeList = *it;
                if(modeList.empty())
                    os << "<li>Unit #" << count << ":&nbsp;&lt;empty&gt;</li>" << std::endl;
                else
                {
                    for(osg::StateSet::ModeList::const_iterator it2 = modeList.begin(); it2 != modeList.end(); it2++)
                    {
                        const osg::StateAttribute::GLMode & mode = it2->first;
                        const osg::StateAttribute::GLModeValue & value = it2->second;
                        os << "<li>Unit #" << count << ":&nbsp;";
                        os << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(mode) << "=" << glValueName(value);
                        os << "</li>";
                    }
                }
            }
        }
        os << "</ol>";
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

void writePrettyHTMLStateSetTextureAttributeList(std::basic_ostream<char>& os, const SGIItemOsg * item, unsigned index)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item->object());
    const osg::StateSet::TextureAttributeList & textureAttributes = object->getTextureAttributeList();
    if(!textureAttributes.empty())
    {
        unsigned count = 0;
        os << "<ol>";
        for(osg::StateSet::TextureAttributeList::const_iterator it = textureAttributes.begin(); it != textureAttributes.end(); it++, count++)
        {
            if(index == ~0u || index == count)
            {
                const osg::StateSet::AttributeList & attributes = *it;
                if(attributes.empty())
                    os << "<li>Unit #" << count << ":&nbsp;&lt;empty&gt;</li>" << std::endl;
                else
                {
                    for(osg::StateSet::AttributeList::const_iterator it2 = attributes.begin(); it2 != attributes.end(); it2++)
                    {
                        const osg::StateAttribute::TypeMemberPair & typememberpair = it2->first;
                        const osg::StateAttribute::Type & type = typememberpair.first;
                        const unsigned int & member = typememberpair.second;
                        const osg::StateSet::RefAttributePair & attrpair = it2->second;
                        const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                        const osg::StateAttribute::OverrideValue & value = attrpair.second;

                        os << "<li>Unit #" << count << ":&nbsp;" << std::endl;
                        os << type << " member=" << member << " value=" << glOverrideValueName(value) << "<br/>";
                        os << osg_helpers::getObjectNameAndType(attr.get(), true);
                        os << "</li>";
                    }
                }
            }
        }
        os << "</ol>";
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

void writePrettyHTMLStateSetAttributeList(std::basic_ostream<char>& os, const SGIItemOsg * item, unsigned index)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item->object());
    const osg::StateSet::AttributeList & attributes = object->getAttributeList();
    if(!attributes.empty())
    {
        os << "<ol>";
        unsigned count = 0;
        for(osg::StateSet::AttributeList::const_iterator it = attributes.begin(); it != attributes.end(); it++, count++)
        {
            if(index == ~0u || index == count)
            {
                const osg::StateAttribute::TypeMemberPair & typememberpair = it->first;
                const osg::StateAttribute::Type & type = typememberpair.first;
                const unsigned int & member = typememberpair.second;
                const osg::StateSet::RefAttributePair & attrpair = it->second;
                const osg::ref_ptr<osg::StateAttribute> & attr = attrpair.first;
                const osg::StateAttribute::OverrideValue & value = attrpair.second;

                os << "<li>" << getStateAttributeTypeName(type) << " member=" << member << " value=" << glOverrideValueName(value) << " (0x" << std::hex << value << std::dec << ")<br/>";
                os << osg_helpers::getObjectNameAndType(attr.get(), true);
                os << "</li>";
            }
        }
        os << "</ol>";
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

void writePrettyHTMLStateSetUniformList(std::basic_ostream<char>& os, const SGIItemOsg * item, unsigned index)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item->object());
    const osg::StateSet::UniformList & uniforms = object->getUniformList();
    if(!uniforms.empty())
    {
        os << "<ol>";
        unsigned count = 0;
        for(osg::StateSet::UniformList::const_iterator it = uniforms.begin(); it != uniforms.end(); it++, count++)
        {
            if(index == ~0u || index == count)
            {
                const std::string & name = it->first;
                const osg::StateAttribute::OverrideValue & overridevalue = it->second.second;
                const osg::ref_ptr<osg::Uniform> & uniform = it->second.first;
                os << "<li>" << name << " override=" << glOverrideValueName(overridevalue) << "<br/>";
                os << osg_helpers::getObjectNameAndType(uniform.get(), true);
                os << "</li>";
            }
        }
        os << "</ol>";
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

void writePrettyHTMLStateSetDefineList(std::basic_ostream<char>& os, const SGIItemOsg * item, unsigned index)
{
    osg::StateSet * object = static_cast<osg::StateSet*>(item->object());
    const osg::StateSet::DefineList & defines = object->getDefineList();
    if(!defines.empty())
    {
        os << "<ol>";
        unsigned count = 0;
        for(osg::StateSet::DefineList::const_iterator it = defines.begin(); it != defines.end(); it++, count++)
        {
            if(index == ~0u || index == count)
            {
                const std::string & name = it->first;
                const std::string & value = it->second.first;
                const osg::StateAttribute::OverrideValue & overridevalue = it->second.second;
                os << "<li>" << name << '=' << value << " override=" << glOverrideValueName(overridevalue) << "</li>";
            }
        }
        os << "</ol>";
    }
    else
    {
        os << "&lt;empty&gt;" << std::endl;
    }
}

bool writePrettyHTMLImpl<osg::StateSet>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::StateSet * object = getObject<osg::StateSet,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining stateset properties
            os << "<tr><td>renderHint</td><td>" <<  (osg::StateSet::RenderingHint)object->getRenderingHint() << "</td></tr>" << std::endl;
            os << "<tr><td>renderBinMode</td><td>" << object->getRenderBinMode() << "</td></tr>" << std::endl;
            os << "<tr><td>renderBinNum</td><td>" << object->getBinNumber() << "</td></tr>" << std::endl;
            os << "<tr><td>renderBinName</td><td>" << object->getBinName() << "</td></tr>" << std::endl;
            os << "<tr><td>nested render bins</td><td>" << (object->getNestRenderBins()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>requiresUpdateTraversal</td><td>" << (object->requiresUpdateTraversal() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenRequiringUpdateTraversal</td><td>" << object->getNumChildrenRequiringUpdateTraversal() << "</td></tr>" << std::endl;
            os << "<tr><td>requiresEventTraversal</td><td>" << (object->requiresEventTraversal() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenRequiringEventTraversal</td><td>" << object->getNumChildrenRequiringEventTraversal() << "</td></tr>" << std::endl;
            
            os << "<tr><td>modes</td><td>" << std::endl;
            writePrettyHTMLStateSetModeList(os, item<SGIItemOsg>());
            os << "</td></tr>" << std::endl;

            os << "<tr><td>texture modes</td><td>" << std::endl;
            writePrettyHTMLStateSetTextureModeList(os, item<SGIItemOsg>(), ~0u);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>texture attributes</td><td>" << std::endl;
            writePrettyHTMLStateSetTextureAttributeList(os, item<SGIItemOsg>(), ~0u);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>attributes</td><td>";
            writePrettyHTMLStateSetAttributeList(os, item<SGIItemOsg>(), ~0u);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>uniforms</td><td>" << std::endl;
            writePrettyHTMLStateSetUniformList(os, item<SGIItemOsg>(), ~0u);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>defines</td><td>" << std::endl;
            writePrettyHTMLStateSetDefineList(os, item<SGIItemOsg>(), ~0u);
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStateSetModeList:
        writePrettyHTMLStateSetModeList(os, item<SGIItemOsg>());
        ret = true;
        break;
    case SGIItemTypeStateSetAttributeList:
        writePrettyHTMLStateSetAttributeList(os, item<SGIItemOsg>(), _item->number());
        ret = true;
        break;
    case SGIItemTypeStateSetTextureModeList:
        writePrettyHTMLStateSetTextureModeList(os, item<SGIItemOsg>(), _item->number());
        ret = true;
        break;
    case SGIItemTypeStateSetTextureAttributeLists:
        writePrettyHTMLStateSetTextureAttributeList(os, item<SGIItemOsg>(), _item->number());
        ret = true;
        break;
    case SGIItemTypeStateSetUniformList:
        writePrettyHTMLStateSetUniformList(os, item<SGIItemOsg>(), _item->number());
        ret = true;
        break;
    case SGIItemTypeStateSetDefineList:
        writePrettyHTMLStateSetDefineList(os, item<SGIItemOsg>(), _item->number());
        ret = true;
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>update callback</td><td>" << osg_helpers::getObjectNameAndType(object->getUpdateCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>event callback</td><td>" << osg_helpers::getObjectNameAndType(object->getEventCallback(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStateSetEffective:
        {
            unsigned itemNumber = _item->number();
            if(itemNumber == ~0u)
            {
                os << "<ol>" << std::endl;
                for(unsigned n = 0; n < object->getNumParents(); n++)
                {
                    const osg::Object * parent = object->getParent(n);
                    os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << "</li>" << std::endl;
                }
                os << "</ol>" << std::endl;
            }
            else
            {
                osg::Node * parentNode = object->getParent(itemNumber);
                osg::NodePathList parentPath = parentNode->getParentalNodePaths();
                for(const osg::NodePath & path : parentPath)
                {
                    osg::ref_ptr<osg::StateSet> ss = buildEffectiveStateSet(path, object);
                    SGIHostItemOsg item(ss.get());
                    _hostInterface->writePrettyHTML(os, &item, true);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>" << std::endl;
            for(unsigned n = 0; n < object->getNumParents(); n++)
            {
                const osg::Object * parent = object->getParent(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << "</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& fout, const osg::State::ModeStack & m)
{
    fout << "<pre>valid = " << m.valid << std::endl;
    fout << "changed = " << m.changed << std::endl;
    fout << "last_applied_value = " << m.last_applied_value << std::endl;
    fout << "global_default_value = " << m.global_default_value << std::endl;
    fout << "valueVec { " << std::endl;
    for (osg::State::ModeStack::ValueVec::const_iterator itr = m.valueVec.begin();
        itr != m.valueVec.end();
        ++itr)
    {
        if (itr != m.valueVec.begin()) fout << ", ";
        fout << *itr;
    }
    fout << " }</pre>" << std::endl;
    return fout;
}


inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::TypeMemberPair & t)
{
    os << '(' << t.first << ',' << t.second << ')';
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& fout, const osg::State::UniformStack & u)
{
    fout << "<pre>UniformVec { ";
    for (osg::State::UniformStack::UniformVec::const_iterator itr = u.uniformVec.begin();
        itr != u.uniformVec.end();
        ++itr)
    {
        if (itr != u.uniformVec.begin()) fout << ", ";
        fout << "(" << osg_helpers::getObjectNameAndType(itr->first) << ", " << itr->second << ")";
    }
    fout << " }</pre>" << std::endl;
    return fout;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& fout, const osg::State::AttributeStack & u)
{
    fout << "<pre>changed = " << u.changed << std::endl;
    fout << "last_applied_attribute = " << u.last_applied_attribute;
    if (u.last_applied_attribute) fout << ", " << u.last_applied_attribute->className() << ", " << u.last_applied_attribute->getName() << std::endl;
    fout << "last_applied_shadercomponent = " << u.last_applied_shadercomponent << std::endl;
    if (u.last_applied_shadercomponent)  fout << ", " << u.last_applied_shadercomponent->className() << ", " << u.last_applied_shadercomponent->getName() << std::endl;
    fout << "global_default_attribute = " << u.global_default_attribute.get() << std::endl;
    fout << "attributeVec { ";
    for (osg::State::AttributeVec::const_iterator itr = u.attributeVec.begin();
        itr != u.attributeVec.end();
        ++itr)
    {
        if (itr != u.attributeVec.begin()) fout << ", ";
        fout << "(" << osg_helpers::getObjectNameAndType(itr->first) << ", " << itr->second << ")";
    }
    fout << " }</pre>" << std::endl;
    return fout;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& fout, const osg::State::DefineStack & u)
{
    fout << "<pre>changed = " << u.changed << std::endl;
    fout << "defineVec { ";
    for (osg::State::DefineStack::DefineVec::const_iterator itr = u.defineVec.begin();
        itr != u.defineVec.end();
        ++itr)
    {
        if (itr != u.defineVec.begin()) fout << ", ";
        fout << "(" << itr->first << ", " << itr->second << ")";
    }
    fout << " }</pre>" << std::endl;
    return fout;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::VertexAttribAlias & u)
{
    return os << '[' << u._location << "]: " << u._declaration << " " << u._glName << "=" << u._osgName;
}

bool writePrettyHTMLImpl<osg::State>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::State * object = getObject<osg::State,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining state properties
            os << "<tr><td>contextID</td><td>" <<  object->getContextID() << "</td></tr>" << std::endl;
            os << "<tr><td>GPU time</td><td>" << object->getGpuTime() << "</td></tr>" << std::endl;
            os << "<tr><td>shaderCompositionEnabled</td><td>" << (object->getShaderCompositionEnabled()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>projectionMatrix</td><td>";
            writePrettyHTML(os, object->getProjectionMatrix(), MatrixUsageTypePerspective, (const osg::Node*)NULL);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>modelViewMatrix</td><td>";
            writePrettyHTML(os, object->getModelViewMatrix(), MatrixUsageTypeModelView, (const osg::Node*)NULL);
            os << "</td></tr>" << std::endl;

            //os << "<tr><td>viewFrustum</td><td>" <<  object->getViewFrustum() << "</td></tr>" << std::endl;

            os << "<tr><td>useModelViewAndProjectionUniforms</td><td>" << (object->getUseModelViewAndProjectionUniforms()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>useVertexAttributeAliasing</td><td>" << (object->getUseVertexAttributeAliasing()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isVertexBufferObjectSupported</td><td>" << (object->isVertexBufferObjectSupported() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>isVertexArrayObjectSupported</td><td>" << (object->isVertexArrayObjectSupported() ? "true" : "false") << "</td></tr>" << std::endl;

            os << "<tr><td>Current VAO</td><td>" << object->getCurrentVertexArrayObject() << "</td></tr>" << std::endl;
            os << "<tr><td>active texture unit</td><td>" << object->getActiveTextureUnit() << "</td></tr>" << std::endl;
            os << "<tr><td>client active texture unit</td><td>" << object->getClientActiveTextureUnit() << "</td></tr>" << std::endl;

            os << "<tr><td>vertexAlias</td><td>" << object->getVertexAlias() << "</td></tr>" << std::endl;
            os << "<tr><td>normalAlias</td><td>" << object->getNormalAlias() << "</td></tr>" << std::endl;
            os << "<tr><td>colorAlias</td><td>" << object->getColorAlias() << "</td></tr>" << std::endl;
            os << "<tr><td>secondaryColorAlias</td><td>" << object->getSecondaryColorAlias() << "</td></tr>" << std::endl;
            os << "<tr><td>fogCoordAlias</td><td>" << object->getFogCoordAlias() << "</td></tr>" << std::endl;
            os << "<tr><td>texCoordAliasList</td><td><ol>";
            for(auto t : object->getTexCoordAliasList())
                os << "<li>" << t << "</li>";
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>attributeBindingList</td><td>";
            for (auto t : object->getAttributeBindingList())
                os << "<li>" << t.first << "=" << t.second << "</li>";
            os << "</td></tr>" << std::endl;

            os << "<tr><td>dyn object count</td><td>" << object->getDynamicObjectCount() << "</td></tr>" << std::endl;
            os << "<tr><td>max texture pool size</td><td>" << object->getMaxTexturePoolSize() << "</td></tr>" << std::endl;
            os << "<tr><td>max buffer object pool size</td><td>" << object->getMaxBufferObjectPoolSize() << "</td></tr>" << std::endl;

            os << "<tr><td>check GL errors</td><td>" << object->getCheckForGLErrors() << "</td></tr>" << std::endl;
            
            os << "<tr><td>modeMap</td><td>" << object->getModeMap().size() << "&nbsp;entries</td></tr>" << std::endl;
            os << "<tr><td>attributeMap</td><td>" << object->getAttributeMap().size() << "&nbsp;entries</td></tr>" << std::endl;
            os << "<tr><td>uniformMap</td><td>" << object->getUniformMap().size() << "&nbsp;entries</td></tr>" << std::endl;
            os << "<tr><td>defineMap</td><td>" << object->getDefineMap().map.size() << "&nbsp;entries</td></tr>" << std::endl;
            os << "<tr><td>textureModeMapList</td><td>" << object->getTextureModeMapList().size() << "&nbsp;entries</td></tr>" << std::endl;
            os << "<tr><td>textureAttributeMapList</td><td>" << object->getTextureAttributeMapList().size() << "&nbsp;entries</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeStateModeMap:
        {
            const osg::State::ModeMap& modemap = object->getModeMap();
            os << "<ul>";
            for (auto it : modemap)
            {
                os << "<li>" << it.first << "=" << it.second << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeStateAttributeMap:
        {
            const osg::State::AttributeMap& attributemap = object->getAttributeMap();
            os << "<ul>";
            for (auto it : attributemap)
            {
                os << "<li>" << it.first << "=" << it.second << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeStateUniformMap:
        {
            const osg::State::UniformMap& uniformmap = object->getUniformMap();
            os << "<ul>";
            os << "<li>ModelViewMatrixUniform=" << osg_helpers::getObjectNameAndType(object->getModelViewMatrixUniform()) << "</li>";
            os << "<li>projectionMatrixUniform=" << osg_helpers::getObjectNameAndType(object->getProjectionMatrixUniform()) << "</li>";
            os << "<li>modelViewProjectionMatrixUniform=" << osg_helpers::getObjectNameAndType(object->getModelViewProjectionMatrixUniform()) << "</li>";
            os << "<li>normalMatrixUniform=" << osg_helpers::getObjectNameAndType(object->getNormalMatrixUniform()) << "</li>";
            for (auto it : uniformmap)
            {
                os << "<li>" << it.first << "=" << it.second << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeStateDefineMap:
        {
            const osg::State::DefineMap& definemap = object->getDefineMap();
            os << "<ul>changed=" << (definemap.changed ? "true" : "false") << "<br/>";
            for (auto it : definemap.map)
            {
                os << "<li>" << it.first << "=" << it.second << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeStateTextureModeMapList:
        {
            const osg::State::TextureModeMapList& texturemodemaplist = object->getTextureModeMapList();
            os << "<ol>";
            for (auto it : texturemodemaplist)
            {
                os << "<li><ul>";
                for (auto it_inner : it)
                {
                    os << "<li>" << it_inner.first << "=" << it_inner.second << "</li>";
                }
                os << "</ul></li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeStateTextureAttributeMapList:
        {
            const osg::State::TextureAttributeMapList& textureattributemaplist = object->getTextureAttributeMapList();
            os << "<ol>";
            for (auto it : textureattributemaplist)
            {
                os << "<li><ul>";
                for (auto it_inner : it)
                {
                    os << "<li>" << it_inner.first << "=" << it_inner.second << "</li>";
                }
                os << "</ul></li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Geode>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Geode * object = getObject<osg::Geode,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add node properties first
            callNextHandler(os);

#if OSG_VERSION_LESS_THAN(3,4,0)
			// add remaining geode properties
            os << "<tr><td>drawables</td><td>";
            os << "<ol>" << std::endl;
            unsigned numDrawables = object->getNumDrawables();
            for(unsigned i = 0; i < numDrawables; i++)
            {
                const osg::Drawable * drawable = object->getDrawable(i);
                os << "<li>" << osg_helpers::getObjectName(drawable) << " (" << osg_helpers::getObjectTypename(drawable) << ")</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;
#endif // OSG_VERSION_LESS_THAN(3,4,0)

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
#if OSG_VERSION_LESS_THAN(3,4,0)
	case SGIItemTypeChilds:
        {
            os << "<ol>";

            unsigned numDrawables = object->getNumDrawables();
            for(unsigned i = 0; i < numDrawables; i++)
            {
                const osg::Drawable * drawable = object->getDrawable(i);
                os << "<li>" << osg_helpers::getObjectName(drawable) << "&nbsp;(" << osg_helpers::getObjectTypename(drawable) << ")</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
#endif // OSG_VERSION_LESS_THAN(3,4,0)
	default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::ProjectionResizePolicy t)
{
    switch(t)
    {
    case osg::Camera::FIXED: os << "FIXED"; break;
    case osg::Camera::HORIZONTAL: os << "HORIZONTAL"; break;
    case osg::Camera::VERTICAL: os << "VERTICAL"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::TransformOrder t)
{
    switch(t)
    {
    case osg::Camera::PRE_MULTIPLY: os << "PRE_MULTIPLY"; break;
    case osg::Camera::POST_MULTIPLY: os << "POST_MULTIPLY"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::RenderOrder t)
{
    switch(t)
    {
    case osg::Camera::PRE_RENDER: os << "PRE_RENDER"; break;
    case osg::Camera::NESTED_RENDER: os << "NESTED_RENDER"; break;
    case osg::Camera::POST_RENDER: os << "POST_RENDER"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::RenderTargetImplementation t)
{
    switch(t)
    {
    case osg::Camera::FRAME_BUFFER_OBJECT: os << "FRAME_BUFFER_OBJECT"; break;
    case osg::Camera::PIXEL_BUFFER_RTT: os << "PIXEL_BUFFER_RTT"; break;
    case osg::Camera::PIXEL_BUFFER: os << "PIXEL_BUFFER"; break;
    case osg::Camera::FRAME_BUFFER: os << "FRAME_BUFFER"; break;
#if OSG_MIN_VERSION_REQUIRED(3,3,3)
    case osg::Camera::SEPARATE_WINDOW: os << "SEPARATE_WINDOW"; break;
#else
    case osg::Camera::SEPERATE_WINDOW: os << "SEPERATE_WINDOW"; break;
#endif
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Camera::BufferComponent t)
{
    switch(t)
    {
    case osg::Camera::DEPTH_BUFFER: os << "DEPTH_BUFFER"; break;
    case osg::Camera::STENCIL_BUFFER: os << "STENCIL_BUFFER"; break;
    case osg::Camera::PACKED_DEPTH_STENCIL_BUFFER: os << "PACKED_DEPTH_STENCIL_BUFFER"; break;
    case osg::Camera::COLOR_BUFFER: os << "COLOR_BUFFER"; break;
    default:
        os << "COLOR_BUFFER" << (t-osg::Camera::COLOR_BUFFER0);
        break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::CullSettings::ComputeNearFarMode t)
{
    switch(t)
    {
    case osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR: os << "DO_NOT_COMPUTE_NEAR_FAR"; break;
    case osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES: os << "COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES"; break;
    case osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES: os << "COMPUTE_NEAR_FAR_USING_PRIMITIVES"; break;
    case osg::CullSettings::COMPUTE_NEAR_USING_PRIMITIVES: os << "COMPUTE_NEAR_USING_PRIMITIVES"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::string cullingModeToString(osg::CullSettings::CullingMode t)
{
    std::string s;
    if(t == 0)
        s = "false";
    else
    {
        if(t & osg::CullSettings::VIEW_FRUSTUM_SIDES_CULLING)
            s += "|frustum";
        if(t & osg::CullSettings::NEAR_PLANE_CULLING)
            s += "|nearPlane";
        if(t & osg::CullSettings::FAR_PLANE_CULLING)
            s += "|farPlane";
        if(t & osg::CullSettings::SMALL_FEATURE_CULLING)
            s += "|smallFeature";
        if(t & osg::CullSettings::SHADOW_OCCLUSION_CULLING)
            s += "|shadowOcclusion";
        if(t & osg::CullSettings::CLUSTER_CULLING)
            s += "|cluster";
        s = s.substr(1);
    }
    return s;
}

namespace {
    class CullStackAccess : public osg::CullStack
    {
    public:
        float getFrustumVolumeNoCompute() const {
            return _frustumVolume;
        }
    };
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::CullStack * object_)
{
    const CullStackAccess* object = static_cast<const CullStackAccess*>(object_);
    os << "<tr><td>frustumVolume</td><td>" << object->getFrustumVolumeNoCompute() << "</td></tr>" << std::endl;
    os << "<tr><td>eyeLocal</td><td>" << object->getEyeLocal() << "</td></tr>" << std::endl;
    os << "<tr><td>viewPointLocal</td><td>" << object->getViewPointLocal() << "</td></tr>" << std::endl;
    if (!object->getModelViewStack().empty())
    {
        os << "<tr><td>upLocal</td><td>" << object->getUpLocal() << "</td></tr>" << std::endl;
        os << "<tr><td>lookVectorLocal</td><td>" << object->getLookVectorLocal() << "</td></tr>" << std::endl;
    }
    os << "<tr><td>eyeLocal</td><td>" << object->getEyeLocal() << "</td></tr>" << std::endl;

    os << "<tr><td>viewport</td><td><table broder=\"1\">";
    osg_helpers::writePrettyHTML(os, object->getViewport());
    os << "</table></td></tr>" << std::endl;

    os << "<tr><td>projectionMatrix</td><td>";
    writePrettyHTML(os, object->getProjectionMatrix(), MatrixUsageTypePerspective);
    os << "</td></tr>" << std::endl;
    os << "<tr><td>modelViewMatrix</td><td>";
    writePrettyHTML(os, object->getModelViewMatrix(), MatrixUsageTypeModelView);
    os << "</td></tr>" << std::endl;
    os << "<tr><td>windowMatrix</td><td>";
    writePrettyHTML(os, object->getWindowMatrix(), MatrixUsageTypeWindow);
    os << "</td></tr>" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::CullSettings * object)
{
    os << "<tr><td>inheritanceMaskActionOnAttributeSetting</td><td>" << object->getInheritanceMaskActionOnAttributeSetting() << "</td></tr>" << std::endl;
    os << "<tr><td>impostersActive</td><td>" << (object->getImpostorsActive()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>impostersPixelErrorThreshold</td><td>" << object->getImpostorPixelErrorThreshold() << "</td></tr>" << std::endl;
    os << "<tr><td>depthSortImpostorSprites</td><td>" << (object->getDepthSortImpostorSprites()?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>numberOfFrameToKeepImpostorSprites</td><td>" << object->getNumberOfFrameToKeepImpostorSprites() << "</td></tr>" << std::endl;
    os << "<tr><td>computeNearFarMode</td><td>" << object->getComputeNearFarMode() << "</td></tr>" << std::endl;
    os << "<tr><td>nearFarRatio</td><td>" << object->getNearFarRatio() << "</td></tr>" << std::endl;
    os << "<tr><td>cullingMode</td><td>" << cullingModeToString(object->getCullingMode()) << "</td></tr>" << std::endl;
    os << "<tr><td>cullMask</td><td>" << castToEnumValueString<osgNodeMask>(object->getCullMask()) << "</td></tr>" << std::endl;
    if(object->getCullMask() != object->getCullMaskLeft() || object->getCullMask() != object->getCullMaskRight())
    {
        os << "<tr><td>cullMaskLeft</td><td>" << castToEnumValueString<osgNodeMask>(object->getCullMaskLeft()) << "</td></tr>" << std::endl;
        os << "<tr><td>cullMaskRight</td><td>" << castToEnumValueString<osgNodeMask>(object->getCullMaskRight()) << "</td></tr>" << std::endl;
    }
    os << "<tr><td>lodScale</td><td>" << object->getLODScale() << "</td></tr>" << std::endl;
    os << "<tr><td>smallFeatureCullingPixelSize</td><td>" << object->getSmallFeatureCullingPixelSize() << "</td></tr>" << std::endl;
}


void writePrettyHTML(std::basic_ostream<char>& os, const osg::Camera::Attachment & attachment)
{
    os << "<tr><td>internalFormat</td><td>" << attachment._internalFormat << "</td></tr>" << std::endl;
    os << "<tr><td>image</td><td>" << osg_helpers::getObjectNameAndType(attachment._image.get(), true) << "</td></tr>" << std::endl;
    os << "<tr><td>texture</td><td>" << osg_helpers::getObjectNameAndType(attachment._texture.get(), true) << "</td></tr>" << std::endl;
    os << "<tr><td>width</td><td>" << attachment.width() << "</td></tr>" << std::endl;
    os << "<tr><td>height</td><td>" << attachment.height() << "</td></tr>" << std::endl;
    os << "<tr><td>depth</td><td>" << attachment.depth() << "</td></tr>" << std::endl;
    os << "<tr><td>level</td><td>" << attachment._level << "</td></tr>" << std::endl;
    os << "<tr><td>face</td><td>" << attachment._face << "</td></tr>" << std::endl;
    os << "<tr><td>mipMapGeneration</td><td>" << attachment._mipMapGeneration << "</td></tr>" << std::endl;
    os << "<tr><td>multisampleSamples</td><td>" << attachment._multisampleSamples << "</td></tr>" << std::endl;
    os << "<tr><td>multisampleColorSamples</td><td>" << attachment._multisampleColorSamples << "</td></tr>" << std::endl;
}


bool writePrettyHTMLImpl<osg::Camera>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Camera * object = getObject<osg::Camera,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Transform properties first
            callNextHandler(os);

            // add the cull settings
            writePrettyHTML(os, static_cast<osg::CullSettings*>(object));

            // add remaining camera properties
            os << "<tr><td>view</td><td>" << osg_helpers::getObjectNameAndType(object->getView(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>graphicsContext</td><td>" << osg_helpers::getObjectNameAndType(object->getGraphicsContext(), true) << "</td></tr>" << std::endl;

            os << "<tr><td>clear color</td><td>"
                << vec4fToHtmlColor(object->getClearColor())
                << "</td></tr>" << std::endl;
            os << "<tr><td>clear mask</td><td>" << clearMaskToString(object->getClearMask()) << "</td></tr>" << std::endl;
            os << "<tr><td>clear accum</td><td>"
                << vec4fToHtmlColor(object->getClearAccum())
                << "</td></tr>" << std::endl;
            os << "<tr><td>clearDepth</td><td>" << object->getClearDepth() << "</td></tr>" << std::endl;
            os << "<tr><td>clearStencil</td><td>" << object->getClearStencil()<< "</td></tr>" << std::endl;

            os << "<tr><td>colorMask</td><td>" << osg_helpers::getObjectNameAndType(object->getColorMask(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>viewport</td><td>" << osg_helpers::getObjectNameAndType(object->getViewport(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>transformOrder</td><td>" << object->getTransformOrder() << "</td></tr>" << std::endl;

            os << "<tr><td>projectionMatrix</td><td>";
            writePrettyHTML(os, object->getProjectionMatrix(), MatrixUsageTypePerspective, object);
            os << "</td></tr>" << std::endl;
            os << "<tr><td>viewMatrix</td><td>";
            writePrettyHTML(os, object->getViewMatrix(), MatrixUsageTypeLookAt, object);
            os << "</td></tr>" << std::endl;

            os << "<tr><td>allowEventFocus</td><td>" << (object->getAllowEventFocus()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>RTT</td><td>" << (object->isRenderToTextureCamera()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>renderOrder</td><td>" << object->getRenderOrder() << "</td></tr>" << std::endl;
            os << "<tr><td>renderOrderNum</td><td>" << object->getRenderOrderNum() << "</td></tr>" << std::endl;
            os << "<tr><td>renderTargetImpl</td><td>" << object->getRenderTargetImplementation() << "</td></tr>" << std::endl;
            os << "<tr><td>renderTargetFallback</td><td>" << object->getRenderTargetFallback() << "</td></tr>" << std::endl;
            os << "<tr><td>drawBuffer</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDrawBuffer()) << "</td></tr>" << std::endl;
            os << "<tr><td>readBuffer</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getReadBuffer()) << "</td></tr>" << std::endl;
            os << "<tr><td>projResizePolicy</td><td>" << object->getProjectionResizePolicy() << "</td></tr>" << std::endl;

            os << "<tr><td>bufferAttachmentMap</td><td>";
            const osg::Camera::BufferAttachmentMap & bufferAttachmentMap = object->getBufferAttachmentMap();
            if (bufferAttachmentMap.empty())
            {
                os << "<i>empty</i>";
            }
            else
            {
                os << "<ul>";
                for (auto it = bufferAttachmentMap.begin(); it != bufferAttachmentMap.end(); ++it)
                {
                    const osg::Camera::BufferComponent & buffercomponent = it->first;
                    os << "<li>" << buffercomponent << "</li>";
                }
                os << "</ul>";
            }
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add transform callbacks first
            callNextHandler(os);

            // add osg::Camera callbacks now
            os << "<tr><td>initial draw callback</td><td>" << osg_helpers::getObjectNameAndType(object->getInitialDrawCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>pre-draw callback</td><td>" << osg_helpers::getObjectNameAndType(object->getPreDrawCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>post-draw callback</td><td>" << osg_helpers::getObjectNameAndType(object->getPostDrawCallback(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>final draw callback</td><td>" << osg_helpers::getObjectNameAndType(object->getFinalDrawCallback(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCameaBufferAttachments:
        {

            const osg::Camera::BufferAttachmentMap & bufferAttachmentMap = object->getBufferAttachmentMap();
            if(_item->number() == ~0u)
            {
                if(_table)
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                for(auto it = bufferAttachmentMap.begin(); it != bufferAttachmentMap.end(); ++it)
                {
                    const osg::Camera::BufferComponent & buffercomponent = it->first;

                    os << "<tr><td>" << buffercomponent << "</td><td>";
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                    const osg::Camera::Attachment & attachment = it->second;
                    writePrettyHTML(os, attachment);

                    os << "</table>" << std::endl;
                    os << "</td></tr>" << std::endl;
                }
            }
            else
            {
                const osg::Camera::BufferComponent buffercomponent = (osg::Camera::BufferComponent)_item->number();
                os << buffercomponent << "<br/>";
                if(_table)
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                auto it = bufferAttachmentMap.find(buffercomponent);
                if(it != bufferAttachmentMap.end())
                {
                    const osg::Camera::Attachment & attachment = it->second;
                    writePrettyHTML(os, attachment);
                }
            }

            if(_table)
                os << "</table>" << std::endl;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<osg::Node>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Node * object = getObject<osg::Node,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            const NodeAccess * access = (const NodeAccess*)object;
            // add remaining node properties
            os << "<tr><td>nodeMask</td><td>" << castToEnumValueString<osgNodeMask>(object->getNodeMask()) << "</td></tr>" << std::endl;
            os << "<tr><td>cullingActive</td><td>" << (object->getCullingActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>isCullingActive</td><td>" << (object->isCullingActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenWithCullingDisabled</td><td>" << object->getNumChildrenWithCullingDisabled() << "</td></tr>" << std::endl;
            os << "<tr><td>numChildrenWithOccluderNodes</td><td>" << object->getNumChildrenWithOccluderNodes() << "</td></tr>" << std::endl;
            os << "<tr><td>numUpdateTraversal</td><td>" << object->getNumChildrenRequiringUpdateTraversal() << "</td></tr>" << std::endl;
            os << "<tr><td>numEventTraversal</td><td>" << object->getNumChildrenRequiringEventTraversal() << "</td></tr>" << std::endl;
            os << "<tr><td>stateSet</td><td>" << osg_helpers::getObjectNameAndType(object->getStateSet()) << "</td></tr>" << std::endl;
            os << "<tr><td>initialBound</td><td>" << object->getInitialBound() << "</td></tr>" << std::endl;
            os << "<tr><td>bound</td><td>" << access->getBoundNoCompute() << "</td></tr>" << std::endl;
            os << "<tr><td>isBoundingSphereComputed</td><td>" << (access->isBoundingSphereComputed()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object callbacks first
            callNextHandler(os);

            // add osg::Node callbacks now
#if OSG_VERSION_GREATER_THAN(3,3,1)
            const osg::Callback * cb;
#else
            const osg::NodeCallback * cb;
#endif
            os << "<tr><td>Event callbacks</td><td><ol>" << std::endl;
            cb = object->getEventCallback();
            while(cb)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(cb) << "</li>" << std::endl;
                cb = cb->getNestedCallback();
            }
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>Update callbacks</td><td><ol>" << std::endl;
            cb = object->getUpdateCallback();
            while(cb)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(cb) << "</li>" << std::endl;
                cb = cb->getNestedCallback();
            }
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>Cull callbacks</td><td><ol>" << std::endl;
            cb = object->getCullCallback();
            while(cb)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(cb) << "</li>" << std::endl;
                cb = cb->getNestedCallback();
            }
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>Compute boundingSphere callback</td><td>" << osg_helpers::getObjectNameAndType(object->getComputeBoundingSphereCallback()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>" << std::endl;
            for(unsigned n = 0; n < object->getNumParents(); n++)
            {
                const osg::Group * parent = object->getParent(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(parent, true) << ")</li>" << std::endl;
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeParentalNodePath:
        {
            osg::NodePathList parentalNodePaths = object->getParentalNodePaths();
            unsigned numPath = _item->number();
            unsigned parentalNodePathNum = 0;
            if(numPath == ~0u)
            {
                os << "<table border=\'1\' align=\'left\'>" << std::endl;
                os << "<tr><td>" << osg_helpers::getObjectNameAndType(object, true) << " <i>" << parentalNodePaths.size() << "</i></td></tr>" << std::endl;
                unsigned num = 0;
                for(osg::NodePathList::const_iterator itPathList = parentalNodePaths.begin(); itPathList != parentalNodePaths.end(); ++itPathList, ++num)
                {
                    const osg::NodePath & nodePath = *itPathList;
                    os << "<tr><td>ParentalNodePath <i>#" << numPath << "</i><br/>" << std::endl;
                    osg_helpers::writePrettyHTML(os, nodePath, true);
                    os << "</td></tr>" << std::endl;
                }
                os << "</table>" << std::endl;
            }
            else
            {
                const osg::NodePathList parentalNodePaths = object->getParentalNodePaths();
                if(numPath >= parentalNodePaths.size())
                {
                    os << "<p>ParentalNodePath <i>#" << numPath << "</i> for <b>" << osg_helpers::getObjectNameAndType(object, true) << "</b> is unavailable.</p>" << std::endl;
                }
                else
                {
                    os << "<p>ParentalNodePath <i>#" << numPath << "</i> for <b>" << osg_helpers::getObjectNameAndType(object, true) << "</b></p>" << std::endl;
                    unsigned parentalNodePathNum = 0;
                    for(osg::NodePathList::const_iterator itPathList = parentalNodePaths.begin(); itPathList != parentalNodePaths.end(); itPathList++, parentalNodePathNum++)
                    {
                        if(parentalNodePathNum == numPath)
                        {
                            const osg::NodePath & nodePath = *itPathList;
                            osg_helpers::writePrettyHTML(os, nodePath, true);
                            break;
                        }
                    }
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeStatistics:
        {
            unsigned contextID = osg_helpers::findContextID(object);
            if (contextID == ~0u)
                os << "<p>Unable to find context id for <b>" << osg_helpers::getObjectNameAndType(object, true) << "</b></p>" << std::endl;

            osg_helpers::StatisticsVisitor sv(contextID);
            object->accept(sv);
            sv.printHTML(os);

            ret = true;
        }
        break;
    case SGIItemTypeStatisticsStateSets:
        {
            unsigned contextID = osg_helpers::findContextID(object);
            if (contextID == ~0u)
                os << "<p>Unable to find context id for <b>" << osg_helpers::getObjectNameAndType(object, true) << "</b></p>" << std::endl;

            osg_helpers::StatisticsVisitor sv(contextID);
            object->accept(sv);
            if (itemNumber() == ~0u)
            {
                sv.printStateSetsHTML(os);
            }
            else
            {
            }
            ret = true;
        }
        break;
    case SGIItemTypeStatisticsStateAttributes:
        {
            unsigned contextID = osg_helpers::findContextID(object);
            osg_helpers::StatisticsVisitor sv(contextID);
            object->accept(sv);
            if (this->itemNumber() == ~0u)
            {
                os << "<table border=\'1\' align=\'left\'><tr><th>Attribute</th><th>Count</th></tr>" << std::endl;
                for(unsigned n = 0; n < osg_helpers::StatisticsVisitor::MaxStateAttributeType; ++n)
                {
                    std::stringstream ss;
                    os << "<tr><td>" << getStateAttributeTypeName((osg::StateAttribute::Type)n) << "</td><td>" << sv.getNumberOfStateAttributes(n) << "</td></tr>";
                }
                os << "</table>";
            }
            else
            {
                os << "<ul>";
                osg::StateAttribute::Type type = (osg::StateAttribute::Type)itemNumber();
                for (unsigned n = 0; n < sv.getNumberOfStateAttributes(type); ++n)
                {
                    os << "<li>" << osg_helpers::getObjectNameAndType(sv.getStateAttribute(type, n), true) << "</li>" << std::endl;
                }
                os << "</ul>";
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::NodeCallback>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::NodeCallback * object = getObject<osg::NodeCallback,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining NodeCallback properties
            os << "<tr><td>nested</td><td>";
#if OSG_VERSION_GREATER_THAN(3,3,1)
            const osg::Callback * nested = object->getNestedCallback();
#else
            const osg::NodeCallback * nested = object->getNestedCallback();
#endif
            os << osg_helpers::getObjectNameAndType(nested);
            os << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

void writePrettyHTMLViewSlave(std::basic_ostream<char>& os, unsigned num, const osg::View::Slave & object)
{
    os << "<table border=\'1\' align=\'left\'><tr><th>Slave " << num << " Field</th><th>Value</th></tr>" << std::endl;

    os << "<tr><td>camera</td><td>";
    os << osg_helpers::getObjectName(object._camera.get()) << " (" << osg_helpers::getObjectTypename(object._camera.get()) << ")" << std::endl;
    os << "</td></tr>" << std::endl;

    os << "<tr><td>projectionOffset</td><td>";
    writePrettyHTML(os, object._projectionOffset, MatrixUsageTypePerspective);
    os << "</td></tr>" << std::endl;
    os << "<tr><td>viewOffset</td><td>";
    writePrettyHTML(os, object._viewOffset, MatrixUsageTypeGeneric);
    os << "</td></tr>" << std::endl;

    os << "<tr><td>useMastersSceneData</td><td>" << (object._useMastersSceneData?"true":"false") << "</td></tr>" << std::endl;
    os << "<tr><td>updateSlaveCallback</td><td>" << (void*)object._updateSlaveCallback.get() << "</td></tr>" << std::endl;

    os << "</table>" << std::endl;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::View::LightingMode t)
{
    switch(t)
    {
    case osg::View::NO_LIGHT: os << "nolight"; break;
    case osg::View::HEADLIGHT: os << "headlight"; break;
    case osg::View::SKY_LIGHT: os << "skylight"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::View>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::View * object = getObject<osg::View,SGIItemOsg,DynamicCaster>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            bool brief = true;

            // add object properties first
            callNextHandler(os);

            // add remaining View properties
            os << "<tr><td>lighting mode</td><td>" << object->getLightingMode() << "</td></tr>" << std::endl;
            os << "<tr><td>light</td><td>" << osg_helpers::getObjectNameAndType(object->getLight(), true) << "</td></tr>" << std::endl;
            os << "<tr><td>camera</td><td>" << osg_helpers::getObjectNameAndType(object->getCamera(), true) << "</td></tr>" << std::endl;
            const osg::FrameStamp * fs = object->getFrameStamp();
            os << "<tr><td>frame stamp</td><td>" << osg_helpers::getObjectNameAndType(object->getFrameStamp()) << "</td></tr>" << std::endl;
            if(fs)
            {
                os << "<tr><td>frame number</td><td>" << fs->getFrameNumber() << "</td></tr>" << std::endl;
                os << "<tr><td>simulation time</td><td>" << fs->getSimulationTime() << "</td></tr>" << std::endl;
                os << "<tr><td>reference time</td><td>" << fs->getReferenceTime() << "</td></tr>" << std::endl;
            }
            os << "<tr><td>slaves</td><td>" << object->getNumSlaves() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeSlaves:
        {
            unsigned num = object->getNumSlaves();
            for(unsigned i = 0; i < num; i++)
            {
                const osg::View::Slave & slave = object->getSlave(i);
                writePrettyHTMLViewSlave(os, i, slave);
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::FrameStamp>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::FrameStamp * object = getObject<osg::FrameStamp,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining  properties
            os << "<tr><td>frame number</td><td>" << object->getFrameNumber() << "</td></tr>" << std::endl;
            os << "<tr><td>simulation time</td><td>" << object->getSimulationTime() << "</td></tr>" << std::endl;
            os << "<tr><td>reference time</td><td>" << object->getReferenceTime() << "</td></tr>" << std::endl;
            os << "<tr><td>calendar time</td><td>";
            struct tm caltime;
            object->getCalendarTime(caltime);
            if (caltime.tm_mday >= 1 && caltime.tm_mday <= 31)
            {
                char sz[64];
                ::strftime(sz, sizeof(sz) / sizeof(sz[0]), "%c", &caltime);
                os << sz;
            }
            else
                os << "N/A";
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::RefMatrixd>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::RefMatrixd * object = getObject<osg::RefMatrixd,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining  properties
            os << "<tr><td>matrix</td><td>";
            writePrettyHTML(os, *static_cast<osg::Matrixd*>(object), MatrixUsageTypeGeneric, (osg::Drawable*)NULL);
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::RefMatrixf>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::RefMatrixf * object = getObject<osg::RefMatrixf,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining  properties
            os << "<tr><td>matrix</td><td>";
            writePrettyHTML(os, *static_cast<osg::Matrixf*>(object), MatrixUsageTypeGeneric, (osg::Drawable*)NULL);
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::UserDataContainer>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::UserDataContainer * object = getObject<osg::UserDataContainer,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            // add remaining UserDataContainer properties
            os << "<tr><td>descriptions</td><td><ol>";
            const osg::UserDataContainer::DescriptionList& descriptions = object->getDescriptions();
            for(osg::UserDataContainer::DescriptionList::const_iterator it = descriptions.begin(); it != descriptions.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>user objects</td><td><ol>";
            unsigned numUserObjects = object->getNumUserObjects();
            for (unsigned n = 0; n < numUserObjects; n++)
            {
                osg::Object * uobj = object->getUserObject(n);
                os << "<li>" << osg_helpers::getObjectNameAndType(uobj)<< "</li>";
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Object::DataVariance t)
{
    switch(t)
    {
    case osg::Object::DYNAMIC: os << "DYNAMIC"; break;
    case osg::Object::STATIC: os << "STATIC"; break;
    case osg::Object::UNSPECIFIED: os << "UNSPECIFIED"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osg::Object>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Object * object = getObject<osg::Object,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining node properties
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>classname</td><td>" << object->className() << "</td></tr>" << std::endl;
            os << "<tr><td>libname</td><td>" << object->libraryName() << "</td></tr>" << std::endl;
            os << "<tr><td>dataVariance</td><td>" << object->getDataVariance() << "</td></tr>" << std::endl;
            os << "<tr><td>userData</td><td>" << osg_helpers::getObjectNameAndType(object->getUserData()) << "</td></tr>" << std::endl;
            os << "<tr><td>userDataContainer</td><td>" << osg_helpers::getObjectNameAndType(object->getUserDataContainer(), true) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::Referenced>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Referenced * object = getObject<osg::Referenced,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>this</td><td>" << std::hex << (void*)object << std::dec << "</td></tr>" << std::endl;
            os << "<tr><td>typename</td><td>" << helpers::getRTTITypename_html(object) << "</td></tr>" << std::endl;
            os << "<tr><td>refCount</td><td>" << (object?object->referenceCount():0) << "</td></tr>" << std::endl;
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        // no more forwarding
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<osg::ObserverSet>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ObserverSet * object = getObject<osg::ObserverSet,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            callNextHandler(os);

            // add remaining ObserverSet properties
            os << "<tr><td>observerd</td><td>";
            os << osg_helpers::getObjectNameAndType(object->getObserverdObject());
            os << "</td></tr>" << std::endl;
            os << "<tr><td>observers</td><td><ul>";
            const osg::ObserverSet::Observers& observers = object->getObservers();
            for(osg::ObserverSet::Observers::const_iterator it = observers.begin(); it != observers.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(*it) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeObservers:
        {
            os << "<ul>";
            const osg::ObserverSet::Observers& observers = object->getObservers();
            for(osg::ObserverSet::Observers::const_iterator it = observers.begin(); it != observers.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(*it) << "</li>";
            }
            os << "</ul>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Array::Type t)
{
    switch(t)
    {
    case osg::Array::ArrayType: os << "ArrayType"; break;
    case osg::Array::ByteArrayType: os << "ByteArrayType"; break;
    case osg::Array::ShortArrayType: os << "ShortArrayType"; break;
    case osg::Array::IntArrayType: os << "IntArrayType"; break;
    case osg::Array::UByteArrayType: os << "UByteArrayType"; break;
    case osg::Array::UShortArrayType: os << "UShortArrayType"; break;
    case osg::Array::UIntArrayType: os << "UIntArrayType"; break;
    case osg::Array::Vec4ubArrayType: os << "Vec4ubArrayType"; break;
    case osg::Array::FloatArrayType: os << "FloatArrayType"; break;
    case osg::Array::Vec2ArrayType: os << "Vec2ArrayType"; break;
    case osg::Array::Vec3ArrayType: os << "Vec3ArrayType"; break;
    case osg::Array::Vec4ArrayType: os << "Vec4ArrayType"; break;
    case osg::Array::Vec2sArrayType: os << "Vec2sArrayType"; break;
    case osg::Array::Vec3sArrayType: os << "Vec3sArrayType"; break;
    case osg::Array::Vec4sArrayType: os << "Vec4sArrayType"; break;
    case osg::Array::Vec2bArrayType: os << "Vec2bArrayType"; break;
    case osg::Array::Vec3bArrayType: os << "Vec3bArrayType"; break;
    case osg::Array::Vec4bArrayType: os << "Vec4bArrayType"; break;
    case osg::Array::DoubleArrayType: os << "DoubleArrayType"; break;
    case osg::Array::Vec2dArrayType: os << "Vec2dArrayType"; break;
    case osg::Array::Vec3dArrayType: os << "Vec3dArrayType"; break;
    case osg::Array::Vec4dArrayType: os << "Vec4dArrayType"; break;
    case osg::Array::MatrixArrayType: os << "MatrixArrayType"; break;
    case osg::Array::MatrixdArrayType: os << "MatrixdArrayType"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg::Array::Binding t)
{
    switch(t)
    {
    case osg::Array::BIND_UNDEFINED: os << "BIND_UNDEFINED"; break;
    case osg::Array::BIND_OFF: os << "BIND_OFF"; break;
    case osg::Array::BIND_OVERALL: os << "BIND_OVERALL"; break;
    case osg::Array::BIND_PER_PRIMITIVE_SET: os << "BIND_PER_PRIMITIVE_SET"; break;
    case osg::Array::BIND_PER_VERTEX: os << "BIND_PER_VERTEX"; break;
    default: os << (int)t; break;
    }
    return os;
}

#define writeArrayDataImpl(__elem_type) \
    { \
        os << "<ol start=\"0\">"; \
        const __elem_type * d = (const __elem_type*)object->getDataPointer(); \
        for(unsigned n = 0; n < object->getNumElements(); n++) \
            os << "<li>" << d[n] << "</li>"; \
        os << "</ol>"; \
    }

bool writePrettyHTMLImpl<osg::Array>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::Array * object = getObject<osg::Array,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::BufferData properties first
            callNextHandler(os);

            // remaining Array properties
            os << "<tr><td>type</td><td>" << object->getType() << "</td></tr>" << std::endl;
            os << "<tr><td>dataType</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getDataType()) << "</td></tr>" << std::endl;
            os << "<tr><td>dataSize</td><td>" << object->getDataSize() << "</td></tr>" << std::endl;
            os << "<tr><td>totalDataSize</td><td>" << object->getTotalDataSize() << "</td></tr>" << std::endl;
            os << "<tr><td>numElements</td><td>" << object->getNumElements() << "</td></tr>" << std::endl;
            os << "<tr><td>dataPointer</td><td>" << object->getDataPointer() << "</td></tr>" << std::endl;
            os << "<tr><td>binding</td><td>" << object->getBinding() << "</td></tr>" << std::endl;
            os << "<tr><td>normalize</td><td>" << (object->getNormalize()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>preserveDataType</td><td>" << (object->getPreserveDataType()?"true":"false") << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeArrayData:
        {
            switch(object->getType())
            {
            default:
            case osg::Array::ArrayType:
            case osg::Array::MatrixArrayType:
            case osg::Array::MatrixdArrayType:
                os << "array type " << object->getType() << " not implemented";
                break;
            case osg::Array::ByteArrayType: writeArrayDataImpl(int8_t); break;
            case osg::Array::ShortArrayType: writeArrayDataImpl(int16_t); break;
            case osg::Array::IntArrayType: writeArrayDataImpl(int32_t); break;
            case osg::Array::UByteArrayType:  writeArrayDataImpl(uint8_t); break;
            case osg::Array::UShortArrayType:  writeArrayDataImpl(uint16_t); break;
            case osg::Array::UIntArrayType:  writeArrayDataImpl(uint32_t); break;
            case osg::Array::Vec4ubArrayType:  writeArrayDataImpl(osg::Vec4ub); break;
            case osg::Array::FloatArrayType: writeArrayDataImpl(float); break;
            case osg::Array::Vec2ArrayType: writeArrayDataImpl(osg::Vec2); break;
            case osg::Array::Vec3ArrayType: writeArrayDataImpl(osg::Vec3); break;
            case osg::Array::Vec4ArrayType: writeArrayDataImpl(osg::Vec4); break;
            case osg::Array::Vec2sArrayType: writeArrayDataImpl(osg::Vec2s); break;
            case osg::Array::Vec3sArrayType: writeArrayDataImpl(osg::Vec3s); break;
            case osg::Array::Vec4sArrayType: writeArrayDataImpl(osg::Vec4s); break;
            case osg::Array::Vec2bArrayType: writeArrayDataImpl(osg::Vec2b); break;
            case osg::Array::Vec3bArrayType: writeArrayDataImpl(osg::Vec3b); break;
            case osg::Array::Vec4bArrayType: writeArrayDataImpl(osg::Vec4b); break;
            case osg::Array::DoubleArrayType: writeArrayDataImpl(double); break;
            case osg::Array::Vec2dArrayType: writeArrayDataImpl(osg::Vec2d); break;
            case osg::Array::Vec3dArrayType: writeArrayDataImpl(osg::Vec3d); break;
            case osg::Array::Vec4dArrayType: writeArrayDataImpl(osg::Vec4d); break;
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::IndexArray>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::IndexArray * object = getObject<osg::IndexArray,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::Array properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::BufferObject>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::BufferObject * object = getObject<osg::BufferObject,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::Array properties first
            callNextHandler(os);

            os << "<tr><td>target</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getTarget()) << "</td></tr>" << std::endl;
            os << "<tr><td>usage</td><td>" << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(object->getUsage()) << "</td></tr>" << std::endl;
            os << "<tr><td>size</td><td>" << object->getProfile()._size << "</td></tr>" << std::endl;
            os << "<tr><td>numBufferData</td><td>" << object->getNumBufferData() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeBufferDatas:
        {
            os << "<ol>";
            unsigned numBufferData = object->getNumBufferData();
            for(unsigned n = 0; n < numBufferData; n++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(object->getBufferData(n), true) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::VertexBufferObject>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::VertexBufferObject * object = getObject<osg::VertexBufferObject,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::Array properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ShaderComposer>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ShaderComposer * object = getObject<osg::ShaderComposer,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::ShaderComposer properties first
            callNextHandler(os);
            ShaderComposerAccess * access = (ShaderComposerAccess *)object;
            os << "<tr><td>programMap size</td><td>" << access->programMap().size() << "</td></tr>" << std::endl;
            os << "<tr><td>shaderMainMap size</td><td>" << access->shaderMainMap().size() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::ShaderComponent>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::ShaderComponent * object = getObject<osg::ShaderComponent,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::ShaderComponent properties first
            callNextHandler(os);
            os << "<tr><td>numShaders</td><td>" << object->getNumShaders() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeShaders:
        {
            os << "<ol>";
            unsigned numShaders = object->getNumShaders();
            for(unsigned n = 0; n < numShaders; n++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(object->getShader(n), true) << "</li>" << std::endl;
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg::VertexArrayState>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg::VertexArrayState * object = getObject<osg::VertexArrayState,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::VertexArrayState properties first
            callNextHandler(os);
            os << "<tr><td>EBO</td><td>" << osg_helpers::getObjectNameAndType(object->getCurrentElementBufferObject()) << "</td></tr>" << std::endl;
            os << "<tr><td>VBO</td><td>" << osg_helpers::getObjectNameAndType(object->getCurrentVertexBufferObject()) << "</td></tr>" << std::endl;
            os << "<tr><td>VAO</td><td>" << object->getVertexArrayObject() << "</td></tr>" << std::endl;
            os << "<tr><td>requiresSetArrays</td><td>" << (object->getRequiresSetArrays() ? "true" : "false") << "</td></tr>" << std::endl;
            os << "<tr><td>vertexArray</td><td>" << osg_helpers::getObjectNameAndType(object->_vertexArray) << "</td></tr>" << std::endl;
            os << "<tr><td>normalArray</td><td>" << osg_helpers::getObjectNameAndType(object->_normalArray) << "</td></tr>" << std::endl;
            os << "<tr><td>colorArray</td><td>" << osg_helpers::getObjectNameAndType(object->_colorArray) << "</td></tr>" << std::endl;
            os << "<tr><td>secondaryColorArray</td><td>" << osg_helpers::getObjectNameAndType(object->_secondaryColorArray) << "</td></tr>" << std::endl;
            os << "<tr><td>fogCoordArray</td><td>" << osg_helpers::getObjectNameAndType(object->_fogCoordArray) << "</td></tr>" << std::endl;
            os << "<tr><td>texCoordArrays</td><td><ol>";
            for (auto t : object->_texCoordArrays)
                os << "<li>" << osg_helpers::getObjectNameAndType(t) << "</li>";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>vertexAttribArrays</td><td><ol>";
            for (auto t : object->_vertexAttribArrays)
                os << "<li>" << osg_helpers::getObjectNameAndType(t) << "</li>";
            os << "</td></tr>" << std::endl; 
            os << "<tr><td>activeDispatchers</td><td><ol>";
            for (auto t : object->_activeDispatchers)
                os << "<li>" << osg_helpers::getObjectNameAndType(t) << "</li>";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>previous_activeDispatchers</td><td><ol>";
            for (auto t : object->_previous_activeDispatchers)
                os << "<li>" << osg_helpers::getObjectNameAndType(t) << "</li>";
            os << "</td></tr>" << std::endl; 

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, osg_helpers::RenderInfoData::VertexArrayStateSnapshot::ArraySnapshot & s)
{
    return os << osg_helpers::getObjectNameAndType(s.array.get()) << " active=" << (s.active ? "true" : "false") << " modified=" << s.modifiedCount;
}

bool writePrettyHTMLImpl<osg_helpers::RenderInfoData::VertexArrayStateSnapshot>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osg_helpers::RenderInfoData::VertexArrayStateSnapshot * object = getObject<osg_helpers::RenderInfoData::VertexArrayStateSnapshot, SGIItemOsg>();
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add osg::VertexArrayState properties first
            callNextHandler(os);

            os << "<tr><td>vertexArraySnapshot</td><td>" << object->_vertexArraySnapshot << "</td></tr>" << std::endl;
            os << "<tr><td>normalArraySnapshot</td><td>" << object->_normalArraySnapshot << "</td></tr>" << std::endl;
            os << "<tr><td>colorArraySnapshot</td><td>" << object->_colorArraySnapshot << "</td></tr>" << std::endl;
            os << "<tr><td>secondaryColorArraySnapshot</td><td>" << object->_secondaryColorArraySnapshot << "</td></tr>" << std::endl;
            os << "<tr><td>fogCoordArraySnapshot</td><td>" << object->_fogCoordArraySnapshot << "</td></tr>" << std::endl;
            os << "<tr><td>texCoordArraySnapshots</td><td><ol>";
            for (auto t : object->_texCoordArrayShapshots)
                os << "<li>" << t << "</li>";
            os << "</td></tr>" << std::endl;
            os << "<tr><td>vertexAttribArrays</td><td><ol>";
            for (auto t : object->_vertexAttribArraySnapshots)
                os << "<li>" << t << "</li>";
            os << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl_RenderInfoData(SGIPluginHostInterface * hostInterface, std::basic_ostream<char>& os, bool table, SGIItemBase * item, const RenderInfoData & data)
{
    bool ret = false;
    SGIItemBase * itemNext = item->nextBase();
    switch(item->type())
    {
    case SGIItemTypeObject:
        {
            if(table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add referenced properties first
            ret = hostInterface->writePrettyHTML(os, itemNext, false);

            if(table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeRenderInfoState:
        {
            if (item->number() == ~0u)
            {
                const RenderInfoData::HashedState & hashedState = data.hashedState();
                if(hashedState.empty())
                    os << "No states available.." << std::endl;
                else
                {
                    os << "<ul>";
                    for (const auto & it : hashedState)
                    {
                        const RenderInfoData::State & state = it.second;
                        os << "<li>0x" << std::hex << it.first << "<br/>";
                        os << "contextID=#" << (state.state.valid()?state.state->getContextID():~0u) << "<br/>" << std::endl;
                        os << "stateSetStack=#" << state.stateSetStack.size() << "<br/>" << std::endl;
                        os << "renderBinStack=#" << state.renderBinStack.size() << "<br/>" << std::endl;
                        os << "cameraStack=#" << state.cameraStack.size() << "<br/>" << std::endl;
                        os << "appliedProgamSet=#" << state.appliedProgamSet.size() << "<br/>" << std::endl;
                        os << "</li>" << std::endl;
                    }
                    os << "</ul>";
                }
            }
            else
            {
                const RenderInfoData::HashedState & hashedState = data.hashedState();
                RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
                if (it != hashedState.end())
                {
                    const RenderInfoData::State & state = it->second;
                    os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
                    os << "<tr><td>hash</td><td>0x" << std::hex << it->first << "</td></tr>" << std::endl;
                    os << "<tr><td>state</td><td>" << osg_helpers::getObjectNameAndType(state.state.get(), true) << "</td></tr>" << std::endl;
                    os << "<tr><td>contextID</td><td>" << (state.state.valid() ? state.state->getContextID() : ~0u) << "</td></tr>" << std::endl;
                    os << "<tr><td>stateSetStack</td><td>" << state.stateSetStack.size() << " items</td></tr>" << std::endl;
                    os << "<tr><td>renderBinStack</td><td>" << state.renderBinStack.size() << " items</td></tr>" << std::endl;
                    os << "<tr><td>cameraStack</td><td>" << state.cameraStack.size() << " items</td></tr>" << std::endl;
                    os << "<tr><td>appliedProgamSet</td><td>" << state.appliedProgamSet.size() << " items</td></tr>" << std::endl;
                    os << "<tr><td>view</td><td>" << osg_helpers::getObjectNameAndType(state.view.get(), true) << "</td></tr>" << std::endl;
                    os << "<tr><td>userData</td><td>" << osg_helpers::getObjectNameAndType(state.userData.get(), true) << "</td></tr>" << std::endl;
                    os << "<tr><td>capturedStateSet</td><td>" << osg_helpers::getObjectNameAndType(state.capturedStateSet.get(), true) << "</td></tr>" << std::endl;
                    os << "<tr><td>combinedStateSet</td><td>" << osg_helpers::getObjectNameAndType(state.combinedStateSet.get(), true) << "</td></tr>" << std::endl;
                    os << "</table>" << std::endl;
                }
                else
                {
                    os << "<b>state <i>" << item->number() << "</i> not found<b>" << std::endl;
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeRenderInfoStateSetStack:
        {
            if (item->number() == ~0u)
            {
                const RenderInfoData::HashedState & hashedState = data.hashedState();
                os << "<ul>";
                for (const auto & state : hashedState)
                {
                    os << "<li>" << state.first << "</li>" << std::endl;
                }
                os << "</ul>";
            }
            else
            {
                const RenderInfoData::HashedState & hashedState = data.hashedState();
                RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
                if (it != hashedState.end())
                {
                    const RenderInfoData::State & state = it->second;
                    const RenderInfoData::StateSetStack & set = state.stateSetStack;
                    if(set.empty())
                        os << "StateSet stack of state <i>" << item->number() << "</i> is empty." << std::endl;
                    else
                    {
                        os << "<ol>";
                        for (RenderInfoData::StateSetStack::const_iterator it = set.begin(); it != set.end(); it++)
                        {
                            const RenderInfoData::StateSetEntry & entry = *it;
                            os << "<li>" << osg_helpers::getObjectNameAndType(entry.stateSet.get(), true) << "<br/>";
                            for (const auto & paths : entry.parentalNodePaths)
                            {
                                os << "<ul>";
                                for (const auto & path : paths)
                                {
                                    os << "<li>";
                                    osg_helpers::writePrettyHTML(os, path);
                                    os << "</li>";
                                }
                                os << "</ul>";
                            }
                            os << "</li>" << std::endl;
                        }
                        os << "</ol>";
                    }
                }
                else
                {
                    os << "<b>state <i>" << item->number() << "</i> not found<b>" << std::endl;
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeRenderInfoRenderBinStack:
        {
            const RenderInfoData::HashedState & hashedState = data.hashedState();
            RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
            if(it != hashedState.end())
            {
                const RenderInfoData::State & state = it->second;
                const RenderInfoData::RenderBinStack & set = state.renderBinStack;
                if (set.empty())
                    os << "render bin stack of state <i>" << item->number() << "</i> is empty." << std::endl;
                else
                {
                    os << "<ol>";
                    for (RenderInfoData::RenderBinStack::const_iterator it = set.begin(); it != set.end(); it++)
                    {
                        os << "<li>" << osg_helpers::getObjectNameAndType((*it).get(), true) << "</li>" << std::endl;
                    }
                    os << "</ol>";
                }
            }
            else
            {
                os << "<b>state <i>" << item->number() << "</i> not found<b>" << std::endl;
            }
            ret = true;
        }
        break;
    case SGIItemTypeRenderInfoCameraStack:
        {
            const RenderInfoData::HashedState & hashedState = data.hashedState();
            RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
            if(it != hashedState.end())
            {
                const RenderInfoData::State & state = it->second;
                const RenderInfoData::CameraStack & set = state.cameraStack;
                if (set.empty())
                    os << "camara stack of state <i>" << item->number() << "</i> is empty." << std::endl;
                else
                {
                    os << "<ol>";
                    for (RenderInfoData::CameraStack::const_iterator it = set.begin(); it != set.end(); it++)
                    {
                        os << "<li>" << osg_helpers::getObjectNameAndType((*it).get(), true) << "</li>" << std::endl;
                    }
                    os << "</ol>";
                }
            }
            else
            {
                os << "<b>state <i>" << item->number() << "</i> not found<b>" << std::endl;
            }
            ret = true;
        }
        break;
    case SGIItemTypeRenderInfoAppliedProgramSet:
        {
            const RenderInfoData::HashedState & hashedState = data.hashedState();
            RenderInfoData::HashedState::const_iterator it = hashedState.find(item->number());
            if(it != hashedState.end())
            {
                os << "<ol>";
                const RenderInfoData::State & state = it->second;
                const RenderInfoData::PerContextProgramSet & set = state.appliedProgamSet;
                for(RenderInfoData::PerContextProgramSet::const_iterator it = set.begin(); it != set.end(); it++)
                {
                    os << "<li>" << osg_helpers::getObjectNameAndType((*it).get()) << "</li>" << std::endl;
                }
                os << "</ol>";
            }
            else
            {
                os << "<b>state <i>" << item->number() << "</i> not found<b>" << std::endl;
            }
            ret = true;
        }
        break;
    default:
        ret = hostInterface->writePrettyHTML(os, itemNext, table);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osg_helpers::RenderInfoDrawCallback>::process(std::basic_ostream<char>& os)
{
    osg_helpers::RenderInfoDrawCallback * object = getObject<osg_helpers::RenderInfoDrawCallback, SGIItemOsg, DynamicCaster>();
    const osg_helpers::RenderInfoData & data = object->data();
    bool ret = writePrettyHTMLImpl_RenderInfoData(_hostInterface, os, _table, _item, data);
    return ret;
}

bool writePrettyHTMLImpl<osg_helpers::RenderInfoDrawable>::process(std::basic_ostream<char>& os)
{
    osg_helpers::RenderInfoDrawable * object = getObject<osg_helpers::RenderInfoDrawable, SGIItemOsg>();
    const osg_helpers::RenderInfoData & data = object->data();
    bool ret = writePrettyHTMLImpl_RenderInfoData(_hostInterface, os, _table, _item, data);
    return ret;
}

#if OSG_VERSION_LESS_THAN(3,5,0)
bool writePrettyHTMLImpl<osg_helpers::RenderInfoGeometry>::process(std::basic_ostream<char>& os)
{
    osg_helpers::RenderInfoGeometry * object = getObject<osg_helpers::RenderInfoGeometry, SGIItemOsg>();
    const osg_helpers::RenderInfoData & data = object->data();
    bool ret = writePrettyHTMLImpl_RenderInfoData(_hostInterface, os, _table, _item, data);
    return ret;
}
#endif

#define writePrettyHTML_ValueObject(__c) \
bool writePrettyHTMLImpl<__c>::process(std::basic_ostream<char>& os) \
{ \
    bool ret = false; \
    __c * object = getObject<__c, SGIItemOsg>(); \
    switch (itemType()) \
    { \
    case SGIItemTypeObject: \
        {\
            if (_table)\
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;\
            callNextHandler(os); \
            os << "<tr><td>value</td><td>" << object->getValue() << "</td></tr>" << std::endl; \
            if (_table) \
                os << "</table>" << std::endl; \
            ret = true; \
        } \
        break; \
    default: \
        ret = callNextHandler(os); \
        break; \
    } \
    return ret; \
}

writePrettyHTML_ValueObject(osg::StringValueObject)
writePrettyHTML_ValueObject(osg::BoolValueObject)
writePrettyHTML_ValueObject(osg::CharValueObject)
writePrettyHTML_ValueObject(osg::UCharValueObject)
writePrettyHTML_ValueObject(osg::ShortValueObject)
writePrettyHTML_ValueObject(osg::UShortValueObject)
writePrettyHTML_ValueObject(osg::IntValueObject)
writePrettyHTML_ValueObject(osg::UIntValueObject)
writePrettyHTML_ValueObject(osg::FloatValueObject)
writePrettyHTML_ValueObject(osg::DoubleValueObject)
writePrettyHTML_ValueObject(osg::Vec2fValueObject)
writePrettyHTML_ValueObject(osg::Vec3fValueObject)
writePrettyHTML_ValueObject(osg::Vec4fValueObject)
writePrettyHTML_ValueObject(osg::Vec2dValueObject)
writePrettyHTML_ValueObject(osg::Vec3dValueObject)
writePrettyHTML_ValueObject(osg::Vec4dValueObject)
writePrettyHTML_ValueObject(osg::QuatValueObject)
writePrettyHTML_ValueObject(osg::PlaneValueObject)
writePrettyHTML_ValueObject(osg::MatrixfValueObject)
writePrettyHTML_ValueObject(osg::MatrixdValueObject)
writePrettyHTML_ValueObject(osg::BoundingBoxfValueObject)
writePrettyHTML_ValueObject(osg::BoundingBoxdValueObject)
writePrettyHTML_ValueObject(osg::BoundingSpherefValueObject)
writePrettyHTML_ValueObject(osg::BoundingSpheredValueObject)

bool writePrettyHTMLImpl<sgi::ReferencedPickerBase>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    sgi::ReferencedPickerBase * object = getObject<ReferencedPickerBase, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>result</td><td>" << (object->result()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>position</td><td>x=" << object->position().x() << " y=" << object->position().y() << "</td></tr>" << std::endl;
            os << "<tr><td>traversalMask</td><td>" << castToEnumValueString<osgNodeMask>(object->traversalMask()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

namespace {
    template<typename INTERSECTOR_HIT_TYPE>
    void writeIntersectorHit(std::basic_ostream<char>& os, const INTERSECTOR_HIT_TYPE & hit, bool table, bool brief=true)
    {
        if(table)
            os << "<table border=\'1\' align=\'left\'>" << std::endl;

        os << "<tr><td>ratio</td><td>" << hit.ratio << "</td></tr>" << std::endl;
        os << "<tr><td>nodePath</td><td>";
        if(brief)
        {
            os << hit.nodePath.size() << " elements";
            if(!hit.nodePath.empty())
            {
                osg::Node * back = hit.nodePath.back();
                os << " back=" << osg_helpers::getObjectName(back) << " (" << osg_helpers::getObjectTypename(back) << ")";
            }
        }
        else
        {
            osg_helpers::writePrettyHTML(os, hit.nodePath);
        }
        os << "</td></tr>" << std::endl;

        os << "<tr><td>drawable</td><td>";
        if(hit.drawable.valid())
            os << osg_helpers::getObjectName(hit.drawable.get()) << " (" << osg_helpers::getObjectTypename(hit.drawable.get()) << ")";
        os << "</td></tr>" << std::endl;
        os << "<tr><td>matrix</td><td>";
        if(hit.matrix.valid())
            writePrettyHTML(os, *hit.matrix.get(), MatrixUsageTypeGeneric, hit.drawable.get());
        else
            os << "&lt;none&gt;";
        os << "</td></tr>" << std::endl;
        os << "<tr><td>localIntersectionPoint</td><td>" << hit.getLocalIntersectPoint() << "</td></tr>" << std::endl;
        os << "<tr><td>worldIntersectionPoint</td><td>";
        writePrettyPositionHTML(os, hit.getWorldIntersectPoint(), hit.drawable.get());
        os << "</td></tr>" << std::endl;
        os << "<tr><td>localIntersectionNormal</td><td>" << hit.getLocalIntersectNormal() << "</td></tr>" << std::endl;
        os << "<tr><td>worldIntersectionNormal</td><td>" << hit.getWorldIntersectNormal() << "</td></tr>" << std::endl;

        os << "<tr><td>indexList</td><td><ol>";
        for(osgUtil::LineSegmentIntersector::Intersection::IndexList::const_iterator it = hit.indexList.begin(); it != hit.indexList.end(); it++)
        {
            os << "<li>" << *it << "</li>";
        }
        os << "</ol></td></tr>" << std::endl;
        os << "<tr><td>ratioList</td><td><ol>";
        for(osgUtil::LineSegmentIntersector::Intersection::RatioList::const_iterator it = hit.ratioList.begin(); it != hit.ratioList.end(); it++)
        {
            os << "<li>" << *it << "</li>";
        }
        os << "</ol></td></tr>" << std::endl;
        os << "<tr><td>primitiveIndex</td><td>" << hit.primitiveIndex << "</td></tr>" << std::endl;

        if(table)
            os << "</table>" << std::endl;
    }
}

#ifdef SGI_USE_OSGEARTH

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
	typedef osgEarth::Picker PickerType;
#else
	typedef osgEarth::IntersectionPicker PickerType;
#endif

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const PickerType::Limit & t)
{
    switch(t)
    {
    case PickerType::NO_LIMIT: os << "NO_LIMIT"; break;
    case PickerType::LIMIT_ONE_PER_DRAWABLE: os << "LIMIT_ONE_PER_DRAWABLE"; break;
    case PickerType::LIMIT_ONE: os << "LIMIT_ONE"; break;
    case PickerType::LIMIT_NEAREST: os << "LIMIT_NEAREST"; break;
    default: os << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<sgi::ReferencedPicker>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    sgi::ReferencedPicker * object = getObject<ReferencedPicker, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>thickness</td><td>" << object->thickness() << "</td></tr>" << std::endl;
            os << "<tr><td>limit</td><td>" << object->limit() << "</td></tr>" << std::endl;
            os << "<tr><td>hits</td><td>";
            const ReferencedPicker::Hits & hits = object->hits();
            os << hits.size();
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePickerHits:
        {
            const ReferencedPicker::Hits & hits = object->hits();
            os << "<ol>" << std::endl;
            for(ReferencedPicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++)
            {
                os << "<li>";
                writeIntersectorHit(os, *it, true, true);
                os << "</li>";
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePickerHit:
        {
            unsigned number = _item->number();
            const ReferencedPicker::Hits & hits = object->hits();

            unsigned current_num = 0;
            for(ReferencedPicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, current_num++)
            {
                if(current_num == number)
                {
                    writeIntersectorHit(os, *it, true, false);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}
#endif // SGI_USE_OSGEARTH

bool writePrettyHTMLImpl<sgi::ReferencedLinePicker>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    sgi::ReferencedLinePicker * object = getObject<ReferencedLinePicker, SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            os << "<tr><td>hits</td><td>";
            const ReferencedLinePicker::Hits & hits = object->hits();
            os << hits.size();
            os << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePickerHits:
        {
            const ReferencedLinePicker::Hits & hits = object->hits();
            os << "<ol>" << std::endl;
            for(ReferencedLinePicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++)
            {
                os << "<li>";
                writeIntersectorHit(os, *it, true, true);
                os << "</li>";
            }
            os << "</ol>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypePickerHit:
        {
            unsigned number = _item->number();
            const ReferencedLinePicker::Hits & hits = object->hits();

            unsigned current_num = 0;
            for(ReferencedLinePicker::Hits::const_iterator it = hits.begin(); it != hits.end(); it++, current_num++)
            {
                if(current_num == number)
                {
                    writeIntersectorHit(os, *it, true, false);
                }
            }
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi
