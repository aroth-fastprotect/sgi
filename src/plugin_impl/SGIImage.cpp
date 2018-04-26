// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIImage.h>
#include <sstream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <memory.h>

#define ALIGN_SIZE(x,n)       ((size_t)((~(n-1))&((x)+(n-1))))
#define ALIGN_BY_16(x) ALIGN_SIZE(x, 16)
#define ALIGN_BY_8(x) ALIGN_SIZE(x, 8)
#define ALIGN_BY_4(x) ALIGN_SIZE(x, 4)

#define SGI_UNUSED(x) (void)x;

namespace sgi {

Image::Pixel::Pixel()
    : type(DataTypeInvalid), elements(0)
{
}

Image::Pixel::Pixel(DataType t, const PixelData & d)
    : type(t), data(d), elements(0)
{
}

Image::Pixel::Pixel(DataType t, float a, float r, float g, float b)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeFloat32:
        data.f32[0] = a;
        data.f32[1] = r;
        data.f32[2] = g;
        data.f32[3] = b;
        elements = 4;
        break;
    }
}

Image::Pixel::Pixel(DataType t, float f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeFloat32:
        data.f32[0] = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, double f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeFloat64:
        data.f64[0] = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, unsigned char f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeUnsignedByte:
        data.u8 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, unsigned char a_, unsigned char r_, unsigned char g_, unsigned char b_)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeUnsignedByte:
        data.u8 = a_;
        elements = 1;
        break;
    case Image::DataTypeARGB:
        data.argb.a = a_;
        data.argb.r = r_;
        data.argb.g = g_;
        data.argb.b = b_;
        elements = 4;
        break;
    }
}

Image::Pixel::Pixel(DataType t, signed char f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeSignedByte:
        data.s8 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, unsigned short f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeUnsignedShort:
        data.u16 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, signed short f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeSignedShort:
        data.s16 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, unsigned int f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeUnsignedInt:
        data.u32 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, signed int f)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeSignedInt:
        data.s32 = f;
        elements = 1;
        break;
    }
}

Image::Pixel::Pixel(DataType t, const RGB & rgb)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeARGB:
        data.argb.a = 255;
        data.argb.r = rgb.r;
        data.argb.g = rgb.g;
        data.argb.b = rgb.b;
        elements = 3;
        break;
    }
}

Image::Pixel::Pixel(DataType t, const ARGB & argb)
    : type(t), elements(0)
{
    switch(type)
    {
    case Image::DataTypeARGB:
        data.argb = argb;
        break;
    }
}


void Image::Pixel::clear()
{
    type = DataTypeInvalid;
}

void Image::Pixel::setARGB(const ARGB & rhs)
{
    type = DataTypeARGB;
    data.argb = rhs;
    elements = 4;
}

void Image::Pixel::setARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
    type = DataTypeARGB;
    data.argb.a = a;
    data.argb.r = r;
    data.argb.g = g;
    data.argb.b = b;
    elements = 4;
}

void Image::Pixel::setFloat32(float a, float r, float g, float b)
{
    type = DataTypeFloat32;
    data.f32[0] = a;
    data.f32[1] = r;
    data.f32[2] = g;
    data.f32[3] = b;
    elements = 4;
}

void Image::Pixel::setFloat64(double f, double f2)
{
    type = DataTypeFloat64;
    data.f64[0] = f;
    data.f64[1] = f2;
    elements = 2;
}

float Image::Pixel::a() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = (float)data.argb.a / 255.0f;
        break;
    case DataTypeFloat32:
        ret = data.f32[0];
        break;
    }
    return ret;
}

float Image::Pixel::r() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = (float)data.argb.r / 255.0f;
        break;
    case DataTypeFloat32:
        ret = data.f32[1];
        break;
    }
    return ret;
}

float Image::Pixel::g() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = (float)data.argb.g / 255.0f;
        break;
    case DataTypeFloat32:
        ret = data.f32[2];
        break;
    }
    return ret;
}

float Image::Pixel::b() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = (float)data.argb.b / 255.0f;
        break;
    case DataTypeFloat32:
        ret = data.f32[3];
        break;
    }
    return ret;
}


int Image::Pixel::alpha() const
{
    int ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = static_cast<int>(data.argb.a);
        break;
    case DataTypeFloat32:
        ret = data.f32[0] * 255.0f;
        break;
    }
    return ret;
}

int Image::Pixel::red() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = static_cast<int>(data.argb.r);
        break;
    case DataTypeFloat32:
        ret = data.f32[1] * 255.0f;
        break;
    }
    return ret;
}

int Image::Pixel::green() const
{
    int ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = static_cast<int>(data.argb.g);
        break;
    case DataTypeFloat32:
        ret = data.f32[2] * 255.0f;
        break;
    }
    return ret;
}

int Image::Pixel::blue() const
{
    int ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = static_cast<int>(data.argb.b);
        break;
    case DataTypeFloat32:
        ret = data.f32[3] * 255.0f;
        break;
    }
    return ret;
}

int Image::Pixel::gray() const
{
    int ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        // See qGray()
        ret = (11 * data.argb.r + 16 * data.argb.g + 5 * data.argb.b) / 32;
        break;
    case DataTypeFloat32:
        ret = 0.2126f * data.f32[1] + 0.7152f * data.f32[2] + 0.0722f * data.f32[3];
        break;
    }
    return ret;
}

float Image::Pixel::lumaF() const
{
    float ret = 0;
    switch(type)
    {
    case DataTypeARGB:
        ret = 0.2126f * data.argb.r + 0.7152f * data.argb.g + 0.0722f * data.argb.b;
        break;
    case DataTypeFloat32:
        ret = 0.2126f * data.f32[1] + 0.7152f * data.f32[2] + 0.0722f * data.f32[3];
        break;
    }
    return ret;
}


Image::Pixel & Image::Pixel::operator *= (const double f)
{
    switch(type)
    {
    case DataTypeInvalid:
        break;
    case DataTypeARGB:
        data.argb.a *= f;
        data.argb.r *= f;
        data.argb.g *= f;
        data.argb.b *= f;
        break;
    case DataTypeFloat32:
        data.f32[0] *= f;
        data.f32[1] *= f;
        data.f32[2] *= f;
        data.f32[3] *= f;
        break;
    case DataTypeFloat64:
        data.f64[0] *= f;
        data.f64[1] *= f;
        break;
    }
    return *this;
}

Image::Pixel Image::Pixel::operator * (const double f) const
{
    Pixel ret = *this;
    ret *= f;
    return ret;
}

Image::Pixel Image::Pixel::operator+(const Pixel & rhs) const
{
    Pixel ret;
    switch(type)
    {
    case DataTypeInvalid:
        break;
    case DataTypeARGB:
        ret.data.argb.a = data.argb.a + rhs.data.argb.a;
        ret.data.argb.r = data.argb.r + rhs.data.argb.r;
        ret.data.argb.g = data.argb.g + rhs.data.argb.g;
        ret.data.argb.b = data.argb.b + rhs.data.argb.b;
        break;
    case DataTypeFloat32:
        ret.data.f32[0] = data.f32[0] + rhs.data.f32[0];
        ret.data.f32[1] = data.f32[1] + rhs.data.f32[1];
        ret.data.f32[2] = data.f32[2] + rhs.data.f32[2];
        ret.data.f32[3] = data.f32[3] + rhs.data.f32[3];
        break;
    case DataTypeFloat64:
        ret.data.f64[0] = data.f64[0] + rhs.data.f64[0];
        ret.data.f64[1] = data.f64[1] + rhs.data.f64[1];
    }
    return ret;
}

std::string Image::Pixel::toString(bool includeDataType) const
{
    std::stringstream ss;
    switch(type)
    {
    case DataTypeInvalid:
        ss << "invalid";
        break;
    case DataTypeUnsignedByte:
        if(includeDataType)
            ss << "u8:";
        ss << (unsigned)data.u8;
        break;
    case DataTypeSignedByte:
        if(includeDataType)
            ss << "s8:";
        ss << (int)data.s8;
        break;
    case DataTypeUnsignedShort:
        if(includeDataType)
            ss << "u16:";
        ss << (unsigned)data.u16;
        break;
    case DataTypeSignedShort:
        if(includeDataType)
            ss << "s16:";
        ss << (int)data.s16;
        break;
    case DataTypeUnsignedInt:
        if(includeDataType)
            ss << "u32:";
        ss << (unsigned)data.u32;
        break;
    case DataTypeSignedInt:
        if(includeDataType)
            ss << "s32:";
        ss << (int)data.s32;
        break;
    case DataTypeARGB:
        if(includeDataType)
            ss << "argb:";
        ss << "#";
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data.argb.r;
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data.argb.g;
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data.argb.b;
        if (data.argb.a < 255 && elements >= 4)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)data.argb.a;
        break;
    case DataTypeFloat32:
        if(includeDataType)
            ss << "f32:";
        ss << data.f32[0];
        if(elements > 1)
            ss << ',' << data.f32[1];
        if(elements > 2)
            ss << ',' << data.f32[2];
        if(elements > 3)
            ss << ',' << data.f32[3];
        break;
    case DataTypeFloat64:
        if(includeDataType)
            ss << "f64:";
        ss << data.f64[0];
        if(elements > 1)
            ss << ',' << data.f64[1];
        break;
    default:
        ss << "type=" << type;
        break;
    }
    return ss.str();
}


Image::Image(ImageFormat format, DataType type)
    : _format(format), _dataType(type), _origin(OriginDefault), _data(NULL), _length(0)
    , _width(0), _height(0), _depth(0)
    , _allocatedWidth(0), _allocatedHeight(0)
    , _pitch { 0, 0, 0, 0 }, _lines{ 0, 0, 0, 0 }, _planeOffset{0, 0, 0, 0}
    , _originalImage(NULL), _originalImageQt(NULL), _freeQt(NULL), _copyQt(NULL)
    , _allocated(false)
{
}

Image::Image(ImageFormat format, DataType type, Origin origin, void * data, size_t length,
        unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
        const osg::Referenced * originalImage, bool copyData)
    : _format(format), _dataType(type), _origin(origin), _data(copyData ? malloc(length) : data), _length(length)
    , _width(width), _height(height), _depth(depth)
    , _allocatedWidth(width), _allocatedHeight(height)
    , _pitch { bytesPerLine, 0, 0, 0 }, _lines{ height, 0, 0, 0 }
    , _planeOffset{0, 0, 0, 0}
    , _originalImage(originalImage), _originalImageQt(NULL), _freeQt(NULL), _copyQt(NULL)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    loadPitchAndPlaneOffsets();
}

Image::Image(ImageFormat format, DataType type, void * data, size_t length, bool copyData)
    : _format(format), _dataType(type), _origin(OriginDefault), _data(copyData ? malloc(length) : data), _length(length)
    , _width(0), _height(0), _depth(0)
    , _allocatedWidth(0), _allocatedHeight(0)
    , _pitch{ 0, 0, 0, 0 }, _lines{ 0, 0, 0, 0 }, _planeOffset{ 0, 0, 0, 0 }
    , _originalImage(NULL), _originalImageQt(NULL), _freeQt(NULL), _copyQt(NULL)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    loadPitchAndPlaneOffsets();
}

Image::Image(const Image & rhs)
    : _format(rhs._format), _dataType(rhs._dataType), _origin(rhs._origin), _data(rhs._data), _length(rhs._length)
    , _width(rhs._width), _height(rhs._height), _depth(rhs._depth)
    , _allocatedWidth(rhs._allocatedWidth), _allocatedHeight(rhs._allocatedHeight)
    , _pitch { rhs._pitch[0], rhs._pitch[1], rhs._pitch[2], rhs._pitch[3] }
    , _lines{ rhs._lines[0], rhs._lines[1], rhs._lines[2], rhs._lines[3] }
    , _planeOffset { rhs._planeOffset[0], rhs._planeOffset[1], rhs._planeOffset[2], rhs._planeOffset[3] }
    , _originalImage(rhs._originalImage)
    , _originalImageQt( (rhs._originalImageQt && rhs._copyQt) ? (rhs.*rhs._copyQt)() : NULL)
    , _freeQt(rhs._freeQt), _copyQt(rhs._copyQt)
    , _allocated(false)
{
    //(this->*_freeQt)();
}

Image::~Image()
{
    free();
}

bool Image::empty() const
{
    return _format == ImageFormatInvalid || (_originalImageQt == nullptr && _data == nullptr);
}

Image & Image::operator=(const Image & rhs)
{
    free();
    _format = rhs._format;
    _dataType = rhs._dataType;
    _origin = rhs._origin;
    _data = rhs._data;
    _allocated = false;
    _length = rhs._length;
    _width = rhs._width;
    _height = rhs._height;
    _depth = rhs._depth;
    _allocatedWidth = rhs._allocatedWidth;
    _allocatedHeight = rhs._allocatedHeight;
    _pitch[0] = rhs._pitch[0];
    _pitch[1] = rhs._pitch[1];
    _pitch[2] = rhs._pitch[2];
    _pitch[3] = rhs._pitch[3];
    _lines[0] = rhs._lines[0];
    _lines[1] = rhs._lines[1];
    _lines[2] = rhs._lines[2];
    _lines[3] = rhs._lines[3];
    _planeOffset[0] = rhs._planeOffset[0];
    _planeOffset[1] = rhs._planeOffset[1];
    _planeOffset[2] = rhs._planeOffset[2];
    _planeOffset[3] = rhs._planeOffset[3];
    _originalImage = rhs._originalImage;
    if (rhs._originalImageQt && rhs._copyQt)
    {
        const Image * prhs = &rhs;
        // copy/clone the QImage instance using the rhs object with the copyQt function ptr from rhs object
        _originalImageQt = (rhs.*rhs._copyQt)();
    }
    else
        _originalImageQt = nullptr;
    _freeQt = rhs._freeQt;
    _copyQt = rhs._copyQt;
    return *this;
}

void Image::loadPitchAndPlaneOffsets()
{
    switch (_format)
    {
    default:
    case ImageFormatInvalid:
        assert(false); // "invalid frame format"
        break;
    case ImageFormatAutomatic:
        assert(false); // "invalid frame format, automatic"
        break;
    case ImageFormatRaw:
        assert(false); // "invalid frame format, raw"
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
    {
        _pitch[0] = _width * 3;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatRGB32:
    case ImageFormatRGBA32:
    case ImageFormatARGB32:
    case ImageFormatBGR32:
    case ImageFormatBGRA32:
    case ImageFormatABGR32:
    case ImageFormatFloat:
    case ImageFormatDepth:
    {
        _pitch[0] = _width * 4;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatFloat64:
    {
        _pitch[0] = _width * 8;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatYUV444:
    {
        _pitch[0] = _pitch[1] = _pitch[2] = _width * 3;
        _pitch[3] = 0;
        _lines[0] = _lines[1] = _lines[2] = _height;
        _lines[3] = 0;
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
        _lines[0] = _lines[1] = _lines[2] = _height;
        _lines[3] = 0;
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
        _lines[0] = _height;
        _lines[1] = _lines[2] = _height / 2;
        _lines[3] = 0;
        _planeOffset[0] = 0;
        _planeOffset[1] = _width * _height;
        _planeOffset[2] = _planeOffset[1] + (_planeOffset[1] >> 2);
        _planeOffset[3] = 0;
    }
    break;

    case ImageFormatYUYV:
    case ImageFormatUYVY:
    {
        _pitch[0] = _width + _width;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
    case ImageFormatLuminance:
    case ImageFormatLuminanceAlpha:
    {
        // only one channel with 8-bit color data
        _pitch[0] = (_width * bitsForDataElement(_dataType)) / 8;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatDXT1:
    case ImageFormatDXT1Alpha:
    {
        // from http://www.gamedev.net/topic/615440-calculating-pitch-of-a-dxt-compressed-texture/
        _pitch[0] = ((_width + 3u) / 4u) * 8u;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatDXT3:
    case ImageFormatDXT5:
    {
        // from http://www.gamedev.net/topic/615440-calculating-pitch-of-a-dxt-compressed-texture/
        _pitch[0] = ((_width + 3u) / 4u) * 16u;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;
    case ImageFormatMono:
    case ImageFormatMonoLSB:
    case ImageFormatIndexed8:
    {
        _pitch[0] = _width;
        _pitch[1] = _pitch[2] = _pitch[3] = 0;
        _lines[0] = _height;
        _lines[1] = _lines[2] = _lines[3] = 0;
        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
    }
    break;

    }
}

void Image::free()
{
    if(_originalImageQt && _freeQt)
        (this->*_freeQt)();
    if(_allocated && _data)
    {
        ::free(_data);
        _data = NULL;
    }
}


bool Image::allocate(unsigned width, unsigned height, ImageFormat format, Origin origin)
{
    bool ret = false;
    unsigned allocated_width = ALIGN_BY_16(width);
    unsigned allocated_height = ALIGN_BY_16(height);
    free();
    _length = 0;
    switch (format)
    {
    default:
    case ImageFormatInvalid:
        assert(false); // "invalid frame format"
        break;
    case ImageFormatOriginal:
        assert(false); // "invalid frame format, original"
        break;
    case ImageFormatAutomatic:
        assert(false); // "invalid frame format, automatic"
        break;
    case ImageFormatRaw:
        assert(false); // "invalid frame format, raw"
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
        _length = allocated_width * allocated_height * 3;
        break;
    case ImageFormatRGB32:
    case ImageFormatRGBA32:
    case ImageFormatARGB32:
    case ImageFormatBGR32:
    case ImageFormatBGRA32:
    case ImageFormatABGR32:
    case ImageFormatFloat:
        _length = allocated_width * allocated_height * 4;
        break;
    case ImageFormatFloat64:
        _length = allocated_width * allocated_height * 8;
        break;
    case ImageFormatYUV444:
        _length = allocated_width * allocated_height * 3;
        break;
    case ImageFormatYUV422:
        _length = allocated_width * allocated_height * 2;
        break;
    case ImageFormatYUV420:
        _length = allocated_width * allocated_height + (allocated_width / 2 * allocated_height/2);
        break;

    case ImageFormatYUYV:
    case ImageFormatUYVY:
        _length = (allocated_width + allocated_width) * allocated_height;
        break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
        // only one channel with 8-bit color data
        _length = allocated_width * allocated_height;
        break;
    }
    if(_length)
    {
        _data = malloc(_length);
        _width = width;
        _height = height;
        _depth = 1;
        _allocatedWidth = allocated_width;
        _allocatedHeight = allocated_height;
        _format = format;
        _dataType = DataTypeUnsignedByte;
        _origin = origin;
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
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
    case ImageFormatGray:
        {
            // reinterpret a single channel image as a full color image
            switch(targetFormat)
            {
            case ImageFormatYUV444:
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
            case ImageFormatYUV422:
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
            case ImageFormatYUV420:
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
            case ImageFormatYUYV:
            case ImageFormatUYVY:
                {
                    // it's a YUV422 color format all in one plane
                    ret = (_height % 2) == 0;
                    if(ret)
                    {
                        _planeOffset[0] = _planeOffset[1] = _planeOffset[2] = _planeOffset[3] = 0;
                        _pitch[0] = _width + _width;
                        _pitch[1] = _pitch[2] = _pitch[3] = 0;
                    }
                }
                break;
            }
        }
        break;
    case ImageFormatBGR32:
    case ImageFormatRGB32:
    case ImageFormatRGBA32:
    case ImageFormatARGB32:
    case ImageFormatBGRA32:
    case ImageFormatABGR32:
        // reinterpret a 32-bit image and reinterpret the colors (helpful for wrong color
        // display)
        switch(targetFormat)
        {
        case ImageFormatBGR32:
        case ImageFormatRGB32:
        case ImageFormatARGB32:
        case ImageFormatRGBA32:
        case ImageFormatABGR32:
        case ImageFormatBGRA32:
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
    _allocatedWidth = width;
    _allocatedHeight = height;
    _depth = depth;
    _format = format;
    loadPitchAndPlaneOffsets();
    return true;
}

bool Image::guessImageSizes(ImageSizeList & possibleSizes) const
{
    bool ret = false;
    possibleSizes.clear();
    size_t totalNumberOfPixels = 0;
    switch(_format)
    {
    case ImageFormatARGB32:
    case ImageFormatRGBA32:
    case ImageFormatRGB32:
    case ImageFormatABGR32:
    case ImageFormatBGRA32:
    case ImageFormatBGR32:
    case ImageFormatFloat:
        ret = (_length % 4 == 0);
        if(ret)
            totalNumberOfPixels = _length / 4;
        break;
    case ImageFormatFloat64:
        ret = (_length % 8 == 0);
        if(ret)
            totalNumberOfPixels = _length / 8;
        break;
    case ImageFormatRGB24:
    case ImageFormatBGR24:
    case ImageFormatYUV444:
        ret = (_length % 3 == 0);
        if(ret)
            totalNumberOfPixels = _length / 3;
        break;
    case ImageFormatYUV422:
    case ImageFormatYUYV:
    case ImageFormatUYVY:
        ret = (_length % 2 == 0);
        if(ret)
            totalNumberOfPixels = _length / 2;
        break;
    case ImageFormatYUV420:
        ret = ((_length*2) % 3 == 0);
        if(ret)
            totalNumberOfPixels = (_length*2) / 3;
    }
    if(ret)
    {
        double d = std::sqrt(totalNumberOfPixels);
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

unsigned Image::bitsPerPixel() const
{
    unsigned ret = 0;
    switch (_format)
    {
    default:
    case ImageFormatInvalid: 
        ret = 0; break;
    case ImageFormatRGB24: 
    case ImageFormatBGR24:
        ret = 24; break;
    case ImageFormatARGB32:
    case ImageFormatRGB32: 
    case ImageFormatBGR32:
    case ImageFormatABGR32:
    case ImageFormatRGBA32:
    case ImageFormatBGRA32:
        ret = 32; break;
    case ImageFormatYUV420:
    case ImageFormatYUV422:
    case ImageFormatYUV444:
    case ImageFormatYUYV:
    case ImageFormatUYVY:
        ret = 16;
        break;
    case ImageFormatMono: 
    case ImageFormatMonoLSB: 
        ret = 1; 
        break;
    case ImageFormatIndexed8: 
        ret = 8;
        break;
    case ImageFormatFloat: 
        ret = 32; 
        break;
    case ImageFormatGray:
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
    case ImageFormatLuminance:
    case ImageFormatLuminanceAlpha:
    case ImageFormatDepth:
        ret = bitsForDataElement(_dataType);
        break;
    case ImageFormatDXT1: 
    case ImageFormatDXT1Alpha: 
        // 4x4 pixels go into 8 bytes
        ret = 4; 
        break;
    case ImageFormatDXT3:
    case ImageFormatDXT5:
        // 4x4 pixels go into 16 bytes
        ret = 8;
        break;
    case ImageFormatFloat64:
        ret = 64;
        break;
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
    case ImageFormatRGBA32: ret = "RGBA32"; break;
    case ImageFormatBGRA32: ret = "BGRA32"; break;
    case ImageFormatFloat64: ret = "Float64"; break;
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
    case ColorSpaceAutomatic: ret = "auto"; break;
    case ColorSpaceRGB: ret = "RGB"; break;
    case ColorSpaceYUV_ITU_R_BT_601: ret = "YUV ITU-R BT.601"; break;
    case ColorSpaceYUV_ITU_R_BT_709: ret = "YUV ITU-R BT.709"; break;
    case ColorSpaceYUV_ITU_R_BT_2020: ret = "YUV ITU-R BT.2020"; break;
    case ColorSpaceCYMK: ret = "CYMK"; break;
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

std::string Image::dataTypeToString(DataType type)
{
    std::string ret;
    switch (type)
    {
    case DataTypeInvalid: ret = "invalid"; break;
    case DataTypeUnsignedByte: ret = "u8"; break;
    case DataTypeSignedByte: ret = "i8"; break;
    case DataTypeUnsignedShort: ret = "u16"; break;
    case DataTypeSignedShort: ret = "i16"; break;
    case DataTypeUnsignedInt: ret = "u32"; break;
    case DataTypeSignedInt: ret = "i32"; break;
    case DataTypeFloat32: ret = "f32"; break;
    case DataTypeFloat64: ret = "f64"; break;
    case DataTypeARGB: ret = "argb"; break;
    default:
    {
        std::stringstream ss;
        ss << "Unknown(" << (int)type << ')';
        ret = ss.str();
    }
    break;
    }
    return ret;
}

unsigned Image::bitsForDataElement(DataType type)
{
    unsigned ret = 0;
    switch (type)
    {
    case DataTypeUnsignedByte: 
    case DataTypeSignedByte: 
        ret = 8; break;
    case DataTypeUnsignedShort: 
    case DataTypeSignedShort: 
        ret = 16; break;
    case DataTypeUnsignedInt:
    case DataTypeSignedInt:
        ret = 32; break;
    case DataTypeFloat32:
        ret = 32; break;
    case DataTypeFloat64:
        ret = 64; break;
    case DataTypeARGB:
        ret = 32; break;
    default: ret = 0; break;
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

unsigned Image::planeEndOffset(unsigned index) const
{ 
    unsigned ret = 0;
    if (index > 0)
        ret = planeOffset(index - 1);
    ret += _pitch[index] * _lines[index];
    return ret;
}

const void * Image::pixelDataPtr(unsigned x, unsigned y, unsigned z, unsigned plane) const
{
    const void * ret = nullptr;
    SGI_UNUSED(z);
    if (_data)
    {
        const uint8_t * src_data = reinterpret_cast<const uint8_t *>(_data);
        unsigned src_bits = bitsPerPixel();
        size_t src_offset = 0;
        switch(_origin)
        {
        case OriginTopLeft:
        default:
            src_offset = _planeOffset[plane] + ((y * _pitch[plane]) + (x * src_bits / 8));
            break;
        case OriginBottomLeft:
            src_offset = _planeOffset[plane] + (((_lines[plane] - y) * _pitch[plane]) + (x * src_bits / 8));
            break;
        }
        ret = src_data + src_offset;
    }
    return ret;
}

Image::Pixel Image::pixel(unsigned x, unsigned y, unsigned z, unsigned plane) const
{
    Pixel ret;
    switch (_format)
    {
    case ImageFormatABGR32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(px->a, px->b, px->g, px->r);
        }
        break;
    case ImageFormatARGB32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(px->a, px->r, px->g, px->b);
        }
        break;
    case ImageFormatBGRA32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(px->b, px->a, px->r, px->g);
        }
        break;
    case ImageFormatRGBA32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(px->b, px->g, px->r, px->a);
        }
        break;
    case ImageFormatBGR32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(0, px->b, px->g, px->r);
        }
        break;
    case ImageFormatRGB32:
        {
            const ARGB * px = pixelData<ARGB>(x, y);
            ret.setARGB(0, px->r, px->g, px->b);
        }
        break;

    case ImageFormatBGR24:
        {
            const RGB * px = pixelData<RGB>(x, y);
            ret.setARGB(0, px->b, px->g, px->r);
        }
        break;
    case ImageFormatRGB24:
        {
            const RGB * px = pixelData<RGB>(x, y);
            ret.setARGB(0, px->r, px->g, px->b);
        }
        break;
    case ImageFormatDepth:
    case ImageFormatFloat:
        {
            const float * px = pixelData<float>(x, y);
            ret.setFloat32(*px);
        }
        break;
    case ImageFormatRed:
    case ImageFormatGreen:
    case ImageFormatBlue:
    case ImageFormatAlpha:
    case ImageFormatGray:
    case ImageFormatLuminance:
    case ImageFormatLuminanceAlpha:
        {
            switch (_dataType)
            {
            case Image::DataTypeUnsignedByte:
            case Image::DataTypeSignedByte:
                {
                    const unsigned char * px = pixelData<unsigned char>(x, y);
                    ret.type = _dataType;
                    ret.data.u8 = *px;
                }
                break;
            case Image::DataTypeUnsignedShort:
            case Image::DataTypeSignedShort:
                {
                    const unsigned short * px = pixelData<unsigned short>(x, y);
                    ret.type = _dataType;
                    ret.data.u16 = *px;
                }
                break;
            case Image::DataTypeUnsignedInt:
            case Image::DataTypeSignedInt:
                {
                    const unsigned int * px = pixelData<unsigned int>(x, y);
                    ret.type = _dataType;
                    ret.data.u32 = *px;
                }
                break;
            case Image::DataTypeFloat32:
                {
                    const float * px = pixelData<float>(x, y);
                    ret.type = _dataType;
                    ret.data.f32[0] = *px;
                }
                break;
            case Image::DataTypeFloat64:
                {
                    const double * px = pixelData<double>(x, y);
                    ret.type = _dataType;
                    ret.data.f64[0] = *px;
                }
                break;
            }
        }
        break;
    }
    return ret;
}

float Image::hscale() const
{
    return (_allocatedWidth > 0) ? ((float)_width / (float)_allocatedWidth) : 1.0f;
}

float Image::vscale() const
{
    return (_allocatedHeight > 0) ? ((float)_height / (float)_allocatedHeight) : 1.0f;
}

float Image::horizontalPixelSize() const
{
    return (_allocatedWidth > 0) ? (1.0f / (float)_allocatedWidth) : 0.0f;
}

float Image::verticalPixelSize() const
{
    return (_allocatedHeight > 0) ? (1.0f / (float)_allocatedHeight) : 0.0f;
}



namespace {
    //static const double r10= 1.0/1023.0;
    //static const double r8 = 1.0/255.0;
    //static const double r6 = 1.0/63.0;
    static const double r5 = 1.0/31.0;
    //static const double r4 = 1.0/15.0;
    static const double r3 = 1.0/7.0;
    static const double r2 = 1.0/3.0;

    // The scale factors to convert from an image data type to a
    // float. This is copied from OSG; I think the factors for the signed
    // types are wrong, but need to investigate further.

    template<typename T> struct GLTypeTraits;

    template<> struct GLTypeTraits<int8_t>
    {
        static double scale(bool norm) { return norm? 1.0/128.0 : 1.0; } // XXX
        static constexpr Image::DataType dataType = Image::DataTypeSignedByte;
    };

    template<> struct GLTypeTraits<uint8_t>
    {
        static double scale(bool norm) { return norm? 1.0/255.0 : 1.0; }
        static constexpr Image::DataType dataType = Image::DataTypeUnsignedByte;
    };

    template<> struct GLTypeTraits<int16_t>
    {
        static double scale(bool norm) { return norm? 1.0/32768.0 : 1.0; } // XXX
        static constexpr Image::DataType dataType = Image::DataTypeSignedShort;
    };

    template<> struct GLTypeTraits<uint16_t>
    {
        static double scale(bool norm) { return norm? 1.0/65535.0 : 1.0; }
        static constexpr Image::DataType dataType = Image::DataTypeUnsignedShort;
    };

    template<> struct GLTypeTraits<int32_t>
    {
        static double scale(bool norm) { return norm? 1.0/2147483648.0 : 1.0; } // XXX
        static constexpr Image::DataType dataType = Image::DataTypeSignedInt;
    };

    template<> struct GLTypeTraits<uint32_t>
    {
        static double scale(bool norm) { return norm? 1.0/4294967295.0 : 1.0; }
        static constexpr Image::DataType dataType = Image::DataTypeUnsignedInt;
    };

    template<> struct GLTypeTraits<float>
    {
        static double scale(bool /*norm*/) { return 1.0; }
        static constexpr Image::DataType dataType = Image::DataTypeFloat32;
    };
    template<> struct GLTypeTraits<double>
    {
        static double scale(bool /*norm*/) { return 1.0; }
        static constexpr Image::DataType dataType = Image::DataTypeFloat64;
    };

    // The Reader function that performs the read.
    template<int Format, typename T> struct ColorReader;
    template<int Format, typename T> struct ColorWriter;


    template<typename T>
    struct ColorReader<Image::ImageFormatDepth, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            float l = float(*ptr) * GLTypeTraits<T>::scale(ia->_normalized);
            return Image::Pixel(Image::DataTypeFloat32, l);
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatDepth, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m)
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            (*ptr) = (T)(c.r() / GLTypeTraits<T>::scale(iw->_normalized));
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatLuminance, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(GLTypeTraits<T>::dataType, *ptr);
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatLuminance, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m)
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            (*ptr) = (T)(c.r() / GLTypeTraits<T>::scale(iw->_normalized));
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatRed, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(GLTypeTraits<T>::dataType, *ptr);
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatRed, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m)
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            (*ptr) = (T)(c.r() / GLTypeTraits<T>::scale(iw->_normalized));
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatAlpha, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(Image::DataTypeFloat32, *ptr);
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatAlpha, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m)
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            (*ptr) = (T)(c.a() / GLTypeTraits<T>::scale(iw->_normalized));
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatLuminanceAlpha, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            float l = float(*ptr++) * GLTypeTraits<T>::scale(ia->_normalized);
            float a = float(*ptr) * GLTypeTraits<T>::scale(ia->_normalized);
            return Image::Pixel(Image::DataTypeFloat32, a, l, l, l);
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatLuminanceAlpha, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m )
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            *ptr++ = (T)( c.r() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr   = (T)( c.a() / GLTypeTraits<T>::scale(iw->_normalized) );
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatRGB24, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(Image::DataTypeARGB, static_cast<unsigned char>(255), static_cast<unsigned char>(*(ptr+0)), static_cast<unsigned char>(*(ptr+1)), static_cast<unsigned char>(*(ptr+2)));
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatRGB24, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m )
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            *ptr++ = (T)( c.r() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.g() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.b() / GLTypeTraits<T>::scale(iw->_normalized) );
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatRGBA32, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(Image::DataTypeARGB, static_cast<unsigned char>(*(ptr+0)), static_cast<unsigned char>(*(ptr+1)), static_cast<unsigned char>(*(ptr+2)), static_cast<unsigned char>(*(ptr+3)));
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatRGBA32, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m)
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            *ptr++ = (T)( c.r() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.g() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.b() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.a() / GLTypeTraits<T>::scale(iw->_normalized) );
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatBGR24, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(Image::DataTypeARGB, static_cast<unsigned char>(255), static_cast<unsigned char>(*(ptr+2)), static_cast<unsigned char>(*(ptr+1)), static_cast<unsigned char>(*(ptr+0)));
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatBGR24, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m )
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            *ptr++ = (T)( c.b() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.g() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.r() / GLTypeTraits<T>::scale(iw->_normalized) );
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatBGRA32, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            const T* ptr = (const T*)ia->data(s, t, r, m);
            return Image::Pixel(Image::DataTypeARGB, static_cast<unsigned char>(*(ptr+0)), static_cast<unsigned char>(*(ptr+3)), static_cast<unsigned char>(*(ptr+2)), static_cast<unsigned char>(*(ptr+1)));
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatBGRA32, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m )
        {
            T* ptr = (T*)iw->data(s, t, r, m);
            *ptr++ = (T)( c.b() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.g() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.r() / GLTypeTraits<T>::scale(iw->_normalized) );
            *ptr++ = (T)( c.a() / GLTypeTraits<T>::scale(iw->_normalized) );
        }
    };

    template<typename T>
    struct ColorReader<Image::ImageFormatInvalid, T>
    {
        static Image::Pixel read(const PixelReader* ia, int s, int t, int r, int m)
        {
            return Image::Pixel();
        }
    };

    template<typename T>
    struct ColorWriter<Image::ImageFormatInvalid, T>
    {
        static void write(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m )
        {
            //nop
        }
    };


    template<Image::ImageFormat GLFormat>
    inline PixelReader::ReaderFunc
    chooseReader(Image::DataType dataType)
    {
        switch (dataType)
        {
        case Image::DataTypeSignedByte:
            return &ColorReader<GLFormat, int8_t>::read;
        case Image::DataTypeUnsignedByte:
            return &ColorReader<GLFormat, uint8_t>::read;
        case Image::DataTypeSignedShort:
            return &ColorReader<GLFormat, int16_t>::read;
        case Image::DataTypeUnsignedShort:
            return &ColorReader<GLFormat, uint16_t>::read;
        case Image::DataTypeSignedInt:
            return &ColorReader<GLFormat, int32_t>::read;
        case Image::DataTypeUnsignedInt:
            return &ColorReader<GLFormat, uint32_t>::read;
        case Image::DataTypeFloat32:
            return &ColorReader<GLFormat, float>::read;
        default:
            return &ColorReader<Image::ImageFormatInvalid, uint8_t>::read;
        }
    }

    inline PixelReader::ReaderFunc
    getReader( Image::ImageFormat imageFormat, Image::DataType dataType)
    {
        switch( imageFormat )
        {
        case Image::ImageFormatDepth:
            return chooseReader<Image::ImageFormatDepth>(dataType);
            break;
        case Image::ImageFormatLuminance:
            return chooseReader<Image::ImageFormatLuminance>(dataType);
            break;
        case Image::ImageFormatRed:
            return chooseReader<Image::ImageFormatRed>(dataType);
            break;
        case Image::ImageFormatAlpha:
            return chooseReader<Image::ImageFormatRed>(dataType);
            break;
        case Image::ImageFormatLuminanceAlpha:
            return chooseReader<Image::ImageFormatLuminanceAlpha>(dataType);
            break;
        case Image::ImageFormatRGB24:
            return chooseReader<Image::ImageFormatRGB24>(dataType);
            break;
        case Image::ImageFormatRGBA32:
            return chooseReader<Image::ImageFormatRGBA32>(dataType);
            break;
        case Image::ImageFormatBGR24:
            return chooseReader<Image::ImageFormatBGR24>(dataType);
            break;
        case Image::ImageFormatBGRA32:
            return chooseReader<Image::ImageFormatBGRA32>(dataType);
            break;
//        case Image::ImageFormatDXT1:
//            return &ColorReader<Image::ImageFormatDXT1, uint8_t>::read;
//            break;
        default:
            return nullptr;
            break;
        }
    }
}


PixelReader::PixelReader(const Image* image) :
_bilinear  (false)
{
    setImage(image);
}

void
PixelReader::setImage(const Image* image)
{
    _image = image;
    if (image)
    {
        _colMult = _image->bitsPerPixel() / 8;
        _rowMult = _image->pitch();
        _imageSize = _image->length();
        Image::DataType dataType = _image->dataType();
        _reader = getReader( _image->format(), dataType );
    }
}

Image::Pixel
PixelReader::operator()(float u, float v, int r, int m) const
{
    return operator()((double)u, (double)v, r, m);
}

Image::Pixel
PixelReader::operator()(double u, double v, int r, int m) const
 {
     if ( _bilinear )
     {
         double sizeS = (double)(_image->width()-1);
         double sizeT = (double)(_image->height()-1);

         // u, v => [0..1]
         double s = u * sizeS;
         double t = v * sizeT;

         double s0 = std::max(floorf(s), 0.0f);
         double s1 = std::min(s0+1.0f, sizeS);
         double smix = s0 < s1 ? (s-s0)/(s1-s0) : 0.0f;

         double t0 = std::max(floorf(t), 0.0f);
         double t1 = std::min(t0+1.0f, sizeT);
         double tmix = t0 < t1 ? (t-t0)/(t1-t0) : 0.0f;

         Image::Pixel UL = (*_reader)(this, (int)s0, (int)t0, r, m); // upper left
         Image::Pixel UR = (*_reader)(this, (int)s1, (int)t0, r, m); // upper right
         Image::Pixel LL = (*_reader)(this, (int)s0, (int)t1, r, m); // lower left
         Image::Pixel LR = (*_reader)(this, (int)s1, (int)t1, r, m); // lower right

         Image::Pixel TOP = UL*(1.0f-smix) + UR*smix;
         Image::Pixel BOT = LL*(1.0f-smix) + LR*smix;

         return TOP*(1.0f-tmix) + BOT*tmix;
     }
     else
     {
         return (*_reader)(this,
             (int)(u * (double)(_image->width()-1)),
             (int)(v * (double)(_image->height()-1)),
             r, m);
     }
}

bool
PixelReader::supports(Image::ImageFormat imageFormat, Image::DataType dataType)
{
    return getReader(imageFormat, dataType) != nullptr;
}



namespace
{
    inline PixelWriter::WriterFunc getWriter(Image::ImageFormat format, Image::DataType dataType)
    {
        switch( format )
        {
        default:
            return nullptr;
            break;
        }
    }
}

PixelWriter::PixelWriter(sgi::Image* image) :
_image(image)
{
    if (image)
    {
        //_normalized = ImageUtils::isNormalized(image);
        _colMult = _image->bitsPerPixel() / 8;
        _rowMult = _image->pitch();
        _imageSize = _image->length();
        Image::DataType dataType = _image->dataType();
        _writer = getWriter( _image->format(), dataType );
    }
}

bool
PixelWriter::supports(Image::ImageFormat format, Image::DataType dataType)
{
    return getWriter(format, dataType) != nullptr;
}


} // namespace sgi
