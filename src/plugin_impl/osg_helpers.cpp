// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <osg/StateAttribute>
#include <osg/Texture>
#include <osg/Camera>
#include <osg/CoordinateSystemNode>
#include <osg/ObserverNodePath>
#include <osg/io_utils>
#include <osg/Shape>
#include <osg/CullingSet>

#include <osg/GL>

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/osg>
#include <sgi/helpers/osg_helper_nodes>

#include <algorithm>

/* AttribMask */
#ifndef GL_CURRENT_BIT
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff
#endif

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

std::string getObjectName(const osg::Referenced * object, bool includeAddr)
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

std::string getObjectName(const osg::Observer * object, bool includeAddr)
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

std::string getObjectName(const osg::Object * object, bool includeAddr)
{
    std::string ret;
    if(includeAddr)
    {
        std::stringstream buf;
        buf << (void*)object;
        if(object)
        {
            const std::string& name = object->getName();
            if(!name.empty())
                buf << ' ' << name;
        }
        ret = buf.str();
    }
    else
    {
        std::string ret = object?(object->getName()):"(null)";
        if(ret.empty())
        {
            std::stringstream buf;
            buf << (void*)object;
            ret = buf.str();
        }
    }
    return ret;
}

std::string getObjectNameAndType(const osg::Referenced * object, bool includeAddr)
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

std::string getObjectNameAndType(const osg::Observer * object, bool includeAddr)
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

std::string getObjectNameAndType(const osg::Object * object, bool includeAddr)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object, includeAddr) << " (" << getObjectTypename(object) << ")";
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
            ret = "ON";
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

std::string cullingMaskToString(unsigned cullingMask)
{
    std::string ret;
    if (cullingMask == 0u)
        ret = "visible";
    else
    {
        if (cullingMask & osg::CullingSet::VIEW_FRUSTUM_SIDES_CULLING)
            sgi::helpers::str_append(ret, "frustum");
        if (cullingMask & osg::CullingSet::NEAR_PLANE_CULLING)
            sgi::helpers::str_append(ret, "near-plane");
        if (cullingMask & osg::CullingSet::FAR_PLANE_CULLING)
            sgi::helpers::str_append(ret, "far-plane");
        if (cullingMask & osg::CullingSet::SMALL_FEATURE_CULLING)
            sgi::helpers::str_append(ret, "small-feature");
        if (cullingMask & osg::CullingSet::SMALL_FEATURE_CULLING)
            sgi::helpers::str_append(ret, "shadow-occlusion");
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

void writePrettyHTML(std::basic_ostream<char>& os, const osg::NodePath & path, bool includeNodeMask)
{
    if(path.empty())
        os << "&lt;empty&gt;";
    else
    {
        os << "<ol>";
        for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::Node * node = *it;
			os << "<li>";
			os << osg_helpers::getObjectNameAndType(node, true);
			if (includeNodeMask)
				os << " 0x" << std::hex << node->getNodeMask() << std::dec;
			os << "</li>" << std::endl;
        }
        os << "</ol>";
    }
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::ObserverNodePath & path, bool includeNodeMask)
{
    osg::NodePath nodepath;
    if(path.getNodePath(nodepath))
        writePrettyHTML(os, nodepath, includeNodeMask);
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
    case MatrixUsageTypeWindow:
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
    case MatrixUsageTypeWindow:
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

void writePrettyHTML(std::basic_ostream<char>& os, const osg::RefMatrixd * mat, MatrixUsageType type, const osg::Node * refNode)
{
    if (!mat)
        os << "<i>(null)</i>";
    else
        writePrettyHTML(os, *mat, type, refNode);
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::RefMatrixd * mat, MatrixUsageType type, const osg::Drawable * refDrawable)
{
    if (!mat)
        os << "<i>(null)</i>";
    else
        writePrettyHTML(os, *mat, type, refDrawable);
}

void writePrettyHTML(std::basic_ostream<char>& os, const osg::Viewport* object)
{
    if (!object)
        os << "<i>(null)</i>";
    else
    {
        os << "<tr><td>x</td><td>" << object->x() << "</td></tr>" << std::endl;
        os << "<tr><td>y</td><td>" << object->y() << "</td></tr>" << std::endl;
        os << "<tr><td>width</td><td>" << object->width() << "</td></tr>" << std::endl;
        os << "<tr><td>height</td><td>" << object->height() << "</td></tr>" << std::endl;
        os << "<tr><td>aspect ratio</td><td>" << object->aspectRatio() << "</td></tr>" << std::endl;
    }
}

const sgi::Image * convertImage(const osg::Image * image)
{
    if (!image)
        return NULL;

    sgi::Image * ret = NULL;
    sgi::Image::ImageFormat imageFormat;
    sgi::Image::DataType dataType;
    switch(image->getPixelFormat())
    {
    case 0:imageFormat = sgi::Image::ImageFormatInvalid; break;
    case GL_RGB8:
    case GL_RGB: imageFormat = sgi::Image::ImageFormatRGB24; break;
    case GL_RGBA8:
    case GL_RGBA:imageFormat = sgi::Image::ImageFormatRGBA32; break;
    case GL_BGR: imageFormat = sgi::Image::ImageFormatBGR24; break;
    case GL_BGRA:imageFormat = sgi::Image::ImageFormatBGRA32; break;
    case GL_DEPTH_COMPONENT:imageFormat = sgi::Image::ImageFormatDepth; break;
    case GL_RED:imageFormat = sgi::Image::ImageFormatRed; break;
    case GL_GREEN:imageFormat = sgi::Image::ImageFormatGreen; break;
    case GL_BLUE:imageFormat = sgi::Image::ImageFormatBlue; break;
    case GL_ALPHA:imageFormat = sgi::Image::ImageFormatAlpha; break;
    case GL_LUMINANCE: imageFormat = sgi::Image::ImageFormatLuminance; break;
    case GL_LUMINANCE_ALPHA:imageFormat = sgi::Image::ImageFormatLuminanceAlpha; break;
    case GL_COMPRESSED_ALPHA:imageFormat = sgi::Image::ImageFormatAlpha; break;
    case GL_COMPRESSED_LUMINANCE:imageFormat = sgi::Image::ImageFormatLuminance; break;
    case GL_COMPRESSED_LUMINANCE_ALPHA:imageFormat = sgi::Image::ImageFormatLuminanceAlpha; break;
    case GL_COMPRESSED_INTENSITY:imageFormat = sgi::Image::ImageFormatGray; break;
    case GL_COMPRESSED_RGB:imageFormat = sgi::Image::ImageFormatRGB24; break;
    case GL_COMPRESSED_RGBA:imageFormat = sgi::Image::ImageFormatRGBA32; break;
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: imageFormat = sgi::Image::ImageFormatDXT1; break;
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: imageFormat = sgi::Image::ImageFormatDXT1Alpha; break;
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: imageFormat = sgi::Image::ImageFormatDXT3; break;
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: imageFormat = sgi::Image::ImageFormatDXT5; break;
    case GL_RG:
    case GL_RG_INTEGER:
    case GL_R8:
    case GL_R16:
    case GL_RG8:
    case GL_RG16:
    case GL_R16F:
    case GL_R32F:
    case GL_RG16F:
    case GL_RG32F:
    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
        switch (image->getInternalTextureFormat())
        {
        case GL_RED:imageFormat = sgi::Image::ImageFormatRed; break;
        case GL_GREEN:imageFormat = sgi::Image::ImageFormatGreen; break;
        case GL_BLUE:imageFormat = sgi::Image::ImageFormatBlue; break;
        case GL_ALPHA:imageFormat = sgi::Image::ImageFormatAlpha; break;
        case GL_LUMINANCE: imageFormat = sgi::Image::ImageFormatLuminance; break;
        default: imageFormat = sgi::Image::ImageFormatInvalid; break;
        }
        break;
    default: imageFormat = sgi::Image::ImageFormatInvalid; break;
    }
    switch (image->getDataType())
    {
    case GL_BYTE: dataType = sgi::Image::DataTypeSignedByte; break;
    case GL_UNSIGNED_BYTE: dataType = sgi::Image::DataTypeUnsignedByte; break;
    case GL_SHORT: dataType = sgi::Image::DataTypeSignedShort; break;
    case GL_UNSIGNED_SHORT: dataType = sgi::Image::DataTypeUnsignedShort; break;
    case GL_INT: dataType = sgi::Image::DataTypeSignedInt; break;
    case GL_UNSIGNED_INT: dataType = sgi::Image::DataTypeUnsignedInt; break;
    case GL_FLOAT: dataType = sgi::Image::DataTypeFloat32; break;
    case GL_DOUBLE: dataType = sgi::Image::DataTypeDouble; break;
    default: dataType = sgi::Image::DataTypeInvalid; break;
    }
    sgi::Image::Origin origin = (image->getOrigin() == osg::Image::TOP_LEFT) ? sgi::Image::OriginTopLeft : sgi::Image::OriginBottomLeft;
    if(imageFormat != sgi::Image::ImageFormatInvalid)
    {
        ret = new sgi::Image(imageFormat, dataType, origin,
                            const_cast<unsigned char*>(image->data()), image->getTotalDataSize(),
                            image->s(), image->t(), image->r(), image->getRowStepInBytes(),
                            image);
    }
    return ret;
}
const sgi::Image * convertImage(const osg::HeightField * hf)
{
    if (!hf)
        return NULL;
    sgi::Image * ret = NULL;
    sgi::Image::ImageFormat imageFormat = sgi::Image::ImageFormatFloat;
    sgi::Image::DataType dataType = sgi::Image::DataTypeFloat32;
    sgi::Image::Origin origin = sgi::Image::OriginBottomLeft;

    const osg::FloatArray* data = hf->getFloatArray();

    unsigned bytesPerLine = hf->getNumColumns() * sizeof(float);
    ret = new sgi::Image(imageFormat, dataType, origin,
        const_cast<unsigned char*>((const unsigned char*)data->getDataPointer()), data->getDataSize(),
        hf->getNumColumns(), hf->getNumRows(), 1, bytesPerLine,
        hf);
    return ret;
}


void heightFieldDumpHTML(std::basic_ostream<char>& os, const osg::HeightField * hf)
{
	os << "<pre>";
	heightFieldDumpPlainText(os, hf);
	os << "</pre>";
}

void heightFieldDumpPlainText(std::basic_ostream<char>& os, const osg::HeightField * hf)
{
	unsigned max_rows = hf->getNumRows();
	unsigned max_cols = hf->getNumColumns();
	float min_height = FLT_MAX;
	float max_height = FLT_MIN;
	float sum_height = 0;
	unsigned num_sum = 0;
	unsigned num_nodata = 0;
	for (unsigned row = 0; row < max_rows; ++row)
	{
		for (unsigned col = 0; col < max_cols; ++col)
		{
			float h = hf->getHeight(col, row);
			if (h == -FLT_MAX)
				++num_nodata;
			else
			{
				min_height = std::min(min_height, h);
				max_height = std::max(max_height, h);
				sum_height += h;
				++num_sum;
			}
		}
	}
	os << "min " << std::setw(7) << (num_sum ? min_height : 0) << std::endl;
	os << "max " << std::setw(7) << (num_sum ? max_height : 0) << std::endl;
	os << "avg " << std::setw(7) << (num_sum ? (sum_height / num_sum) : -FLT_MAX) << std::endl;
	os << "nodata " << num_nodata << '/' << (num_sum + num_nodata) << std::endl;
	os << std::setfill(' ') << "     ";
	for (unsigned col = 0; col < max_cols; ++col)
	{
		os << std::setw(7) << col << std::setw(0) << ' ';
	}
	os << std::endl;
	for (unsigned row = 0; row < max_rows; ++row)
	{
		os << std::setfill(' ') << std::setw(3) << row << std::setw(0) << ':' << ' ';
		os << std::setfill(' ') << std::fixed << std::setprecision(1);
		for (unsigned col = 0; col < max_cols; ++col)
		{
			float h = hf->getHeight(col, row);
			os << std::setw(7);
			if (h == -FLT_MAX)
				os << "n/a";
			else
				os << h;
			os << std::setw(0);
			os << ' ';
		}
		os << std::endl;
	}
}

osg::Camera * findCamera(const osg::Node * node)
{
    osg::Camera * ret = dynamic_cast<osg::Camera*>(const_cast<osg::Node*>(node));
    if (!ret)
        ret = findFirstParentOfType<osg::Camera>(const_cast<osg::Node*>(node));
    return ret;
}

osg::Camera * findCamera(const osg::StateSet * stateset)
{
    osg::Camera * ret = NULL;
    const osg::StateSet::ParentList & parents = stateset->getParents();
    for (osg::Node * ss : parents)
    {
        ret = findCamera(ss);
        if (ret)
            break;
    }
    return ret;
}

osg::Camera * findCamera(const osg::StateAttribute * sa)
{
    osg::Camera * ret = NULL;
    const osg::StateAttribute::ParentList & parents = sa->getParents();
    for (osg::StateSet * ss : parents)
    {
        ret = findCamera(ss);
        if (ret)
            break;
    }
    return ret;
}

unsigned findContextID(const osg::Camera * camera)
{
    unsigned ret = ~0u;
    if (camera)
    {
        const osg::GraphicsContext * gc = camera->getGraphicsContext();
        if (gc)
        {
            const osg::State * state = gc->getState();
            if (state)
                ret = state->getContextID();
        }
    }
    return ret;
}

unsigned findContextID(const osg::Node * node)
{
    osg::Camera * camera = findCamera(node);
    return findContextID(camera);
}

unsigned findContextID(const osg::StateSet * stateset)
{
    osg::Camera * camera = findCamera(stateset);
    return findContextID(camera);
}

unsigned findContextID(const osg::StateAttribute * sa)
{
    osg::Camera * camera = findCamera(sa);
    return findContextID(camera);
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingBoxf & b)
{
    return output << std::setprecision(12) << '[' << b._min << ',' << b._max << ']';
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingBoxd & b)
{
    return output << std::setprecision(12) << '[' << b._min << ',' << b._max << ']';
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingSpheref & b)
{
    return output << std::setprecision(12) << '[' << b._center << ",r=" << b._radius << ']';
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::BoundingSphered & b)
{
    return output << std::setprecision(12) << '[' << b._center << ",r=" << b._radius << ']';
}

osg::Texture * FindTreeItemNodeVisitor::NodeItem::getImageGeodeTexture() const
{
    osg::Texture * ret = nullptr;
    // ... and if it is a image geode try to add the image to the tree as well
    osg::StateSet* stateSet = node ? node->getStateSet() : nullptr;
    if (stateSet)
    {
        osg::StateAttribute * sa = stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
        ret = sa ? sa->asTexture() : nullptr;
    }
    return ret;
}

osg::Image * FindTreeItemNodeVisitor::NodeItem::getImage() const
{
    osg::Texture * texture = getImageGeodeTexture();
    return (texture) ? texture->getImage(0) : nullptr;
}

osg::HeightField * FindTreeItemNodeVisitor::NodeItem::getHeightField() const
{
    osg::UserDataContainer * container = node ? node->getUserDataContainer() : nullptr;
    return container ? (osg::HeightField *)container->getUserObject(0) : nullptr;
}

void FindTreeItemNodeVisitor::apply(osg::Node& node)
{
    bool sgi_tree_item = false;
    if (node.getUserValue<bool>("sgi_tree_item", sgi_tree_item))
    {
        if (sgi_tree_item)
        {
            NodeItem item;
            item.node = &node;
            item.imageGeode = false;
            item.heightField = false;
            node.getUserValue<std::string>("sgi_tree_itemname", item.name);
            if (!node.getUserValue<bool>("sgi_tree_imagegeode", item.imageGeode))
                item.imageGeode = false;
            if (!node.getUserValue<bool>("sgi_tree_heightfield", item.heightField))
                item.heightField = false;
            _nodes.push_back(item);
        }
    }
    traverse(node);
}


    } // namespace osg_helpers
} // namespace sgi
