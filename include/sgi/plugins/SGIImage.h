// kate: syntax C++11;
// SGI - Copyright (C) 2012-2017 FAST Protect, Andreas Roth

#pragma once
#include <osg/Object>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <sgi/Export>

class QImage;

namespace sgi {

class SGI_IMPL_EXPORT Image : public osg::Referenced
{
public:
    enum ImageFormat {
        ImageFormatAutomatic = -2,
        ImageFormatInvalid = -1,
        ImageFormatRGB24 = 0,
        ImageFormatRGB32,
        ImageFormatARGB32,
        ImageFormatMono,
        ImageFormatMonoLSB,
        ImageFormatIndexed8,
        ImageFormatFloat,
        ImageFormatBGR24,
        ImageFormatBGR32,
        ImageFormatABGR32,
        ImageFormatYUV420,
        ImageFormatYUV422,
        ImageFormatYUV444,
        ImageFormatYUYV,
        ImageFormatUYVY,
        ImageFormatGray,
        ImageFormatRed,
        ImageFormatGreen,
        ImageFormatBlue,
        ImageFormatAlpha,
        ImageFormatDepth,
        ImageFormatLuminance,
        ImageFormatLuminanceAlpha,
        ImageFormatDXT1,
        ImageFormatDXT1Alpha,
        ImageFormatDXT3,
        ImageFormatDXT5,
        ImageFormatRGBA32,
        ImageFormatBGRA32,
        ImageFormatFloat64,
        ImageFormatRaw = 1000,
    };
    static std::string imageFormatToString(ImageFormat format);
    enum ColorSpace {
        ColorSpaceInvalid = -1,
        ColorSpaceAutomatic = 0,
        ColorSpaceRGB,
        ColorSpaceYUV_ITU_R_BT_601, /* used by SDTV video and JPEG */
        ColorSpaceYUV_ITU_R_BT_709, /* used by HDTV video */
        ColorSpaceYUV_ITU_R_BT_2020,
        ColorSpaceCYMK,
    };
    static std::string colorSpaceToString(ColorSpace colorspace);
    enum DataType {
        DataTypeInvalid = -1,
        DataTypeUnsignedByte,
        DataTypeSignedByte,
        DataTypeUnsignedShort,
        DataTypeSignedShort,
        DataTypeUnsignedInt,
        DataTypeSignedInt,
        DataTypeFloat32,
        DataTypeFloat64,
        DataTypeDouble = DataTypeFloat64,
    };
    static std::string dataTypeToString(DataType type);
    static unsigned bitsForDataElement(DataType type);

    enum Origin {
        OriginBottomLeft,
        OriginTopLeft,
        OriginDefault = OriginBottomLeft
    };
    static std::string originToString(Origin o);

    struct ImageSize {
        ImageSize(Image::ImageFormat f, unsigned w, unsigned h, unsigned d=1)
            : format(f), width(w), height(h), depth(d) {}
        Image::ImageFormat format;
        unsigned width;
        unsigned height;
        unsigned depth;
    };
    typedef std::vector<ImageSize> ImageSizeList;

    Image(ImageFormat format=ImageFormatInvalid, DataType type=DataTypeInvalid);
    explicit Image(ImageFormat format, DataType type, void * data, size_t length, bool copyData=true);
    explicit Image(ImageFormat format, DataType type, Origin origin, void * data, size_t length,
          unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
          const osg::Referenced * originalImage, bool copyData = false);
    explicit Image(ImageFormat format, DataType type, Origin origin, void * data, size_t length,
          unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
          QImage * originalImage, bool copyData = false);
    Image(QImage * originalImage, bool copyData = false);

    Image(const Image & rhs);
    Image & operator=(const Image & rhs);
    virtual ~Image();

    bool empty() const;
    ImageFormat format() const { return _format; }
    DataType dataType() const { return _dataType; }
    const void * data() const { return _data; }
    void * data() { return _data; }
    size_t length() const { return _length; }
    unsigned width() const { return _width; }
    unsigned height() const { return _height; }
    unsigned depth() const { return _depth; }
    unsigned allocatedWidth() const { return _allocatedWidth; }
    unsigned allocatedHeight() const { return _allocatedHeight; }
    unsigned pitch(unsigned index=0) const { return _pitch[index]; }
    unsigned lines(unsigned index=0) const { return _lines[index]; }
    unsigned planeOffset(unsigned index=0) const { return _planeOffset[index]; }
    unsigned planeEndOffset(unsigned index = 0) const;
    const osg::Referenced * originalImage() const { return _originalImage.get(); }
    QImage * originalImageQt() const { return _originalImageQt; }
    Origin origin() const { return _origin; }
    bool allocate(unsigned width, unsigned height, ImageFormat format, Origin origin=OriginDefault);
    void free();
    bool reinterpretFormat(ImageFormat targetFormat);
    bool guessImageSizes(ImageSizeList & possibleSizes) const;
    bool reinterpret(const ImageSize & size);
    bool reinterpret(ImageFormat format, unsigned width, unsigned height, unsigned depth = 1);
    unsigned bitsPerPixel() const;

    const void * pixelPtr(unsigned x, unsigned y, unsigned z = 0, unsigned plane=0) const;
    template<typename PXTYPE>
    const PXTYPE * pixel(unsigned x, unsigned y, unsigned z = 0, unsigned plane = 0) const
    {
        return reinterpret_cast<const PXTYPE*>(pixelPtr(x, y, z, plane));
    }

    float hscale() const;
    float vscale() const;

    float horizontalPixelSize() const;
    float verticalPixelSize() const;

protected:
    void loadPitchAndPlaneOffsets();
    void freeQt();
    QImage * copyQt() const;
    typedef void (Image::* pfnFreeQt)();
    typedef QImage * (Image::* pfnCopyImageQt)() const;

protected:
    ImageFormat _format;
    DataType _dataType;
    Origin _origin;
    void * _data;
    size_t _length;
    unsigned _width;
    unsigned _height;
    unsigned _depth;
    unsigned _allocatedWidth;
    unsigned _allocatedHeight;
    unsigned _pitch[4];
    unsigned _lines[4];
    unsigned _planeOffset[4];
    osg::ref_ptr<const osg::Referenced> _originalImage;
    QImage * _originalImageQt;
    pfnFreeQt _freeQt;
    pfnCopyImageQt _copyQt;
    bool _allocated;
};

} // namespace sgi

namespace std {
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, sgi::Image::ImageFormat f)
{
    return os << sgi::Image::imageFormatToString(f);
}
} // namespace std
