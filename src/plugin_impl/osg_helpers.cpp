// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <osg/StateAttribute>
#include <osg/Texture>
#include <osg/CoordinateSystemNode>
#include <osg/ObserverNodePath>
#include <osg/io_utils>

#include <QImage>
#include <QGLWidget>

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/osg>

namespace sgi {
    namespace osg_helpers {

std::string getObjectTypename(const osg::Referenced * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

std::string getObjectTypename(const osg::Observer * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

std::string getObjectTypename(const osg::Object * object)
{
    return object?(std::string(object->libraryName()) + std::string("::") + std::string(object->className())):"(null)";
}

std::string getObjectName(const osg::Referenced * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << helpers::getRTTITypename_html(object);
        buf << '(';
        buf << (void*)object;
        buf << ')';
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectName(const osg::Observer * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << helpers::getRTTITypename_html(object);
        buf << '(';
        buf << (void*)object;
        buf << ')';
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectName(const osg::Object * object)
{
    std::string ret = object?(object->getName()):"(null)";
    if(ret.empty())
    {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    return ret;
}

std::string getObjectNameAndType(const osg::Referenced * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << (void*)object;
        buf << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectNameAndType(const osg::Observer * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << (void*)object;
        buf << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectNameAndType(const osg::Object * object)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object) << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}


std::string glValueName(unsigned n)
{
    std::string ret;
    if(n & osg::StateAttribute::ON)
        ret = "ON";
    else
        ret = "OFF";
    if(n & osg::StateAttribute::OVERRIDE)
        sgi::helpers::str_append(ret, "OVERRIDE");
    if(n & osg::StateAttribute::PROTECTED)
        sgi::helpers::str_append(ret, "PROTECTED");
    if(n & osg::StateAttribute::INHERIT)
        sgi::helpers::str_append(ret, "INHERIT");
    else
        sgi::helpers::str_append(ret, "NOINHERIT");
    std::stringstream ss;
    ss << ret << "(0x" << std::hex << n << ')';
    ret = ss.str();
    return ret;
}

std::string glOverrideValueName(unsigned n)
{
    std::string ret;
    if(n == 0)
        ret = "NOOVERRIDE";
    else
    {
        if(n & osg::StateAttribute::ON)
            ret = "Override disabled";
        else
        {
            if(n & osg::StateAttribute::OVERRIDE)
                sgi::helpers::str_append(ret, "OVERRIDE");
            if(n & osg::StateAttribute::INHERIT)
                sgi::helpers::str_append(ret, "INHERIT");
            if(n & osg::StateAttribute::PROTECTED)
                sgi::helpers::str_append(ret, "PROTECTED");
            if(ret.empty())
            {
                std::stringstream ss;
                ss << "0x" << std::hex << n;
                ret = ss.str();
            }
        }
    }
    return ret;
}

std::string clearMaskToString(unsigned clearMask)
{
    std::string ret;
    if(clearMask == ~0u)
        ret = "all";
    else if(clearMask == 0u)
        ret = "off";
    else
    {
        if(clearMask & GL_CURRENT_BIT)
            sgi::helpers::str_append(ret, "current");
        if(clearMask & GL_POINT_BIT)
            sgi::helpers::str_append(ret, "point");
        if(clearMask & GL_LINE_BIT)
            sgi::helpers::str_append(ret, "line");
        if(clearMask & GL_POLYGON_BIT)
            sgi::helpers::str_append(ret, "polygon");
        if(clearMask & GL_POLYGON_STIPPLE_BIT)
            sgi::helpers::str_append(ret, "polygonStipple");
        if(clearMask & GL_PIXEL_MODE_BIT)
            sgi::helpers::str_append(ret, "pixelMode");
        if(clearMask & GL_LIGHTING_BIT)
            sgi::helpers::str_append(ret, "lighting");
        if(clearMask & GL_FOG_BIT)
            sgi::helpers::str_append(ret, "fog");
        if(clearMask & GL_DEPTH_BUFFER_BIT)
            sgi::helpers::str_append(ret, "depthBuffer");
        if(clearMask & GL_ACCUM_BUFFER_BIT)
            sgi::helpers::str_append(ret, "accumBuffer");
        if(clearMask & GL_STENCIL_BUFFER_BIT)
            sgi::helpers::str_append(ret, "stencilBuffer");
        if(clearMask & GL_VIEWPORT_BIT)
            sgi::helpers::str_append(ret, "viewport");
        if(clearMask & GL_TRANSFORM_BIT)
            sgi::helpers::str_append(ret, "transform");
        if(clearMask & GL_ENABLE_BIT)
            sgi::helpers::str_append(ret, "enable");
        if(clearMask & GL_COLOR_BUFFER_BIT)
            sgi::helpers::str_append(ret, "colorBuffer");
        if(clearMask & GL_HINT_BIT)
            sgi::helpers::str_append(ret, "hint");
        if(clearMask & GL_EVAL_BIT)
            sgi::helpers::str_append(ret, "eval");
        if(clearMask & GL_LIST_BIT)
            sgi::helpers::str_append(ret, "list");
        if(clearMask & GL_TEXTURE_BIT)
            sgi::helpers::str_append(ret, "texture");
        if(clearMask & GL_SCISSOR_BIT)
            sgi::helpers::str_append(ret, "scissor");
    }
    return ret;
}


const osg::EllipsoidModel & getDefaultEllipsoid()
{
    static osg::EllipsoidModel s_default;
    return s_default;
}

osg::Vec3d fromECEFToXYZ(const osg::EllipsoidModel & ellipsoid, const osg::Vec3d & coord)
{
    osg::Vec3d ret;
    ellipsoid.convertLatLongHeightToXYZ(osg::DegreesToRadians(coord.x()), osg::DegreesToRadians(coord.y()), coord.z(), ret.x(), ret.y(), ret.z());
    return ret;
}

osg::Vec3d fromXYZToECEF(const osg::EllipsoidModel & ellipsoid, const osg::Vec3d & v)
{
    osg::Vec3d ret;
    ellipsoid.convertXYZToLatLongHeight(v.x(), v.y(), v.z(), ret.x(), ret.y(), ret.z());
    ret.x() = osg::RadiansToDegrees(ret.x());
    ret.y() = osg::RadiansToDegrees(ret.y());
    return ret;
}

osg::Vec3d fromECEFToXYZ(const osg::Vec3d & coord)
{
    return fromECEFToXYZ(getDefaultEllipsoid(), coord);
}

osg::Vec3d fromXYZToECEF(const osg::Vec3d & v)
{
    return fromXYZToECEF(getDefaultEllipsoid(), v);
}

void getHPRFromQuat(const osg::Quat& q, double &h, double &p, double &r)
{
    osg::Matrixd rot(q);
    p = asin(rot(1,2));
    if( osg::equivalent(osg::absolute(p), osg::PI_2) )
    {
        r = 0.0;
        h = atan2( rot(0,1), rot(0,0) );
    }
    else
    {
        r = atan2( rot(0,2), rot(2,2) );
        h = atan2( rot(1,0), rot(1,1) );
    }
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec2f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec3f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec4f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec2d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec3d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec4d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")" << std::endl;
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Quat & q)
{
    double h, p, r;
    getHPRFromQuat(q, h, p, r);
    os << "h=" << osg::RadiansToDegrees(h) << ";p=" << osg::RadiansToDegrees(p) << ";r=" << osg::RadiansToDegrees(r) << " raw=";
    osg::Vec4d v = q.asVec4();
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::NodePath & path)
{
    if(path.empty())
        os << "&lt;empty&gt;";
    else
    {
        os << "<ol>";
        for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::Node * node = *it;
            os << "<li>" << osg_helpers::getObjectNameAndType(node) << "</li>" << std::endl;
        }
        os << "</ol>";
    }
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::ObserverNodePath & path)
{
    osg::NodePath nodepath;
    if(path.getNodePath(nodepath))
        writePrettyHTML(os, nodepath);
    else
        os << "&lt;none&gt;";
}

void writePrettyPositionHTML(std::basic_ostream<char>& os, const osg::Vec3 & v, const osg::Node * refNode)
{
    osg::Vec3d ecef = fromXYZToECEF(v);
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")@("
        << ecef[0] << ", " << ecef[1] << ", " << ecef[2] << ")" << std::endl;
}

void writePrettyPositionHTML(std::basic_ostream<char>& os, const osg::Vec3 & v, const osg::Drawable * refDrawable)
{
    osg::Vec3d ecef = fromXYZToECEF(v);
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")@("
        << ecef[0] << ", " << ecef[1] << ", " << ecef[2] << ")" << std::endl;
    /*
    const osg::Node * refNode = (refDrawable && refDrawable->getNumParents() > 0)?refDrawable->getParent(0):NULL;
    osg::Vec3d absv;
    if(refNode)
    {
        osg::Matrix m = GetWorldCoordinatesNodeVisitor::calc(const_cast<osg::Node*>(refNode));
        absv = v * m;
    }
    else
        absv = v;
    GpsCoordinate c = fromXYZToGpsCoordinate(absv);
    os << "@(" << c.latitude() << ", " << c.longitude() << ", " << c.elevation() << ")" << std::endl;
    */
}

void writePrettyRotationHTML(std::basic_ostream<char>& os, const osg::Quat & q)
{
    writePrettyHTML(os, q);
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingSphere & bs, const osg::Node * refNode)
{
    const osg::BoundingSphere::vec_type & center = bs.center();
    const osg::BoundingSphere::value_type & radius = bs.radius();

    os << "center=";
    writePrettyPositionHTML(os, center, refNode);
    os << "<br/>radius=" << radius << (bs.valid()?" valid":" invalid");
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingSphere * bs, const osg::Node * refNode)
{
    if(bs)
        writePrettyHTML(os, *bs, refNode);
    else
        os << "&lt;null&gt;";
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox & bb, const osg::Node * refNode)
{
    os << "center=";
    writePrettyPositionHTML(os, bb.center(), refNode);
    os << "<br/>min=";
    writePrettyPositionHTML(os, bb._min, refNode);
    os << "<br/>max=";
    writePrettyPositionHTML(os, bb._max, refNode);
    os << "<br/>radius=" << bb.radius() << (bb.valid()?" valid":" invalid");
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox & bb, const osg::Drawable * refDrawable)
{
    os << "center=";
    writePrettyPositionHTML(os, bb.center(), refDrawable);
    os << "<br/>min=";
    writePrettyPositionHTML(os, bb._min, refDrawable);
    os << "<br/>max=";
    writePrettyPositionHTML(os, bb._max, refDrawable);
    os << "<br/>radius=" << bb.radius() << (bb.valid()?" valid":" invalid");
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox * bb, const osg::Drawable * refDrawable)
{
    if(bb)
        writePrettyHTML(os, *bb, refDrawable);
    else
        os << "&lt;null&gt;";
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Matrixd & mat, MatrixUsageType type, const osg::Node * refNode)
{
    switch(type)
    {
    default:
    case MatrixUsageTypeGeneric:
        os << std::setprecision(12) << "<table border=\'1\' align=\'left\'>" << std::endl;
        os << "<tr><td>" << mat(0,0) << "</td><td>" << mat(0,1) << "</td><td>" << mat(0,2) << "</td><td>" << mat(0,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(1,0) << "</td><td>" << mat(1,1) << "</td><td>" << mat(1,2) << "</td><td>" << mat(1,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(2,0) << "</td><td>" << mat(2,1) << "</td><td>" << mat(2,2) << "</td><td>" << mat(2,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(3,0) << "</td><td>" << mat(3,1) << "</td><td>" << mat(3,2) << "</td><td>" << mat(3,3) << "</td></tr>" << std::endl;
        os << "</table>" << std::endl;
        break;
    case MatrixUsageTypeModelView:
        os << "<ul>";
        os << "<li>translate ";
        writePrettyPositionHTML(os, mat.getTrans(), refNode);
        os << "</li>" << std::endl;
        os << "<li>rotate " << mat.getRotate() << "</li>" << std::endl;
        os << "<li>scale " << mat.getScale() << "</li>" << std::endl;
        os << "</ul>";
        break;
    case MatrixUsageTypePerspective:
        {
            double fovy;
            double aspectRatio;
            double zNear, zFar;
            mat.getPerspective(fovy, aspectRatio, zNear, zFar);

            os << std::setprecision(12) << "<ul>";
            os << "<li>fovy " << fovy << "</li>" << std::endl;
            os << "<li>aspectRatio " << aspectRatio << "</li>" << std::endl;
            os << "<li>near " << zNear << "</li>" << std::endl;
            os << "<li>far " << zFar << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    case MatrixUsageTypeLookAt:
        {
            osg::Vec3d eye;
            osg::Vec3d center;
            osg::Vec3d up;
            mat.getLookAt(eye, center, up);

            os << "<ul>";
            os << "<li>eye ";
            writePrettyPositionHTML(os, eye, refNode);
            os << "</li>" << std::endl;
            os << "<li>center ";
            writePrettyPositionHTML(os, center, refNode);
            os << "</li>" << std::endl;
            os << "<li>up " << up << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    case MatrixUsageTypeFrustum:
        {
            double left, right, bottom, top;
            double zNear, zFar;
            mat.getFrustum(left, right, bottom, top, zNear, zFar);
            os << std::setprecision(12) << "<ul>";
            os << "<li>left " << left << "</li>" << std::endl;
            os << "<li>right " << right << "</li>" << std::endl;
            os << "<li>top " << top << "</li>" << std::endl;
            os << "<li>bottom " << bottom << "</li>" << std::endl;
            os << "<li>near " << zNear << "</li>" << std::endl;
            os << "<li>far " << zFar << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    case MatrixUsageTypeOrtho:
        {
            double left, right, bottom, top;
            double zNear, zFar;
            mat.getOrtho(left, right, bottom, top, zNear, zFar);
            os << std::setprecision(12) << "<ul>";
            os << "<li>left " << left << "</li>" << std::endl;
            os << "<li>right " << right << "</li>" << std::endl;
            os << "<li>top " << top << "</li>" << std::endl;
            os << "<li>bottom " << bottom << "</li>" << std::endl;
            os << "<li>near " << zNear << "</li>" << std::endl;
            os << "<li>far " << zFar << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    }
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Matrixd & mat, MatrixUsageType type, const osg::Drawable * refDrawable)
{
    switch(type)
    {
    default:
    case MatrixUsageTypeGeneric:
        os << std::setprecision(12) << "<table border=\'1\' align=\'left\'>" << std::endl;
        os << "<tr><td>" << mat(0,0) << "</td><td>" << mat(0,1) << "</td><td>" << mat(0,2) << "</td><td>" << mat(0,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(1,0) << "</td><td>" << mat(1,1) << "</td><td>" << mat(1,2) << "</td><td>" << mat(1,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(2,0) << "</td><td>" << mat(2,1) << "</td><td>" << mat(2,2) << "</td><td>" << mat(2,3) << "</td></tr>" << std::endl;
        os << "<tr><td>" << mat(3,0) << "</td><td>" << mat(3,1) << "</td><td>" << mat(3,2) << "</td><td>" << mat(3,3) << "</td></tr>" << std::endl;
        os << "</table>" << std::endl;
        break;
    case MatrixUsageTypeModelView:
        os << "<ul>";
        os << "<li>translate ";
        writePrettyPositionHTML(os, mat.getTrans(), refDrawable);
        os << "</li>" << std::endl;
        os << "<li>rotate " << mat.getRotate() << "</li>" << std::endl;
        os << "<li>scale " << mat.getScale() << "</li>" << std::endl;
        os << "</ul>";
        break;
    case MatrixUsageTypePerspective:
        {
            double fovy;
            double aspectRatio;
            double zNear, zFar;
            mat.getPerspective(fovy, aspectRatio, zNear, zFar);

            os << std::setprecision(12) << "<ul>";
            os << "<li>fovy " << fovy << "</li>" << std::endl;
            os << "<li>aspectRatio " << aspectRatio << "</li>" << std::endl;
            os << "<li>near/far " << zNear << "/" << zFar << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    case MatrixUsageTypeLookAt:
        {
            osg::Vec3d eye;
            osg::Vec3d center;
            osg::Vec3d up;
            mat.getLookAt(eye, center, up);

            os << "<ul>";
            os << "<li>eye ";
            writePrettyPositionHTML(os, eye, refDrawable);
            os << "</li>" << std::endl;
            os << "<li>center ";
            writePrettyPositionHTML(os, center, refDrawable);
            os << "</li>" << std::endl;
            os << "<li>up" << up << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    case MatrixUsageTypeFrustum:
        {
            double left, right, bottom, top;
            double zNear, zFar;
            mat.getFrustum(left, right, bottom, top, zNear, zFar);
            os << std::setprecision(12) << "<ul>";
            os << "<li>left/right " << left << "/" << right << "</li>" << std::endl;
            os << "<li>top/bottom " << top << "/" << bottom << "</li>" << std::endl;
            os << "<li>near/far " << zNear << "/" << zFar << "</li>" << std::endl;
            os << "</ul>";
        }
        break;
    }
}

bool osgImageToQImage(const osg::Image * image, QImage * qimage)
{
    bool ret = false;
    QImage::Format format;
    switch(image->getPixelFormat())
    {
    case GL_RGB:
        format = QImage::Format_RGB888;
        break;
    case GL_RGBA:
        //format = QImage::Format_ARGB32;
        format = QImage::Format_ARGB32_Premultiplied;
        break;
    case GL_LUMINANCE:
        format = QImage::Format_Indexed8;
        break;
    default:
        format = QImage::Format_Invalid;
        break;
    }

    if(format != QImage::Format_Invalid)
    {
        int bytesPerLine = image->getRowSizeInBytes();
        int width = image->s();
        int height = image->t();
        *qimage = QImage(image->data(), width, height, bytesPerLine, format);
        *qimage = QGLWidget::convertToGLFormat(*qimage);
        //ret = ret.mirrored(false, true);
        ret = true;
    }
    return ret;
}

} // namespace osg_helpers
} // namespace sgi
