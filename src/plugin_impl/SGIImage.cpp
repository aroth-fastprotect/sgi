// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIImage.h>
#include <sstream>
#include <QImage>

namespace sgi {

Image::Image(ImageFormat format)
    : _format(format), _origin(OriginDefault), _data(NULL), _length(0)
    , _width(0), _height(0), _depth(0), _pitch { 0, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt(NULL)
    , _allocated(false)
{
}

Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
        unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
        const osg::Referenced * originalImage, bool copyData)
    : _format(format), _origin(origin), _data(copyData ? malloc(length) : data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(originalImage), _originalImageQt(NULL)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    loadPitchAndPlaneOffsets();
}

Image::Image(ImageFormat format, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    QImage * originalImage, bool copyData)
    : _format(format), _origin(origin), _data(copyData ? malloc(length) : data), _length(length)
    , _width(width), _height(height), _depth(depth), _pitch { bytesPerLine, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage)?new QImage(*originalImage):NULL)
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
    , _origin(OriginTopLeft), _data(NULL), _length(0)
    , _width(originalImage->width()), _height(originalImage->height()), _depth(1), _pitch { (unsigned)originalImage->bytesPerLine(), 0, 0, 0 }
    , _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt((originalImage) ? new QImage(*originalImage) : NULL)
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

Image::Image(ImageFormat format, void * data, size_t length, bool copyData)
    : _format(format), _origin(OriginDefault), _data(copyData ? malloc(length) : data), _length(length)
    , _width(0), _height(0), _depth(0), _pitch{ 0, 0, 0, 0 }, _planeOffset{ 0, 0, 0, 0 }
    , _originalImage(NULL), _originalImageQt(NULL)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    loadPitchAndPlaneOffsets();
}

Image::Image(const Image & rhs)
    : _format(rhs._format), _origin(rhs._origin), _data(rhs._data), _length(rhs._length)
    , _width(rhs._width), _height(rhs._height), _depth(rhs._depth)
    , _pitch { rhs._pitch[0], rhs._pitch[1], rhs._pitch[2], rhs._pitch[3] }
    , _planeOffset { rhs._planeOffset[0], rhs._planeOffset[1], rhs._planeOffset[2], rhs._planeOffset[3] }
    , _originalImage(rhs._originalImage), _originalImageQt(rhs._originalImageQt)
    , _allocated(false)
{

}

Image::~Image()
{
    free();
}

Image & Image::operator=(const Image & rhs)
{
    free();
    _format = rhs._format;
    _origin = rhs._origin;
    _data = rhs._data;
    _allocated = false;
    _length = rhs._length;
    _width = rhs._width;
    _height = rhs._height;
    _depth = rhs._depth;
    _pitch[0] = rhs._pitch[0];
    _pitch[1] = rhs._pitch[1];
    _pitch[2] = rhs._pitch[2];
    _pitch[3] = rhs._pitch[3];
    _planeOffset[0] = rhs._planeOffset[0];
    _planeOffset[1] = rhs._planeOffset[1];
    _planeOffset[2] = rhs._planeOffset[2];
    _planeOffset[3] = rhs._planeOffset[3];
    _originalImage = rhs._originalImage;
    _originalImageQt = rhs._originalImageQt;
    return *this;
}

void Image::loadPitchAndPlaneOffsets()
{
    switch (_format)
    {
    default:
    case ImageFormatInvalid:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format");
        break;
    case ImageFormatAutomatic:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, automatic");
        break;
    case ImageFormatRaw:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, raw");
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
    {
        _pitch[0] = _width * 3;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatRGB32:
    case ImageFormatARGB32:
    case ImageFormatBGR32:
    case ImageFormatABGR32:
    case ImageFormatFloat:
    {
        _pitch[0] = _width * 4;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatYUV444:
    {
        _pitch[0] = _pitch[1] = _pitch[2] = _width * 3;
        _pitch[3] = 0;
        _planeOffset[0] = 0;
        _planeOffset[1] = _width * _height * 3;
        _planeOffset[2] = _planeOffset[1] << 1;
        _planeOffset[3] = 0;
    }
    break;
    case ImageFormatYUV422:
    {
        _pitch[0] = _width;
        _pitch[1] = _pitch[2] = _width / 2;
        _pitch[3] = 0;
        _planeOffset[0] = 0;
        _planeOffset[1] = _width * _height;
        _planeOffset[2] = _planeOffset[1] + (_planeOffset[1] >> 1);
        _planeOffset[3] = 0;
    }
    break;
    case ImageFormatYUV420:
    {
        _pitch[0] = _width;
        _pitch[1] = _pitch[2] = _width / 2;
        _pitch[3] = 0;
        _planeOffset[0] = 0;
        _planeOffset[1] = _width * _height;
        _planeOffset[2] = _planeOffset[1] + (_planeOffset[1] >> 2);
        _planeOffset[3] = 0;
    }
    break;

    case ImageFormatYUYV:
    case ImageFormatUYVY:
    {
        _pitch[0] = _width;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
    {
        // only one channel with 8-bit color data
        _pitch[0] = _width;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    }
}

void Image::free()
{
    if (_originalImageQt)
    {
        delete _originalImageQt;
        _originalImageQt = NULL;
    }
    if(_allocated && _data)
    {
        ::free(_data);
        _data = NULL;
    }
}


bool Image::allocate(unsigned width, unsigned height, ImageFormat format)
{
    bool ret = false;
    free();
    _length = 0;
    switch (format)
    {
    default:
    case ImageFormatInvalid:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format");
        break;
    case ImageFormatAutomatic:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, automatic");
        break;
    case ImageFormatRaw:
        Q_ASSERT_X(false, __FUNCTION__, "invalid frame format, raw");
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
        _length = width * height * 3;
        break;
    case ImageFormatRGB32:
    case ImageFormatARGB32:
    case ImageFormatBGR32:
    case ImageFormatABGR32:
    case ImageFormatFloat:
        _length = width * height * 4;
        break;
    case ImageFormatYUV444:
        _length = width * height * 3;
        break;
    case ImageFormatYUV422:
        _length = width * height * 2;
        break;
    case ImageFormatYUV420:
        _length = width * height + (width / 2 * height/2);
        break;

    case ImageFormatYUYV:
    case ImageFormatUYVY:
        _length = width * height * 2;
        break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
        // only one channel with 8-bit color data
        _length = width * height;
        break;
    }
    if(_length)
    {
        _data = malloc(_length);
        _width = width;
        _height = height;
        _format = format;
        loadPitchAndPlaneOffsets();
        ret = _data != NULL;
        _allocated = ret;
    }
    return ret;
}

bool Image::reinterpretFormat(ImageFormat targetFormat)
{
    bool ret = false;
    switch(_format)
    {
    case Image::ImageFormatRed:
    case Image::ImageFormatGreen:
    case Image::ImageFormatBlue:
    case Image::ImageFormatAlpha:
    case Image::ImageFormatGray:
        {
            // reinterpret a single channel image as a full color image
            switch(targetFormat)
            {
            case Image::ImageFormatYUV444:
                ret = (_height % 3) == 0;
                if(ret)
                {
                    unsigned planeHeight = _height / 3;
                    unsigned planeLength = planeHeight * _width;
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = planeLength;
                    _planeOffset[2] = planeLength + planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width;
                    _pitch[2] = _width;
                    _pitch[3] = 0;
                    _height = planeHeight;
                }
                break;
            case Image::ImageFormatYUV422:
                ret = (_height % 2) == 0;
                if(ret)
                {
                    unsigned luma_planeHeight = _height / 2;
                    unsigned chroma_planeHeight = luma_planeHeight / 2;
                    unsigned luma_planeLength = luma_planeHeight * _width;
                    unsigned chroma_planeLength = chroma_planeHeight * _width;
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = luma_planeLength;
                    _planeOffset[2] = luma_planeLength + chroma_planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width;
                    _pitch[2] = _width;
                    _pitch[3] = 0;
                    _height = luma_planeHeight;
                }
                break;
            case Image::ImageFormatYUV420:
                ret = ((_height+_height) % 3) == 0;
                if(ret)
                {
                    unsigned luma_planeHeight = (_height + _height)/ 3;
                    unsigned chroma_planeHeight = luma_planeHeight / 2;
                    unsigned luma_planeLength = luma_planeHeight * _width;
                    unsigned chroma_planeLength = chroma_planeHeight * (_width/2);
                    // split single plane into three separate ones
                    _planeOffset[0] = 0;
                    _planeOffset[1] = luma_planeLength;
                    _planeOffset[2] = luma_planeLength + chroma_planeLength;
                    _planeOffset[3] = 0;
                    _pitch[0] = _width;
                    _pitch[1] = _width / 2;
                    _pitch[2] = _width / 2;
                    _pitch[3] = 0;
                    _height = luma_planeHeight;
                }
                break;
            case Image::ImageFormatYUYV:
            case Image::ImageFormatUYVY:
                {
                    // it's a YUV422 color format all in one plane
                    ret = (_height % 2) == 0;
                    if(ret)
                    {
                        unsigned luma_planeHeight = _height / 2;
                        // split single plane into three separate ones
                        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
                        _pitch[0] = _width;
                        _pitch[1] = _pitch[2] = _pitch[3] = 0;
                        _height = luma_planeHeight;
                    }
                }
                break;
            }
        }
        break;
    case ImageFormatBGR32:
    case ImageFormatRGB32:
    case ImageFormatARGB32:
    case ImageFormatABGR32:
        // reinterpret a 32-bit image and reinterpret the colors (helpful for wrong color
        // display)
        switch(targetFormat)
        {
        case ImageFormatBGR32:
        case ImageFormatRGB32:
        case ImageFormatARGB32:
        case ImageFormatABGR32:
            // just accept this, no special handling required
            ret = true;
        }
        break;
    case ImageFormatBGR24:
    case ImageFormatRGB24:
    case ImageFormatYUV444:
        // reinterpret a 24-bit image and reinterpret the colors (helpful for wrong color
        // display)
        switch(targetFormat)
        {
        case ImageFormatBGR24:
        case ImageFormatRGB24:
        case ImageFormatYUV444:
            // just accept this, no special handling required
            ret = true;
        }
        break;
    }
    if(ret)
    {
        // we got the desired format, so take the targetFormat as new format
        _format = targetFormat;
    }
    return ret;
}

bool Image::reinterpret(const ImageSize & size)
{
    return reinterpret(size.format, size.width, size.height, size.depth);
}

bool Image::reinterpret(ImageFormat format, unsigned width, unsigned height, unsigned depth)
{
    _width = width;
    _height = height;
    _depth = depth;
    _format = format;
    loadPitchAndPlaneOffsets();
    return true;
}

bool Image::guessImageSizes(ImageSizeList & possibleSizes) const
{
    bool ret = false;
    possibleSizes.clear();
    quint64 totalNumberOfPixels = 0;
    switch(_format)
    {
    case Image::ImageFormatARGB32:
    case Image::ImageFormatRGB32:
    case Image::ImageFormatABGR32:
    case Image::ImageFormatBGR32:
    case Image::ImageFormatFloat:
        ret = (_length % 4 == 0);
        if(ret)
            totalNumberOfPixels = _length / 4;
        break;
    case Image::ImageFormatRGB24:
    case Image::ImageFormatBGR24:
    case Image::ImageFormatYUV444:
        ret = (_length % 3 == 0);
        if(ret)
            totalNumberOfPixels = _length / 3;
        break;
    case Image::ImageFormatYUV422:
    case Image::ImageFormatYUYV:
    case Image::ImageFormatUYVY:
        ret = (_length % 2 == 0);
        if(ret)
            totalNumberOfPixels = _length / 2;
        break;
    case Image::ImageFormatYUV420:
        ret = ((_length*2) % 3 == 0);
        if(ret)
            totalNumberOfPixels = (_length*2) / 3;
    }
    if(ret)
    {
        double d = sqrt(totalNumberOfPixels);
        if(fmod(d, 1.0) == 0)
        {
            // got a square image
            int width = std::floor(d);
            int height = width;
            possibleSizes.push_back(ImageSize(_format, width, height));
        }
        else
        {
            const int size_step_size = 8;
            const int min_height = 16;
            const int min_width = 16;
            int max_width = totalNumberOfPixels / min_height;
            int start_width = (int)std::floor(d)/size_step_size * size_step_size;
            // adjust to next matching 8-pixel width
            start_width = (start_width / size_step_size) * size_step_size;

            for(int width = start_width; width <= max_width; width += size_step_size)
            {
                int height_4_3 = (width * 3) / 4;
                int height_16_9 = (width * 9) / 16;
                int total_4_3 = width * height_4_3;
                int total_16_9 = width * height_16_9;
                if(total_4_3 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_4_3));
                }
                if(total_16_9 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_16_9));
                }
            }
            for(int width = start_width; width >= min_width; width -= size_step_size)
            {
                int height_4_3 = (width * 3) / 4;
                int height_16_9 = (width * 9) / 16;
                int total_4_3 = width * height_4_3;
                int total_16_9 = width * height_16_9;
                if(total_4_3 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_4_3));
                }
                if(total_16_9 == totalNumberOfPixels)
                {
                    possibleSizes.push_back(ImageSize(_format, width, height_16_9));
                }
            }

        }
    }
    return ret;
}

std::string Image::imageFormatToString(ImageFormat format)
{
    std::string ret;
    switch(format)
    {
    case ImageFormatInvalid: ret = "Invalid"; break;
    case ImageFormatRGB24: ret = "RGB24"; break;
    case ImageFormatRGB32: ret = "RGB32"; break;
    case ImageFormatARGB32: ret = "ARGB32"; break;
    case ImageFormatMono: ret = "Mono"; break;
    case ImageFormatMonoLSB: ret = "MonoLSB"; break;
    case ImageFormatIndexed8: ret = "Indexed8"; break;
    case ImageFormatFloat: ret = "Float"; break;
    case ImageFormatBGR24: ret = "BGR24"; break;
    case ImageFormatBGR32: ret = "BGR32"; break;
    case ImageFormatABGR32: ret = "ABGR32"; break;
    case ImageFormatYUV420: ret = "YUV420"; break;
    case ImageFormatYUV422: ret = "YUV422"; break;
    case ImageFormatYUV444: ret = "YUV444"; break;
    case ImageFormatYUYV: ret = "YUYV"; break;
    case ImageFormatUYVY: ret = "UYVY"; break;
    case ImageFormatGray: ret = "Gray"; break;
    case ImageFormatRed: ret = "Red"; break;
    case ImageFormatGreen: ret = "Green"; break;
    case ImageFormatBlue: ret = "Blue"; break;
    case ImageFormatAlpha: ret = "Alpha"; break;
    case ImageFormatDepth: ret = "Depth"; break;
    case ImageFormatLuminance: ret = "Luminance"; break;
    case ImageFormatLuminanceAlpha: ret = "LuminanceAlpha"; break;
    case ImageFormatDXT1: ret = "DXT1"; break;
    case ImageFormatDXT1Alpha: ret = "DXT1a"; break;
    case ImageFormatDXT3: ret = "DXT3"; break;
    case ImageFormatDXT5: ret = "DXT5"; break;
    case ImageFormatRaw: ret = "raw"; break;
    default:
        {
            std::stringstream ss;
            ss << "Unknown(" << (int)format << ')';
            ret = ss.str();
        }
        break;
    }
    return ret;
}

std::string Image::colorSpaceToString(ColorSpace colorspace)
{
    std::string ret;
    switch (colorspace)
    {
    case ColorSpaceInvalid: ret = "invalid"; break;
    case ColorSpaceAutomatic: ret = "invalid"; break;
    case ColorSpaceRGB: ret = "RGB"; break;
    case ColorSpaceYUV_ITU_R_BT_601: ret = "YUV ITU-R BT.601"; break;
    case ColorSpaceYUV_ITU_R_BT_709: ret = "YUV ITU-R BT.709"; break;
    case ColorSpaceYUV_ITU_R_BT_2020: ret = "YUV ITU-R BT.2020"; break;
    case ColorSpaceCYMK: ret = "invalid"; break;
    default:
    {
        std::stringstream ss;
        ss << "Unknown(" << (int)colorspace << ')';
        ret = ss.str();
    }
    break;
    }
    return ret;
}

std::string Image::originToString(Origin o)
{
    std::string ret;
    switch(o)
    {
    case OriginBottomLeft: ret = "bottom-left"; break;
    case OriginTopLeft: ret = "top-left"; break;
    default:
        {
            std::stringstream ss;
            ss << "Unknown(" << (int)o << ')';
            ret = ss.str();
        }
        break;
    }
    return ret;
}

} // namespace sgi
