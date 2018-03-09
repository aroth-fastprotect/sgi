// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

// tricking the SGI_IMPL_EXPORT defined to switch to export
#define SGI_IMPL_LIBRARY
#include <sgi/plugins/SGIImage.h>
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
    , _originalImage(NULL), _originalImageQt((originalImage)?new QImage(*originalImage):NULL)
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
        case QImage::Format_RGB32: imageFormat = Image::ImageFormatRGB32; break;
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBA8888_Premultiplied:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_ARGB32: imageFormat = Image::ImageFormatARGB32; break;
        case QImage::Format_RGB888: imageFormat = Image::ImageFormatRGB24; break;
        case QImage::Format_RGB16:
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_RGB666:
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_RGB555:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_RGB444:
        case QImage::Format_ARGB4444_Premultiplied:
        case QImage::Format_RGBX8888:
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
}

Image::Image(QImage * originalImage, bool copyData)
    : _format(imageFormatFromQImage(originalImage->format()))
    , _dataType(DataTypeUnsignedByte)
    , _origin(OriginTopLeft), _data(NULL), _length(0)
    , _width(originalImage->width()), _height(originalImage->height()), _depth(1)
    , _allocatedWidth(originalImage->width()), _allocatedHeight(originalImage->height())
    , _pitch { (unsigned)originalImage->bytesPerLine(), 0, 0, 0 }
    , _lines{ (unsigned)originalImage->height(), 0, 0, 0 }
    , _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage) ? new QImage(*originalImage) : NULL)
    , _freeQt(&Image::freeQt), _copyQt(&Image::copyQt)
    , _allocated(false)
{
    _length = _originalImageQt->byteCount();
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
        _originalImageQt = NULL;
    }
}

QImage * Image::copyQt() const
{
    if (!_originalImageQt)
        return nullptr;
    return new QImage(*_originalImageQt);
}

} // namespace sgi
