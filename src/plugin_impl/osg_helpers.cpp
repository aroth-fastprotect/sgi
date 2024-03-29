// kate: syntax C++;
// SGI - Copyright (C) 2012-2019 FAST Protect, Andreas Roth

#include <osg/StateAttribute>
#include <osg/Texture>
#include <osg/Camera>
#include <osg/CoordinateSystemNode>
#include <osg/ObserverNodePath>
#include <osg/io_utils>
#include <osg/Shape>
#include <osg/CullingSet>

#include <osgUtil/SceneView>
#include <osgViewer/Renderer>

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

#ifndef GL_EXT_texture_storage
#define GL_EXT_texture_storage 1
#define GL_TEXTURE_IMMUTABLE_FORMAT_EXT   0x912F
#define GL_ALPHA8_EXT                     0x803C
#define GL_LUMINANCE8_EXT                 0x8040
#define GL_LUMINANCE8_ALPHA8_EXT          0x8045
#define GL_RGBA32F_EXT                    0x8814
#define GL_RGB32F_EXT                     0x8815
#define GL_ALPHA32F_EXT                   0x8816
#define GL_LUMINANCE32F_EXT               0x8818
#define GL_LUMINANCE_ALPHA32F_EXT         0x8819
#define GL_ALPHA16F_EXT                   0x881C
#define GL_LUMINANCE16F_EXT               0x881E
#define GL_LUMINANCE_ALPHA16F_EXT         0x881F
#define GL_R32F_EXT                       0x822E
#define GL_RG32F_EXT                      0x8230
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
    const osg::Node * refNode = (refDrawable && refDrawable->getNumParents() > 0)?refDrawable->getParent(0):nullptr;
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
        return nullptr;

    sgi::Image * ret = nullptr;
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
    case GL_RGBA32F_EXT:imageFormat = sgi::Image::ImageFormatRGBA32; break;
    case GL_RGB32F_EXT:imageFormat = sgi::Image::ImageFormatRGB32; break;
    case GL_ALPHA32F_EXT:imageFormat = sgi::Image::ImageFormatAlpha; break;
    case GL_LUMINANCE32F_EXT: imageFormat = sgi::Image::ImageFormatLuminance; break;
    case GL_LUMINANCE_ALPHA32F_EXT: imageFormat = sgi::Image::ImageFormatLuminance; break;
    case GL_ALPHA16F_EXT:imageFormat = sgi::Image::ImageFormatAlpha; break;
    case GL_LUMINANCE16F_EXT: imageFormat = sgi::Image::ImageFormatLuminance; break;
    case GL_LUMINANCE_ALPHA16F_EXT: imageFormat = sgi::Image::ImageFormatLuminance; break;
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
        return nullptr;
    sgi::Image * ret = nullptr;
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
    osg::Camera * ret = nullptr;
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
    osg::Camera * ret = nullptr;
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

osg::StateSet* buildEffectiveStateSet(const osg::NodePath & path, osg::StateSet* top)
{
    if(path.empty())
        return nullptr;
    
    osg::StateSet* ret = new osg::StateSet;
    for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); ++it)
    {
        osg::Node * node = *it;
        osg::StateSet * cur = node->getStateSet();
        if(cur)
            ret->merge(*cur);
    }
    if(top)
        ret->merge(*top);
    return ret;
}

bool collectStateSetList(const osg::NodePath & path, StateSetList & list)
{
    if (path.empty())
        return false;

    osg::StateSet* ret = new osg::StateSet;
    for (osg::NodePath::const_iterator it = path.begin(); it != path.end(); ++it)
    {
        osg::Node * node = *it;
        osg::StateSet * cur = node->getStateSet();
        if (cur)
            list.push_back(cur);
    }
    return true;
}

bool collectUniformList(const osg::NodePath & path, UniformList & list, bool & gotStateUniforms)
{
    if (path.empty())
        return false;

    for (osg::NodePath::const_iterator it = path.begin(); it != path.end(); ++it)
    {
        osg::Node * node = *it;
        if (!gotStateUniforms)
        {
            osg::Camera * camera = node->asCamera();
            osg::GraphicsContext * ctx = camera ? camera->getGraphicsContext() : nullptr;
            osg::State * state = ctx ? ctx->getState() : nullptr;
            if (state)
            {
#define ADD_STATE_UNIFORM(get_func) \
            { osg::Uniform * u = get_func(); list.insert(UniformList::value_type(u->getName(), RefUniformPair(u, osg::StateAttribute::ON) )); }
                ADD_STATE_UNIFORM(state->getModelViewMatrixUniform);
                ADD_STATE_UNIFORM(state->getProjectionMatrixUniform);
                ADD_STATE_UNIFORM(state->getModelViewProjectionMatrixUniform);
                ADD_STATE_UNIFORM(state->getNormalMatrixUniform);
#undef ADD_STATE_UNIFORM
            }
            osgViewer::Renderer * renderer = camera ? dynamic_cast<osgViewer::Renderer *>(camera->getRenderer()) : nullptr;
            osgUtil::SceneView * sceneview = renderer ? renderer->getSceneView(0) : nullptr;
            if (sceneview)
            {
                osg::StateSet * localStateSet = sceneview->getLocalStateSet();
                if (localStateSet)
                {
                    const osg::StateSet::UniformList & curlist = localStateSet->getUniformList();
                    for (auto itl : curlist)
                        list.insert(UniformList::value_type(itl.first, itl.second));
                }
            }
            gotStateUniforms = true;
        }
        osg::StateSet * cur = node->getStateSet();
        if (cur)
        {
            const osg::StateSet::UniformList & curlist = cur->getUniformList();
            for (auto itl : curlist)
                list.insert(UniformList::value_type(itl.first, itl.second));
        }
    }
    return true;
}

bool collectUniformList(const osg::NodePath & path, UniformList & list)
{
    bool gotStateUniforms = false;
    return collectUniformList(path, list, gotStateUniforms);
}

bool collectUniformList(const osg::StateSet * stateSet, UniformList & list)
{
    bool ret = false;
    bool gotStateUniforms = false;
    for (osg::Node * parent : stateSet->getParents())
    {
        for (const osg::NodePath & path : parent->getParentalNodePaths())
        {
            if (collectUniformList(path, list, gotStateUniforms))
                ret = true;
        }
    }
    const osg::StateSet::UniformList & curlist = stateSet->getUniformList();
    for (auto itl : curlist)
        list.insert(UniformList::value_type(itl.first, itl.second));

    return ret;
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

FindTreeItemNodeVisitor::FindTreeItemNodeVisitor(TraversalMode tm)
    : osg::NodeVisitor(tm)
{
    // find all nodes even when hidden
    setNodeMaskOverride(~0u);
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


osg::Geometry* createImageGeometry(float s, float t, osg::Image::Origin origin, osg::Texture * texture)
{
    osg::Geometry* geom = nullptr;
    float y = 1.0;
    float x = y * (s / t);

    float texcoord_y_b = (origin == osg::Image::BOTTOM_LEFT) ? 0.0f : 1.0f;
    float texcoord_y_t = (origin == osg::Image::BOTTOM_LEFT) ? 1.0f : 0.0f;
    float texcoord_x = 1.0f;

    // set up the drawstate.
    osg::StateSet* dstate = new osg::StateSet;
    dstate->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    dstate->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    dstate->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

    geom = new osg::Geometry;
    geom->setStateSet(dstate);

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0].set(-x, 0.0f, y);
    (*coords)[1].set(-x, 0.0f, -y);
    (*coords)[2].set(x, 0.0f, -y);
    (*coords)[3].set(x, 0.0f, y);
    geom->setVertexArray(coords);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f*texcoord_x, texcoord_y_t);
    (*tcoords)[1].set(0.0f*texcoord_x, texcoord_y_b);
    (*tcoords)[2].set(1.0f*texcoord_x, texcoord_y_b);
    (*tcoords)[3].set(1.0f*texcoord_x, texcoord_y_t);
    geom->setTexCoordArray(0, tcoords);

    osg::Vec4Array* colours = new osg::Vec4Array(1);
    (*colours)[0].set(1.0f, 1.0f, 1.0, 1.0f);
    geom->setColorArray(colours, osg::Array::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    return geom;
}

osg::Geometry* createGeometryForImage(osg::Image* image, float s, float t)
{
    osg::Geometry* geom = nullptr;
    if (image && s > 0 && t > 0)
    {
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
        texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
        texture->setResizeNonPowerOfTwoHint(false);
        geom = createImageGeometry(s, t, image->getOrigin(), texture);
    }
    return geom;
}
osg::Geometry * createGeometryForImage(osg::Image* image)
{
    return createGeometryForImage(image, image->s(), image->t());
}

osg::Geometry * createGeometryForTexture(osg::Texture* texture)
{
    return createImageGeometry(texture->getTextureWidth(), texture->getTextureHeight(), osg::Image::BOTTOM_LEFT, texture);
}

// see osg\io_utils
template<int num_rows, int num_cols, typename T>
inline std::ostream& matrix_out(std::ostream& os, const T& m)
{
    os << "{" << std::endl;
    os << std::setprecision(12);
    for (int row = 0; row < num_rows; ++row) {
        os << "\t";
        for (int col = 0; col < num_cols; ++col)
            os << m(row, col) << " ";
        os << std::endl;
    }
    os << "}" << std::endl;
    return os;
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2 & m)
{
    return matrix_out<2,2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3 & m)
{
    return matrix_out<3, 3>(output, m);
}
#if 0
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrixf & m)
{
    return matrix_out<4, 4>(output, m);
}
#endif
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2x3 & m)
{
    return matrix_out<2, 3>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2x4 & m)
{
    return matrix_out<2, 4>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3x2 & m)
{
    return matrix_out<3, 2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3x4 & m)
{
    return matrix_out<3, 4>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix4x2 & m)
{
    return matrix_out<4, 2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix4x3 & m)
{
    return matrix_out<4, 3>(output, m);
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2d & m)
{
    return matrix_out<2, 2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3d & m)
{
    return matrix_out<3, 3>(output, m);
}
#if 0
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrixd & m)
{
    return matrix_out<4, 4>(output, m);
}
#endif
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2x3d & m)
{
    return matrix_out<2, 3>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix2x4d & m)
{
    return matrix_out<2, 4>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3x2d & m)
{
    return matrix_out<3, 2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix3x4d & m)
{
    return matrix_out<3, 4>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix4x2d & m)
{
    return matrix_out<4, 2>(output, m);
}
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& output, const osg::Matrix4x3d & m)
{
    return matrix_out<4, 3>(output, m);
}


#define uniformToString_Data(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                (void(0)); \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val; \
                object->getElement(0, val); \
                os << val; \
            } \
            else { \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    if(n != 0) os << ','; \
                    __c_type val; \
                    object->getElement(n, val); \
                    os << val; \
                } \
            } \
        } \
        break

#define uniformToString_Sampler(__gl_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                (void(0)); \
            else if( object->getNumElements() == 1) \
            { \
                int val; \
                object->getElement(0, val); \
                os << val; \
            } \
            else { \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    if(n != 0) os << ','; \
                    int val; \
                    object->getElement(n, val); \
                    os << val; \
                } \
            } \
        } \
        break



std::string uniformToString(const osg::Uniform * object, bool * ok)
{
    std::stringstream os;
    if (ok)
        *ok = true;
    switch (object->getType())
    {
    case osg::Uniform::UNDEFINED:
        os << "undefined";
        break;
        uniformToString_Data(BOOL, bool);
        uniformToString_Data(FLOAT, float);
        uniformToString_Data(FLOAT_VEC2, osg::Vec2f);
        uniformToString_Data(FLOAT_VEC3, osg::Vec3f);
        uniformToString_Data(FLOAT_VEC4, osg::Vec4f);
        uniformToString_Data(DOUBLE, double);
        uniformToString_Data(DOUBLE_VEC2, osg::Vec2d);
        uniformToString_Data(DOUBLE_VEC3, osg::Vec3d);
        uniformToString_Data(DOUBLE_VEC4, osg::Vec4d);
        uniformToString_Data(INT, int);
        //uniformToString_Data(INT_VEC2, osg::Vec2i);
        //uniformToString_Data(INT_VEC3, osg::Vec3i);
        //uniformToString_Data(INT_VEC4, osg::Vec4i);
        uniformToString_Data(UNSIGNED_INT, unsigned int);
        //uniformToString_Data(UNSIGNED_INT_VEC2, osg::Vec2ui);
        //uniformToString_Data(UNSIGNED_INT_VEC3, osg::Vec3ui);
        //uniformToString_Data(UNSIGNED_INT_VEC4, osg::Vec4ui);

        uniformToString_Data(FLOAT_MAT2, osg::Matrix2);
        uniformToString_Data(FLOAT_MAT3, osg::Matrix3);
        uniformToString_Data(FLOAT_MAT4, osg::Matrixf);
        uniformToString_Data(FLOAT_MAT2x3, osg::Matrix2x3);
        uniformToString_Data(FLOAT_MAT2x4, osg::Matrix2x4);
        uniformToString_Data(FLOAT_MAT3x2, osg::Matrix3x2);
        uniformToString_Data(FLOAT_MAT3x4, osg::Matrix3x4);
        uniformToString_Data(FLOAT_MAT4x2, osg::Matrix4x2);
        uniformToString_Data(FLOAT_MAT4x3, osg::Matrix4x3);

        uniformToString_Data(DOUBLE_MAT2, osg::Matrix2d);
        uniformToString_Data(DOUBLE_MAT3, osg::Matrix3d);
        uniformToString_Data(DOUBLE_MAT4, osg::Matrixd);
        uniformToString_Data(DOUBLE_MAT2x3, osg::Matrix2x3d);
        uniformToString_Data(DOUBLE_MAT2x4, osg::Matrix2x4d);
        uniformToString_Data(DOUBLE_MAT3x2, osg::Matrix3x2d);
        uniformToString_Data(DOUBLE_MAT3x4, osg::Matrix3x4d);
        uniformToString_Data(DOUBLE_MAT4x2, osg::Matrix4x2d);
        uniformToString_Data(DOUBLE_MAT4x3, osg::Matrix4x3d);

        uniformToString_Sampler(SAMPLER_1D);
        uniformToString_Sampler(SAMPLER_2D);
        uniformToString_Sampler(SAMPLER_3D);
        uniformToString_Sampler(SAMPLER_CUBE);
        uniformToString_Sampler(SAMPLER_1D_SHADOW);
        uniformToString_Sampler(SAMPLER_2D_SHADOW);
        uniformToString_Sampler(SAMPLER_1D_ARRAY);
        uniformToString_Sampler(SAMPLER_2D_ARRAY);
        uniformToString_Sampler(SAMPLER_CUBE_MAP_ARRAY);
        uniformToString_Sampler(SAMPLER_1D_ARRAY_SHADOW);
        uniformToString_Sampler(SAMPLER_2D_ARRAY_SHADOW);
        uniformToString_Sampler(SAMPLER_2D_MULTISAMPLE);
        uniformToString_Sampler(SAMPLER_2D_MULTISAMPLE_ARRAY);
        uniformToString_Sampler(SAMPLER_CUBE_SHADOW);
        uniformToString_Sampler(SAMPLER_CUBE_MAP_ARRAY_SHADOW);
        uniformToString_Sampler(SAMPLER_BUFFER);
        uniformToString_Sampler(SAMPLER_2D_RECT);
        uniformToString_Sampler(SAMPLER_2D_RECT_SHADOW);
    default:
        os << "Type " << osg::Uniform::getTypename(object->getType()) << " not implemented";
        if (ok)
            *ok = false;
        break;
    }
    return os.str();
}


// see osg\io_utils
template<int num_rows, int num_cols, typename T>
inline std::istream& matrix_in(std::istream& is, T& m)
{
    for (int row = 0; row < num_rows; ++row) {
        for (int col = 0; col < num_cols; ++col)
        {
            is >> m(row, col);
        }
    }
    return is;
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2 & m)
{
    return matrix_in<2, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3 & m)
{
    return matrix_in<3, 3>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrixf & m)
{
    return matrix_in<4, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2x3 & m)
{
    return matrix_in<2, 3>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2x4 & m)
{
    return matrix_in<2, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3x2 & m)
{
    return matrix_in<3, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3x4 & m)
{
    return matrix_in<3, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix4x2 & m)
{
    return matrix_in<4, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix4x3 & m)
{
    return matrix_in<4, 3>(input, m);
}

std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2d & m)
{
    return matrix_in<2, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3d & m)
{
    return matrix_in<3, 3>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrixd & m)
{
    return matrix_in<4, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2x3d & m)
{
    return matrix_in<2, 3>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix2x4d & m)
{
    return matrix_in<2, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3x2d & m)
{
    return matrix_in<3, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix3x4d & m)
{
    return matrix_in<3, 4>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix4x2d & m)
{
    return matrix_in<4, 2>(input, m);
}
std::basic_istream<char>& operator>>(std::basic_istream<char>& input, osg::Matrix4x3d & m)
{
    return matrix_in<4, 3>(input, m);
}

#define stringToUniform_Data(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            unsigned num_elems = object->getNumElements(); \
            if( num_elems == 0) \
                ret = true; \
            else if( num_elems == 1) \
            { \
                __c_type val; \
                is >> val; \
                ret = object->setElement(0, val); \
            } \
            else { \
                ret = true; \
                for(unsigned n = 0; n < num_elems; n++) \
                { \
                    __c_type val; \
                    is >> val; \
                    if(!object->setElement(n, val)) \
                        ret = false; \
                } \
            } \
        } \
        break

#define stringToUniform_Sampler(__gl_type) \
    case osg::Uniform::__gl_type: \
        { \
            unsigned num_elems = object->getNumElements(); \
            if( num_elems == 0) \
                ret = true; \
            else if( num_elems == 1) \
            { \
                int val; \
                is >> val; \
                ret = object->setElement(0, val); \
            } \
            else { \
                ret = true; \
                for(unsigned n = 0; n < num_elems; n++) \
                { \
                    int val; \
                    is >> val; \
                    if(!object->setElement(n, val)) \
                        ret = false; \
                } \
            } \
        } \
        break

bool stringToUniform(const std::string & s, osg::Uniform * object)
{
    bool ret = false;
    std::stringstream is(s);
    switch (object->getType())
    {
    case osg::Uniform::UNDEFINED:
        ret = true;
        break;
        stringToUniform_Data(BOOL, bool);
        stringToUniform_Data(FLOAT, float);
        stringToUniform_Data(FLOAT_VEC2, osg::Vec2f);
        stringToUniform_Data(FLOAT_VEC3, osg::Vec3f);
        stringToUniform_Data(FLOAT_VEC4, osg::Vec4f);
        stringToUniform_Data(DOUBLE, double);
        stringToUniform_Data(DOUBLE_VEC2, osg::Vec2d);
        stringToUniform_Data(DOUBLE_VEC3, osg::Vec3d);
        stringToUniform_Data(DOUBLE_VEC4, osg::Vec4d);
        stringToUniform_Data(INT, int);
        //stringToUniform_Data(INT_VEC2, osg::Vec2i);
        //stringToUniform_Data(INT_VEC3, osg::Vec3i);
        //stringToUniform_Data(INT_VEC4, osg::Vec4i);
        stringToUniform_Data(UNSIGNED_INT, unsigned int);
        //stringToUniform_Data(UNSIGNED_INT_VEC2, osg::Vec2ui);
        //stringToUniform_Data(UNSIGNED_INT_VEC3, osg::Vec3ui);
        //stringToUniform_Data(UNSIGNED_INT_VEC4, osg::Vec4ui);

        stringToUniform_Data(FLOAT_MAT2, osg::Matrix2);
        stringToUniform_Data(FLOAT_MAT3, osg::Matrix3);
        stringToUniform_Data(FLOAT_MAT4, osg::Matrixf);
        stringToUniform_Data(FLOAT_MAT2x3, osg::Matrix2x3);
        stringToUniform_Data(FLOAT_MAT2x4, osg::Matrix2x4);
        stringToUniform_Data(FLOAT_MAT3x2, osg::Matrix3x2);
        stringToUniform_Data(FLOAT_MAT3x4, osg::Matrix3x4);
        stringToUniform_Data(FLOAT_MAT4x2, osg::Matrix4x2);
        stringToUniform_Data(FLOAT_MAT4x3, osg::Matrix4x3);

        stringToUniform_Data(DOUBLE_MAT2, osg::Matrix2d);
        stringToUniform_Data(DOUBLE_MAT3, osg::Matrix3d);
        stringToUniform_Data(DOUBLE_MAT4, osg::Matrixd);
        stringToUniform_Data(DOUBLE_MAT2x3, osg::Matrix2x3d);
        stringToUniform_Data(DOUBLE_MAT2x4, osg::Matrix2x4d);
        stringToUniform_Data(DOUBLE_MAT3x2, osg::Matrix3x2d);
        stringToUniform_Data(DOUBLE_MAT3x4, osg::Matrix3x4d);
        stringToUniform_Data(DOUBLE_MAT4x2, osg::Matrix4x2d);
        stringToUniform_Data(DOUBLE_MAT4x3, osg::Matrix4x3d);

        stringToUniform_Sampler(SAMPLER_1D);
        stringToUniform_Sampler(SAMPLER_2D);
        stringToUniform_Sampler(SAMPLER_3D);
        stringToUniform_Sampler(SAMPLER_CUBE);
        stringToUniform_Sampler(SAMPLER_1D_SHADOW);
        stringToUniform_Sampler(SAMPLER_2D_SHADOW);
        stringToUniform_Sampler(SAMPLER_1D_ARRAY);
        stringToUniform_Sampler(SAMPLER_2D_ARRAY);
        stringToUniform_Sampler(SAMPLER_CUBE_MAP_ARRAY);
        stringToUniform_Sampler(SAMPLER_1D_ARRAY_SHADOW);
        stringToUniform_Sampler(SAMPLER_2D_ARRAY_SHADOW);
        stringToUniform_Sampler(SAMPLER_2D_MULTISAMPLE);
        stringToUniform_Sampler(SAMPLER_2D_MULTISAMPLE_ARRAY);
        stringToUniform_Sampler(SAMPLER_CUBE_SHADOW);
        stringToUniform_Sampler(SAMPLER_CUBE_MAP_ARRAY_SHADOW);
        stringToUniform_Sampler(SAMPLER_BUFFER);
        stringToUniform_Sampler(SAMPLER_2D_RECT);
        stringToUniform_Sampler(SAMPLER_2D_RECT_SHADOW);
    default:
        break;
    }
    return ret;
}

#define uniformToHTML_Data_pre(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val; \
                object->getElement(0, val); \
                os << "<li><pre>" << val << "</pre></li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    __c_type val; \
                    object->getElement(n, val); \
                    os << "<li><pre>" << val << "</pre></li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break
#define uniformToHTML_Data(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val; \
                object->getElement(0, val); \
                os << "<li>" << val << "</li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    __c_type val; \
                    object->getElement(n, val); \
                    os << "<li>" << val << "</li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break
#define uniformToHTML_Data2(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val0, val1; \
                object->getElement(0, val0, val1); \
                os << "<li>" << val0 << ',' << val1 << "</li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    __c_type val0, val1; \
                    object->getElement(n, val0, val1); \
                    os << "<li>" << val0 << ',' << val1 << "</li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break
#define uniformToHTML_Data3(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val0, val1, val2; \
                object->getElement(0, val0, val1, val2); \
                os << "<li>" << val0 << ',' << val1 << ',' << val2 << "</li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    __c_type val0, val1, val2; \
                    object->getElement(n, val0, val1, val2); \
                    os << "<li>" << val0 << ',' << val1 << ',' << val2 << "</li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break
#define uniformToHTML_Data4(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                __c_type val0, val1, val2, val3; \
                object->getElement(0, val0, val1, val2, val3); \
                os << "<li>" << val0 << ',' << val1 << ',' << val2 << ',' << val3 << "</li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    __c_type val0, val1, val2, val3; \
                    object->getElement(n, val0, val1, val2, val3); \
                    os << "<li>" << val0 << ',' << val1 << ',' << val2 << ',' << val3 << "</li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break
#define uniformToHTML_Sampler(__gl_type) \
    case osg::Uniform::__gl_type: \
        { \
            os << std::setprecision(12); \
            if( object->getNumElements() == 0) \
                os << "<i>empty</i>"; \
            else if( object->getNumElements() == 1) \
            { \
                int val; \
                object->getElement(0, val); \
                os << "<li>Id=" << val << "</li>" << std::endl; \
            } \
            else { \
                os << "<ol>"; \
                for(unsigned n = 0, maxnum = object->getNumElements(); n < maxnum; n++) \
                { \
                    int val; \
                    object->getElement(n, val); \
                    os << "<li>Id=" << val << "</li>" << std::endl; \
                } \
                os << "</ol>"; \
            } \
        } \
        break

std::string uniformToHTML(const osg::Uniform * object, bool * ok)
{
    std::stringstream os;
    if (ok)
        *ok = true;
    switch (object->getType())
    {
    case osg::Uniform::UNDEFINED:
        os << "<i>undefined</i>";
        break;
        uniformToHTML_Data(BOOL, bool);
        uniformToHTML_Data2(BOOL_VEC2, bool);
        uniformToHTML_Data3(BOOL_VEC3, bool);
        uniformToHTML_Data4(BOOL_VEC4, bool);
        uniformToHTML_Data(FLOAT, float);
        uniformToHTML_Data(FLOAT_VEC2, osg::Vec2f);
        uniformToHTML_Data(FLOAT_VEC3, osg::Vec3f);
        uniformToHTML_Data(FLOAT_VEC4, osg::Vec4f);
        uniformToHTML_Data(DOUBLE, double);
        uniformToHTML_Data(DOUBLE_VEC2, osg::Vec2d);
        uniformToHTML_Data(DOUBLE_VEC3, osg::Vec3d);
        uniformToHTML_Data(DOUBLE_VEC4, osg::Vec4d);
        uniformToHTML_Data(INT, int);
        uniformToHTML_Data2(INT_VEC2, int);
        uniformToHTML_Data3(INT_VEC3, int);
        uniformToHTML_Data4(INT_VEC4, int);
        uniformToHTML_Data(UNSIGNED_INT, unsigned int);
        uniformToHTML_Data2(UNSIGNED_INT_VEC2, unsigned int);
        uniformToHTML_Data3(UNSIGNED_INT_VEC3, unsigned int);
        uniformToHTML_Data4(UNSIGNED_INT_VEC4, unsigned int);
        uniformToHTML_Data_pre(FLOAT_MAT2, osg::Matrix2);
        uniformToHTML_Data_pre(FLOAT_MAT3, osg::Matrix3);
        uniformToHTML_Data_pre(FLOAT_MAT4, osg::Matrix);
        uniformToHTML_Data_pre(FLOAT_MAT2x3, osg::Matrix2x3);
        uniformToHTML_Data_pre(FLOAT_MAT2x4, osg::Matrix2x4);
        uniformToHTML_Data_pre(FLOAT_MAT3x2, osg::Matrix3x2);
        uniformToHTML_Data_pre(FLOAT_MAT3x4, osg::Matrix3x4);
        uniformToHTML_Data_pre(FLOAT_MAT4x2, osg::Matrix4x2);
        uniformToHTML_Data_pre(FLOAT_MAT4x3, osg::Matrix4x3);
        uniformToHTML_Data_pre(DOUBLE_MAT2, osg::Matrix2d);
        uniformToHTML_Data_pre(DOUBLE_MAT3, osg::Matrix3d);
        uniformToHTML_Data_pre(DOUBLE_MAT4, osg::Matrixd);
        uniformToHTML_Data_pre(DOUBLE_MAT2x3, osg::Matrix2x3d);
        uniformToHTML_Data_pre(DOUBLE_MAT2x4, osg::Matrix2x4d);
        uniformToHTML_Data_pre(DOUBLE_MAT3x2, osg::Matrix3x2d);
        uniformToHTML_Data_pre(DOUBLE_MAT3x4, osg::Matrix3x4d);
        uniformToHTML_Data_pre(DOUBLE_MAT4x2, osg::Matrix4x2d);
        uniformToHTML_Data_pre(DOUBLE_MAT4x3, osg::Matrix4x3d);

        uniformToHTML_Sampler(SAMPLER_1D);
        uniformToHTML_Sampler(SAMPLER_2D);
        uniformToHTML_Sampler(SAMPLER_3D);
        uniformToHTML_Sampler(SAMPLER_CUBE);
        uniformToHTML_Sampler(SAMPLER_1D_SHADOW);
        uniformToHTML_Sampler(SAMPLER_2D_SHADOW);
        uniformToHTML_Sampler(SAMPLER_1D_ARRAY);
        uniformToHTML_Sampler(SAMPLER_2D_ARRAY);
        uniformToHTML_Sampler(SAMPLER_CUBE_MAP_ARRAY);
        uniformToHTML_Sampler(SAMPLER_1D_ARRAY_SHADOW);
        uniformToHTML_Sampler(SAMPLER_2D_ARRAY_SHADOW);
        uniformToHTML_Sampler(SAMPLER_2D_MULTISAMPLE);
        uniformToHTML_Sampler(SAMPLER_2D_MULTISAMPLE_ARRAY);
        uniformToHTML_Sampler(SAMPLER_CUBE_SHADOW);
        uniformToHTML_Sampler(SAMPLER_CUBE_MAP_ARRAY_SHADOW);
        uniformToHTML_Sampler(SAMPLER_BUFFER);
        uniformToHTML_Sampler(SAMPLER_2D_RECT);
        uniformToHTML_Sampler(SAMPLER_2D_RECT_SHADOW);
    default:
        os << "<i>Type " << osg::Uniform::getTypename(object->getType()) << " not implemented.</i>";
        if (ok)
            *ok = false;
        break;
    }
    return os.str();
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osg::StateAttribute::Type & t)
{
    return os << getStateAttributeTypeName(t);
}

std::string getStateAttributeTypeName(const osg::StateAttribute::Type & t)
{
    std::string ret;
    switch (t)
    {
    case osg::StateAttribute::TEXTURE: ret = "TEXTURE"; break;
    case osg::StateAttribute::POLYGONMODE: ret = "POLYGONMODE"; break;
    case osg::StateAttribute::POLYGONOFFSET: ret = "POLYGONOFFSET"; break;
    case osg::StateAttribute::MATERIAL: ret = "MATERIAL"; break;
    case osg::StateAttribute::ALPHAFUNC: ret = "ALPHAFUNC"; break;
    case osg::StateAttribute::ANTIALIAS: ret = "ANTIALIAS"; break;
    case osg::StateAttribute::COLORTABLE: ret = "COLORTABLE"; break;
    case osg::StateAttribute::CULLFACE: ret = "CULLFACE"; break;
    case osg::StateAttribute::FOG: ret = "FOG"; break;
    case osg::StateAttribute::FRONTFACE: ret = "FRONTFACE"; break;
    case osg::StateAttribute::LIGHT: ret = "LIGHT"; break;
    case osg::StateAttribute::POINT: ret = "POINT"; break;
    case osg::StateAttribute::LINEWIDTH: ret = "LINEWIDTH"; break;
    case osg::StateAttribute::LINESTIPPLE: ret = "LINESTIPPLE"; break;
    case osg::StateAttribute::POLYGONSTIPPLE: ret = "POLYGONSTIPPLE"; break;
    case osg::StateAttribute::SHADEMODEL: ret = "SHADEMODEL"; break;
    case osg::StateAttribute::TEXENV: ret = "TEXENV"; break;
    case osg::StateAttribute::TEXENVFILTER: ret = "TEXENVFILTER"; break;
    case osg::StateAttribute::TEXGEN: ret = "TEXGEN"; break;
    case osg::StateAttribute::TEXMAT: ret = "TEXMAT"; break;
    case osg::StateAttribute::LIGHTMODEL: ret = "LIGHTMODEL"; break;
    case osg::StateAttribute::BLENDFUNC: ret = "BLENDFUNC"; break;
    case osg::StateAttribute::BLENDEQUATION: ret = "BLENDEQUATION"; break;
    case osg::StateAttribute::LOGICOP: ret = "LOGICOP"; break;
    case osg::StateAttribute::STENCIL: ret = "STENCIL"; break;
    case osg::StateAttribute::COLORMASK: ret = "COLORMASK"; break;
    case osg::StateAttribute::DEPTH: ret = "DEPTH"; break;
    case osg::StateAttribute::VIEWPORT: ret = "VIEWPORT"; break;
    case osg::StateAttribute::SCISSOR: ret = "SCISSOR"; break;
    case osg::StateAttribute::BLENDCOLOR: ret = "BLENDCOLOR"; break;
    case osg::StateAttribute::MULTISAMPLE: ret = "MULTISAMPLE"; break;
    case osg::StateAttribute::CLIPPLANE: ret = "CLIPPLANE"; break;
    case osg::StateAttribute::COLORMATRIX: ret = "COLORMATRIX"; break;
    case osg::StateAttribute::VERTEXPROGRAM: ret = "VERTEXPROGRAM"; break;
    case osg::StateAttribute::FRAGMENTPROGRAM: ret = "FRAGMENTPROGRAM"; break;
    case osg::StateAttribute::POINTSPRITE: ret = "POINTSPRITE"; break;
    case osg::StateAttribute::PROGRAM: ret = "PROGRAM"; break;
    case osg::StateAttribute::CLAMPCOLOR: ret = "CLAMPCOLOR"; break;
    case osg::StateAttribute::HINT: ret = "HINT"; break;
    case osg::StateAttribute::SAMPLEMASKI: ret = "SAMPLEMASKI"; break;
    case osg::StateAttribute::PRIMITIVERESTARTINDEX: ret = "PRIMITIVERESTARTINDEX"; break;
    case osg::StateAttribute::CLIPCONTROL: ret = "CLIPCONTROL"; break;

        /// osgFX namespace
    case osg::StateAttribute::VALIDATOR: ret = "VALIDATOR"; break;
    case osg::StateAttribute::VIEWMATRIXEXTRACTOR: ret = "VIEWMATRIXEXTRACTOR"; break;
        /// osgNV namespace
    case osg::StateAttribute::OSGNV_PARAMETER_BLOCK: ret = "OSGNV_PARAMETER_BLOCK"; break;
        // osgNVExt namespace
    case osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER: ret = "OSGNVEXT_TEXTURE_SHADER"; break;
    case osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM: ret = "OSGNVEXT_VERTEX_PROGRAM"; break;
    case osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS: ret = "OSGNVEXT_REGISTER_COMBINERS"; break;
        /// osgNVCg namespace
    case osg::StateAttribute::OSGNVCG_PROGRAM: ret = "OSGNVCG_PROGRAM"; break;
        // osgNVSlang namespace
    case osg::StateAttribute::OSGNVSLANG_PROGRAM: ret = "OSGNVSLANG_PROGRAM"; break;
        // osgNVParse
    case osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER: ret = "OSGNVPARSE_PROGRAM_PARSER"; break;

    case osg::StateAttribute::UNIFORMBUFFERBINDING: ret = "UNIFORMBUFFERBINDING"; break;
    case osg::StateAttribute::TRANSFORMFEEDBACKBUFFERBINDING: ret = "TRANSFORMFEEDBACKBUFFERBINDING"; break;
    case osg::StateAttribute::ATOMICCOUNTERBUFFERBINDING: ret = "ATOMICCOUNTERBUFFERBINDING"; break;
    case osg::StateAttribute::PATCH_PARAMETER: ret = "PATCH_PARAMETER"; break;
    case osg::StateAttribute::FRAME_BUFFER_OBJECT: ret = "FRAME_BUFFER_OBJECT"; break;

    case osg::StateAttribute::VERTEX_ATTRIB_DIVISOR: ret = "VERTEX_ATTRIB_DIVISOR"; break;
    case osg::StateAttribute::SHADERSTORAGEBUFFERBINDING: ret = "SHADERSTORAGEBUFFERBINDING"; break;
    case osg::StateAttribute::INDIRECTDRAWBUFFERBINDING: ret = "INDIRECTDRAWBUFFERBINDING"; break;
    case osg::StateAttribute::VIEWPORTINDEXED: ret = "VIEWPORTINDEXED"; break;
    case osg::StateAttribute::DEPTHRANGEINDEXED: ret = "DEPTHRANGEINDEXED"; break;
    case osg::StateAttribute::SCISSORINDEXED: ret = "SCISSORINDEXED"; break;
    case osg::StateAttribute::BINDIMAGETEXTURE: ret = "BINDIMAGETEXTURE"; break;
    case osg::StateAttribute::SAMPLER: ret = "SAMPLER"; break;
    default:
        std::stringstream ss;
        ss << (int)t; 
        ret = ss.str();
        break;
    }
    return ret;
}
    } // namespace osg_helpers
} // namespace sgi
