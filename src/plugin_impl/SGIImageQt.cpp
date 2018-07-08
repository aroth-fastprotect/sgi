// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

// tricking the SGI_IMPL_EXPORT defined to switch to export
#define SGI_IMPL_LIBRARY
#include <sgi/plugins/SGIImage.h>
#include <sgi/helpers/qt>
#include <sstream>
#include <cmath>
#include <QImage>

#define ALIGN_SIZE(x,n)       ((size_t)((~(n-1))&((x)+(n-1))))
#define ALIGN_BY_16(x) ALIGN_SIZE(x, 16)
#define ALIGN_BY_8(x) ALIGN_SIZE(x, 8)
#define ALIGN_BY_4(x) ALIGN_SIZE(x, 4)

namespace sgi {


Image::Image(ImageFormat format, DataType type, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    QImage * originalImage, bool copyData)
    : _format(format), _dataType(type), _origin(origin), _data(copyData ? malloc(length) : data), _length(length)
    , _width(width), _height(height), _depth(depth)
    , _allocatedWidth(width), _allocatedHeight(height)
    , _pitch{ bytesPerLine, 0, 0, 0 }, _lines{ height, 0, 0, 0 }
    , _planeOffset { 0, 0, 0, 0 }
    , _originalImage(nullptr), _originalImageQt((originalImage)?new QImage(*originalImage):nullptr)
    , _freeQt(&Image::freeQt)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    loadPitchAndPlaneOffsets();
}

namespace {
    Image::ImageFormat imageFormatFromQImage(QImage::Format format)
    {
        Image::ImageFormat imageFormat;
        switch (format)
        {
        case QImage::Format_Invalid:imageFormat = Image::ImageFormatInvalid; break;
        case QImage::Format_Mono: imageFormat = Image::ImageFormatMono; break;
        case QImage::Format_MonoLSB: imageFormat = Image::ImageFormatMonoLSB; break;
        case QImage::Format_Indexed8: imageFormat = Image::ImageFormatIndexed8; break;
        case QImage::Format_Alpha8: imageFormat = Image::ImageFormatAlpha; break;
        case QImage::Format_Grayscale8: imageFormat = Image::ImageFormatGray; break;
        case QImage::Format_RGB32: imageFormat = Image::ImageFormatRGB32; break;
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBA8888_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32: imageFormat = Image::ImageFormatARGB32; break;
        case QImage::Format_RGB888: imageFormat = Image::ImageFormatRGB24; break;
        case QImage::Format_RGBX8888: imageFormat = Image::ImageFormatRGB32; break;
        case QImage::Format_RGB16:
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_RGB666:
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_RGB555:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_RGB444:
        case QImage::Format_ARGB4444_Premultiplied:
        case QImage::Format_BGR30:
        case QImage::Format_A2BGR30_Premultiplied:
        case QImage::Format_RGB30:
        case QImage::Format_A2RGB30_Premultiplied:
        default:
            imageFormat = Image::ImageFormatRaw;
            break;
        }
        return imageFormat;
    }
    unsigned bytesPerPixelFromQImage(QImage::Format format)
    {
        unsigned ret = 0;
        switch (format)
        {
        case QImage::Format_Invalid: ret = 0; break;
        case QImage::Format_Mono:
        case QImage::Format_MonoLSB:
        case QImage::Format_Indexed8:
        case QImage::Format_Alpha8:
        case QImage::Format_Grayscale8:
            ret = 1;
            break;
        case QImage::Format_RGB16: 
            ret = 2; 
            break;
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_RGB444:
        case QImage::Format_RGB555:
        case QImage::Format_RGB666: ret = 3;
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_ARGB4444_Premultiplied:
        case QImage::Format_BGR30:
        case QImage::Format_A2BGR30_Premultiplied:
        case QImage::Format_RGB30:
        case QImage::Format_A2RGB30_Premultiplied:
        case QImage::Format_RGB32:
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBA8888_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32:
        case QImage::Format_RGB888:
        case QImage::Format_RGBX8888:
            ret = 4;
            break;
        default:
            ret = 1;
            break;
        }
        return ret;
    }
}

Image::Image(QImage * originalImage, bool copyData)
    : _format(imageFormatFromQImage(originalImage->format()))
    , _dataType(DataTypeUnsignedByte)
    , _origin(OriginTopLeft), _data(nullptr), _length(originalImage->byteCount())
    , _width(originalImage->width()), _height(originalImage->height()), _depth(1)
    , _allocatedWidth(originalImage->bytesPerLine() / bytesPerPixelFromQImage(originalImage->format())), _allocatedHeight(originalImage->height())
    , _pitch { (unsigned)originalImage->bytesPerLine(), 0, 0, 0 }
    , _lines{ (unsigned)originalImage->height(), 0, 0, 0 }
    , _planeOffset{0, 0, 0, 0}
    , _originalImage(nullptr), _originalImageQt(new QImage(*originalImage))
    , _freeQt(&Image::freeQt), _copyQt(&Image::copyQt)
    , _allocated(false)
{
    if (copyData)
    {
        _data = malloc(_length);
        memcpy(_data, _originalImageQt->bits(), _length);
    }
    else
        _data = _originalImageQt->bits();
}

void Image::freeQt()
{
    if (_originalImageQt)
    {
        delete _originalImageQt;
        _originalImageQt = nullptr;
    }
}

QImage * Image::copyQt() const
{
    if (!_originalImageQt)
        return nullptr;
    return new QImage(*_originalImageQt);
}

namespace qt_helpers {

bool convertImageToQImage_RGB24(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB888);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatRGB24:
        memcpy(dest, src, image->width() * image->height() * 3);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 3;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            src++;
            dest += 3;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_BGR24(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB888);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatBGR24:
        memcpy(dest, src, image->width() * image->height() * 3);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 4;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            src += 3;
            dest += 3;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            src++;
            dest += 3;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_RGB32(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB32);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatRGB32:
        memcpy(dest, src, image->width() * image->height() * 4);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src++;
            dest += 4;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_BGR32(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_RGB32);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatBGR32:
        memcpy(dest, src, image->width() * image->height() * 4);
        ret = true;
        break;
    case Image::ImageFormatARGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src++;
            dest += 4;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage_RGBA32(const sgi::Image * image, QImage & qimage)
{
    bool ret = false;
    qimage = QImage(image->width(), image->height(), QImage::Format_ARGB32);
    uchar * dest = qimage.bits();
    uchar * src = (uchar *)const_cast<void*>(image->data());
    unsigned pixels = image->width() * image->height();
    switch (image->format())
    {
    case Image::ImageFormatARGB32:
        memcpy(dest, src, image->width() * image->height() * 4);
        ret = true;
        break;
    case Image::ImageFormatRGBA32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = src[3];
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatRGB32:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[2];
            dest[1] = src[1];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src += 4;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatBGR24:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[1];
            dest[2] = src[2];
            dest[3] = 0xFF;
            src += 3;
            dest += 4;
        }
        ret = true;
    }
    break;
    case Image::ImageFormatLuminance:
    {
        for (unsigned n = 0; n < pixels; ++n)
        {
            dest[0] = src[0];
            dest[1] = src[0];
            dest[2] = src[0];
            dest[3] = 0xFF;
            src++;
            dest += 4;
        }
        ret = true;
    }
    break;
    }
    return ret;
}

bool convertImageToQImage(const sgi::Image * image, sgi::Image::ImageFormat destFormat, QImage & qimage)
{
    bool ret = false;
    if(!image)
        return false;
    if (destFormat == Image::ImageFormatOriginal)
        destFormat = image->format();
    switch (destFormat)
    {
    case Image::ImageFormatInvalid:
        // invalid -> return empty QImage
        qimage = QImage();
        break;
    case Image::ImageFormatRGB24:
        ret = convertImageToQImage_RGB24(image, qimage);
        if (image->origin() == Image::OriginBottomLeft)
            qimage = qimage.mirrored(false, true);
        break;
    case Image::ImageFormatBGR24:
        ret = convertImageToQImage_BGR24(image, qimage);
        if (image->origin() == Image::OriginBottomLeft)
            qimage = qimage.mirrored(false, true);
        break;
    case Image::ImageFormatRGB32:
        ret = convertImageToQImage_RGB32(image, qimage);
        if (image->origin() == Image::OriginBottomLeft)
            qimage = qimage.mirrored(false, true);
        break;
    case Image::ImageFormatBGR32:
        ret = convertImageToQImage_BGR32(image, qimage);
        if (image->origin() == Image::OriginBottomLeft)
            qimage = qimage.mirrored(false, true);
        break;
    case Image::ImageFormatAutomatic:
        if (image->originalImageQt())
        {
            qimage = *image->originalImageQt();
            ret = true;
        }
        else
        {
            QImage::Format qt_format = QImage::Format_Invalid;
            switch (image->format())
            {
            case Image::ImageFormatRGB24: qt_format = QImage::Format_RGB888; break;
            case Image::ImageFormatRGB32: qt_format = QImage::Format_RGB32; break;
            case Image::ImageFormatARGB32: qt_format = QImage::Format_ARGB32; break;
            case Image::ImageFormatMono: qt_format = QImage::Format_Mono; break;
            case Image::ImageFormatMonoLSB: qt_format = QImage::Format_MonoLSB; break;
            case Image::ImageFormatIndexed8: qt_format = QImage::Format_Indexed8; break;
            case Image::ImageFormatFloat:
            case Image::ImageFormatInvalid:
            default: qt_format = QImage::Format_Invalid; break;
            }
            if (qt_format != QImage::Format_Invalid)
            {
                qimage = QImage((uchar*)image->data(), (int)image->width(), (int)image->height(), (int)image->pitch(0), qt_format);
                if (image->origin() == Image::OriginBottomLeft)
                    qimage = qimage.mirrored(false, true);
                ret = true;
            }
            else
            {
                ret = convertImageToQImage_RGB32(image, qimage);
            }
        }
        break;
    }
    return ret;
}

} // namespace qt_helpers


} // namespace sgi
