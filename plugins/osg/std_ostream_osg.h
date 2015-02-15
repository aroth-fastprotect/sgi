#pragma once

#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/CoordinateSystemNode>
#include <vector>
#include <osg/io_utils>

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
}

namespace sgi {
namespace osg_plugin {

inline const osg::EllipsoidModel & getDefaultEllipsoid()
{
    static osg::EllipsoidModel s_default;
    return s_default;
}

inline osg::Vec3d fromECEFToXYZ(const osg::EllipsoidModel & ellipsoid, const osg::Vec3d & coord)
{
    osg::Vec3d ret;
    ellipsoid.convertLatLongHeightToXYZ(osg::DegreesToRadians(coord.x()), osg::DegreesToRadians(coord.y()), coord.z(), ret.x(), ret.y(), ret.z());
    return ret;
}

inline osg::Vec3d fromXYZToECEF(const osg::EllipsoidModel & ellipsoid, const osg::Vec3d & v)
{
    osg::Vec3d ret;
    ellipsoid.convertXYZToLatLongHeight(v.x(), v.y(), v.z(), ret.x(), ret.y(), ret.z());
    ret.x() = osg::RadiansToDegrees(ret.x());
    ret.y() = osg::RadiansToDegrees(ret.y());
    return ret;
}

inline osg::Vec3d fromECEFToXYZ(const osg::Vec3d & coord)
{
    return fromECEFToXYZ(getDefaultEllipsoid(), coord);
}

inline osg::Vec3d fromXYZToECEF(const osg::Vec3d & v)
{
    return fromXYZToECEF(getDefaultEllipsoid(), v);
}

inline void getHPRFromQuat(const osg::Quat& q, double &h, double &p, double &r)
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

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec2f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec3f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec4f & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec2d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec3d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Vec4d & v)
{
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")" << std::endl;
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Quat & q)
{
    double h, p, r;
    getHPRFromQuat(q, h, p, r);
    os << "h=" << osg::RadiansToDegrees(h) << ";p=" << osg::RadiansToDegrees(p) << ";r=" << osg::RadiansToDegrees(r) << " raw=";
    osg::Vec4d v = q.asVec4();
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::NodePath & path)
{
    if(path.empty())
        os << "&lt;empty&gt;";
    else
    {
        os << "<ol>";
        for(osg::NodePath::const_iterator it = path.begin(); it != path.end(); it++)
        {
            const osg::Node * node = *it;
            os << "<li>" << getObjectNameAndType(node) << "</li>" << std::endl;
        }
        os << "</ol>";
    }
}

#ifdef OSG_OBSERVERNODEPATH
inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::ObserverNodePath & path)
{
    osg::NodePath nodepath;
    if(path.getNodePath(nodepath))
        writePrettyHTML(os, nodepath);
    else
        os << "&lt;none&gt;";
}
#endif // OSG_OBSERVERNODEPATH

inline void writePrettyPositionHTML(std::basic_ostream<char>& os, const osg::Vec3 & v, const osg::Node * refNode=NULL)
{
    osg::Vec3d ecef = fromXYZToECEF(v);
    os << std::setprecision(12) << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")@("
        << ecef[0] << ", " << ecef[1] << ", " << ecef[2] << ")" << std::endl;
}

inline void writePrettyPositionHTML(std::basic_ostream<char>& os, const osg::Vec3 & v, const osg::Drawable * refDrawable=NULL)
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

inline void writePrettyRotationHTML(std::basic_ostream<char>& os, const osg::Quat & q)
{
    writePrettyHTML(os, q);
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingSphere & bs, const osg::Node * refNode=NULL)
{
    const osg::BoundingSphere::vec_type & center = bs.center();
    const osg::BoundingSphere::value_type & radius = bs.radius();

    os << "center=";
    writePrettyPositionHTML(os, center, refNode);
    os << "<br/>radius=" << radius << (bs.valid()?" valid":" invalid");
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingSphere * bs, const osg::Node * refNode=NULL)
{
    if(bs)
        writePrettyHTML(os, *bs, refNode);
    else
        os << "&lt;null&gt;";
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox & bb, const osg::Node * refNode)
{
    os << "center=";
    writePrettyPositionHTML(os, bb.center(), refNode);
    os << "<br/>min=";
    writePrettyPositionHTML(os, bb._min, refNode);
    os << "<br/>max=";
    writePrettyPositionHTML(os, bb._max, refNode);
    os << "<br/>radius=" << bb.radius() << (bb.valid()?" valid":" invalid");
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox & bb, const osg::Drawable * refDrawable)
{
    os << "center=";
    writePrettyPositionHTML(os, bb.center(), refDrawable);
    os << "<br/>min=";
    writePrettyPositionHTML(os, bb._min, refDrawable);
    os << "<br/>max=";
    writePrettyPositionHTML(os, bb._max, refDrawable);
    os << "<br/>radius=" << bb.radius() << (bb.valid()?" valid":" invalid");
}

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::BoundingBox * bb, const osg::Drawable * refDrawable)
{
    if(bb)
        writePrettyHTML(os, *bb, refDrawable);
    else
        os << "&lt;null&gt;";
}

enum MatrixUsageType
{
    MatrixUsageTypeGeneric = 0,
    MatrixUsageTypeModelView,
    MatrixUsageTypePerspective,
    MatrixUsageTypeLookAt,
    MatrixUsageTypeFrustum,
    MatrixUsageTypeOrtho,
};

inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Matrixd & mat, MatrixUsageType type, const osg::Node * refNode=NULL)
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
inline void writePrettyHTML(std::basic_ostream<char>& os, const osg::Matrixd & mat, MatrixUsageType type, const osg::Drawable * refDrawable)
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

} // namespace osg_plugin
} // namespace sgi
