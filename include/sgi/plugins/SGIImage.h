// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include <osg/Object>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

class QImage;

namespace sgi {

class Image : public osg::Referenced
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

    Image(ImageFormat format=ImageFormatInvalid);
    explicit Image(ImageFormat format, void * data, size_t length, bool copyData=true);
    explicit Image(ImageFormat format, Origin origin, void * data, size_t length,
          unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
          const osg::Referenced * originalImage, bool copyData = false);
    explicit Image(ImageFormat format, Origin origin, void * data, size_t length,
          unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
          QImage * originalImage, bool copyData = false);
    Image(QImage * originalImage, bool copyData = false);
    Image(const Image & rhs);
    Image & operator=(const Image & rhs);
    virtual ~Image();

    ImageFormat format() const { return _format; }
    const void * data() const { return _data; }
    void * data() { return _data; }
    size_t length() const { return _length; }
    unsigned width() const { return _width; }
    unsigned height() const { return _height; }
    unsigned depth() const { return _depth; }
    unsigned pitch(unsigned index=0) const { return _pitch[index]; }
    unsigned planeOffset(unsigned index=0) const { return _planeOffset[index]; }
    const osg::Referenced * originalImage() const { return _originalImage.get(); }
	QImage * originalImageQt() const { return _originalImageQt; }
    Origin origin() const { return _origin; }
    bool allocate(unsigned width, unsigned height, ImageFormat format);
    void free();
    bool reinterpretFormat(ImageFormat targetFormat);
    bool guessImageSizes(ImageSizeList & possibleSizes) const;
    bool reinterpret(const ImageSize & size);
    bool reinterpret(ImageFormat format, unsigned width, unsigned height, unsigned depth = 1);

protected:
    ImageFormat _format;
    Origin _origin;
    void * _data;
    size_t _length;
    unsigned _width;
    unsigned _height;
    unsigned _depth;
    unsigned _pitch[4];
    unsigned _planeOffset[4];
    osg::ref_ptr<const osg::Referenced> _originalImage;
	QImage * _originalImageQt;
    bool _allocated;
};

} // namespace sgi

namespace std {
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, sgi::Image::ImageFormat f)
{
    return os << sgi::Image::imageFormatToString(f);
}
} // namespace std
