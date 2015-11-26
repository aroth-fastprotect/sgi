// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <osg/StateAttribute>
#include <osg/Texture>
#include <osg/CoordinateSystemNode>
#include <osg/ObserverNodePath>
#include <osg/io_utils>
#include <osg/Shape>

#include <QImage>
#include <QGLWidget>

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/osg>

extern QImage decodeDDSImage(const osg::Image * image);

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

namespace {

    static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
    {
        if (texture_format == GL_BGRA) {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return ((src_pixel << 24) & 0xff000000)
                    | ((src_pixel >> 24) & 0x000000ff)
                    | ((src_pixel << 8) & 0x00ff0000)
                    | ((src_pixel >> 8) & 0x0000ff00);
            } else {
                return src_pixel;
            }
        } else {  // GL_RGBA
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
            } else {
                return ((src_pixel << 16) & 0xff0000)
                    | ((src_pixel >> 16) & 0xff)
                    | (src_pixel & 0xff00ff00);
            }
        }
    }

    QRgb qt_gl_convertToGLFormat(QRgb src_pixel, GLenum texture_format)
    {
        return qt_gl_convertToGLFormatHelper(src_pixel, texture_format);
    }

    static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format, GLint internal_format)
    {
        Q_ASSERT(dst.depth() == 32);
        Q_ASSERT(img.depth() == 32);

        if (dst.size() != img.size()) {
            int target_width = dst.width();
            int target_height = dst.height();
            qreal sx = target_width / qreal(img.width());
            qreal sy = target_height / qreal(img.height());

            quint32 *dest = (quint32 *) dst.scanLine(0); // NB! avoid detach here
            uchar *srcPixels = (uchar *) img.scanLine(img.height() - 1);
            int sbpl = img.bytesPerLine();
            int dbpl = dst.bytesPerLine();

            int ix = int(0x00010000 / sx);
            int iy = int(0x00010000 / sy);

            quint32 basex = int(0.5 * ix);
            quint32 srcy = int(0.5 * iy);

            // scale, swizzle and mirror in one loop
            while (target_height--) {
                const uint *src = (const quint32 *) (srcPixels - (srcy >> 16) * sbpl);
                int srcx = basex;
                for (int x=0; x<target_width; ++x) {
                    dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                    srcx += ix;
                }
                dest = (quint32 *)(((uchar *) dest) + dbpl);
                srcy += iy;
            }
        } else {
            const int width = img.width();
            const int height = img.height();
            const uint *p = (const uint*) img.scanLine(img.height() - 1);
            uint *q = (uint*) dst.scanLine(0);

            if (texture_format == GL_BGRA) {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    // mirror + swizzle
                    for (int i=0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = ((*p << 24) & 0xff000000)
                                | ((*p >> 24) & 0x000000ff)
                                | ((*p << 8) & 0x00ff0000)
                                | ((*p >> 8) & 0x0000ff00);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                } else {
					if (internal_format == GL_RGBA8) {
						p = (const uint*)img.scanLine(0);
						for (int i = 0; i < height; ++i) {
							const uint *end = p + width;
							while (p < end) {
								*q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
								p++;
								q++;
							}
						}
					}
					else {
						const uint bytesPerLine = img.bytesPerLine();
						for (int i = 0; i < height; ++i) {
							memcpy(q, p, bytesPerLine);
							q += width;
							p -= width;
						}
					}
                }
            } else {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    for (int i=0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = (*p << 8) | ((*p >> 24) & 0xff);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                } else {
                    for (int i=0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                }
            }
        }
    }

    QImage convertToGLFormat(const QImage& img, GLenum pixel_format, GLint internal_format)
    {
        QImage res(img.size(), QImage::Format_ARGB32);
		// transform given image into ARGB32 (32-bits with 8 bits alpha channel)
        convertToGLFormatHelper(res, img.convertToFormat(QImage::Format_ARGB32), GL_BGRA, internal_format);
        return res;
    }
}

bool osgImageToQImage(const osg::Image * image, QImage * qimage)
{
    bool ret = false;
    QImage::Format format;
	GLenum pixel_format = image->getPixelFormat();
	GLint internal_format = image->getInternalTextureFormat();
	bool run_convert = true;
    switch(pixel_format)
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
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		format = QImage::Format_Invalid;
		*qimage = decodeDDSImage(image);
		run_convert = false;
		ret = true;
		break;
	default:
        format = QImage::Format_Invalid;
        break;
    }

    if(format != QImage::Format_Invalid && run_convert)
    {
        int bytesPerLine = image->getRowSizeInBytes();
        int width = image->s();
        int height = image->t();
        *qimage = QImage(image->data(), width, height, bytesPerLine, format);
        *qimage = convertToGLFormat(*qimage, pixel_format, internal_format);
        //ret = ret.mirrored(false, true);
        ret = true;
    }
    return ret;
}

const sgi::Image * convertImage(const osg::Image * image)
{
    if (!image)
        return NULL;

    sgi::Image * ret = NULL;
    sgi::Image::ImageFormat imageFormat;
    switch(image->getPixelFormat())
    {
    case GL_RGB: imageFormat = sgi::Image::ImageFormatRGB24; break;
    case GL_RGBA:imageFormat = sgi::Image::ImageFormatARGB32; break;
    case GL_LUMINANCE: imageFormat = sgi::Image::ImageFormatMono; break;
    default: imageFormat = sgi::Image::ImageFormatInvalid; break;
    }
    sgi::Image::Origin origin = (image->getOrigin() == osg::Image::TOP_LEFT) ? sgi::Image::OriginTopLeft : sgi::Image::OriginBottomLeft;
    if(imageFormat != sgi::Image::ImageFormatInvalid)
    {
        ret = new sgi::Image(imageFormat, origin,
                            image->data(), image->getTotalDataSize(),
                            image->s(), image->t(), image->r(), image->getRowStepInBytes(),
                            image);
    }
    else
    {
        QImage * qimage = new QImage;
        osgImageToQImage(image, qimage);
        ret = new sgi::Image(imageFormat, origin,
            image->data(), image->getTotalDataSize(),
            image->s(), image->t(), image->r(), image->getRowStepInBytes(),
            qimage);
    }
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

} // namespace osg_helpers
} // namespace sgi
