// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth


#include <osg/Image>

#if defined(_WIN32) && defined(OSG_GL3_AVAILABLE)
#define __GL_H__
#endif
#include <QImage>
#include <QGLWidget>

#include <sgi/plugins/SGIItemBase.h>
#include <sgi/plugins/SGIImage.h>
#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/osg>

extern QImage decodeDDSImage(const osg::Image * image);

namespace sgi {

    namespace osg_helpers {

namespace {

    static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
    {
        if (texture_format == GL_BGRA) {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return ((src_pixel << 24) & 0xff000000)
                    | ((src_pixel >> 24) & 0x000000ff)
                    | ((src_pixel << 8) & 0x00ff0000)
                    | ((src_pixel >> 8) & 0x0000ff00);
            }
            else {
                return src_pixel;
            }
        }
        else {  // GL_RGBA
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
            }
            else {
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

            quint32 *dest = (quint32 *)dst.scanLine(0); // NB! avoid detach here
            uchar *srcPixels = (uchar *)img.scanLine(img.height() - 1);
            int sbpl = img.bytesPerLine();
            int dbpl = dst.bytesPerLine();

            int ix = int(0x00010000 / sx);
            int iy = int(0x00010000 / sy);

            quint32 basex = int(0.5 * ix);
            quint32 srcy = int(0.5 * iy);

            // scale, swizzle and mirror in one loop
            while (target_height--) {
                const uint *src = (const quint32 *)(srcPixels - (srcy >> 16) * sbpl);
                int srcx = basex;
                for (int x = 0; x < target_width; ++x) {
                    dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                    srcx += ix;
                }
                dest = (quint32 *)(((uchar *)dest) + dbpl);
                srcy += iy;
            }
        }
        else {
            const int width = img.width();
            const int height = img.height();
            const uint *p = (const uint*)img.scanLine(img.height() - 1);
            uint *q = (uint*)dst.scanLine(0);

            if (texture_format == GL_BGRA) {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    // mirror + swizzle
                    for (int i = 0; i < height; ++i) {
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
                }
                else {
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
            }
            else {
                if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                    for (int i = 0; i < height; ++i) {
                        const uint *end = p + width;
                        while (p < end) {
                            *q = (*p << 8) | ((*p >> 24) & 0xff);
                            p++;
                            q++;
                        }
                        p -= 2 * width;
                    }
                }
                else {
                    for (int i = 0; i < height; ++i) {
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
    switch (pixel_format)
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

    if (format != QImage::Format_Invalid && run_convert)
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


} // namespace osg_helpers
} // namespace sgi
