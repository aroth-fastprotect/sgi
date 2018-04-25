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
        ImageFormatOriginal = -3,
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
        DataTypeARGB,
    };
    static std::string dataTypeToString(DataType type);
    static unsigned bitsForDataElement(DataType type);

    enum Origin {
        OriginBottomLeft,
        OriginTopLeft,
        OriginDefault = OriginBottomLeft
    };
    static std::string originToString(Origin o);

    struct SGI_IMPL_EXPORT ImageSize
    {
        ImageSize(Image::ImageFormat f, unsigned w, unsigned h, unsigned d=1)
            : format(f), width(w), height(h), depth(d) {}
        Image::ImageFormat format;
        unsigned width;
        unsigned height;
        unsigned depth;
    };
    typedef std::vector<ImageSize> ImageSizeList;

    struct SGI_IMPL_EXPORT ARGB
    {
        unsigned char a;
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    struct SGI_IMPL_EXPORT RGB
    {
        RGB(unsigned char r_, unsigned char g_, unsigned char b_)
            : r(r_), g(g_), b(b_) {}
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    class SGI_IMPL_EXPORT Pixel
    {
    public:
        union PixelData {
            unsigned char u8;
            signed char s8;
            unsigned short u16;
            signed short s16;
            unsigned int u32;
            signed int s32;
            float f32[4];
            double f64[2];
            ARGB argb;
        };

        Pixel();
        Pixel(DataType t, const PixelData & d);
        Pixel(DataType t, float a, float r, float g, float b);
        Pixel(DataType t, float f);
        Pixel(DataType t, double f);
        Pixel(DataType t, unsigned char f);
        Pixel(DataType t, unsigned char a_, unsigned char r_, unsigned char g_, unsigned char b_);
        Pixel(DataType t, signed char f);
        Pixel(DataType t, unsigned short f);
        Pixel(DataType t, signed short f);
        Pixel(DataType t, unsigned int f);
        Pixel(DataType t, signed int f);
        Pixel(DataType t, const RGB & rgb);
        Pixel(DataType t, const ARGB & argb);
        bool valid() const { return type != DataTypeInvalid; }
        void clear();
        void setARGB(const ARGB & rhs);
        void setARGB(unsigned char a, unsigned char r, unsigned char g, unsigned char b);
        void setFloat32(float a=0.0f, float r=0.0f, float g=0.0f, float b=0.0f);
        void setFloat64(double f, double f2=0);
        DataType type;
        PixelData data;

        float a() const;
        float r() const;
        float g() const;
        float b() const;

        float lumaF() const;

        int alpha() const;
        int red() const;
        int green() const;
        int blue() const;
        int gray() const;

        Pixel & operator *= (const double f);
        Pixel operator*(const double f) const;
        Pixel operator+(const Pixel & rhs) const;

        std::string toString() const;
    };

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

    const void * pixelDataPtr(unsigned x, unsigned y, unsigned z = 0, unsigned plane=0) const;
    template<typename PXTYPE>
    const PXTYPE * pixelData(unsigned x, unsigned y, unsigned z = 0, unsigned plane = 0) const
    {
        return reinterpret_cast<const PXTYPE*>(pixelDataPtr(x, y, z, plane));
    }
    Pixel pixel(unsigned x, unsigned y, unsigned z = 0, unsigned plane = 0) const;

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

/**
 * Reads color data out of an image, regardles of its internal pixel format.
 */
class SGI_IMPL_EXPORT PixelReader
{
public:
    /**
     * Constructs a pixel reader. "Normalized" means that the values in the source
     * image have been scaled to [0..1] and should be denormalized upon reading.
     */
    PixelReader(const sgi::Image* image);

    /** Sets an image to read. */
    void setImage(const sgi::Image* image);

    /** Whether to use bilinear interpolation when reading with u,v coords (default=true) */
    void setBilinear(bool value) { _bilinear = value; }

    /** Whether PixelReader supports a given format/datatype combiniation. */
    static bool supports( Image::ImageFormat imageFormat, Image::DataType dataType );

    /** Whether PixelReader can read from the specified image. */
    static bool supports( const sgi::Image* image ) {
        return image && supports(image->format(), image->dataType() );
    }

    /** Reads a color from the image */
    Image::Pixel operator()(int s, int t, int r=0, int m=0) const {
        return (*_reader)(this, s, t, r, m);
    }

    /** Reads a color from the image */
    Image::Pixel operator()(unsigned s, unsigned t, unsigned r=0, int m=0) const {
        return (*_reader)(this, s, t, r, m);
    }

    /** Reads a color from the image by unit coords [0..1] */
    Image::Pixel operator()(float u, float v, int r=0, int m=0) const;
    Image::Pixel operator()(double u, double v, int r=0, int m=0) const;

    // internals:
    const unsigned char* data(int s=0, int t=0, int r=0, int m=0) const {
        return static_cast<const unsigned char*>(_image->data()) + s*_colMult + t*_rowMult + r*_imageSize;
    }

    typedef Image::Pixel (*ReaderFunc)(const PixelReader* ia, int s, int t, int r, int m);
    ReaderFunc _reader;
    const sgi::Image* _image;
    unsigned _colMult;
    unsigned _rowMult;
    unsigned _imageSize;
    bool     _normalized;
    bool     _bilinear;
};

/**
 * Writes color data to an image, regardles of its internal pixel format.
 */
class SGI_IMPL_EXPORT PixelWriter
{
public:
    /**
     * Constructs a pixel writer. "Normalized" means the values are scaled to [0..1]
     * before writing.
     */
    PixelWriter(sgi::Image* image);

    /** Whether PixelWriter can write to an image with the given format/datatype combo. */
    static bool supports( Image::ImageFormat imageFormat, Image::DataType dataType );

    /** Whether PixelWriter can write to non-const version of an image. */
    static bool supports( const sgi::Image* image ) {
        return image && supports(image->format(), image->dataType() );
    }

    /** Writes a color to a pixel. */
    void operator()(const Image::Pixel& c, int s, int t, int r=0, int m=0) {
        (*_writer)(this, c, s, t, r, m );
    }

    void f(const Image::Pixel& c, float s, float t, int r=0, int m=0) {
        this->operator()( c,
            (int)(s * (float)(_image->width()-1)),
            (int)(t * (float)(_image->height()-1)),
            r, m);
    }

    // internals:
    sgi::Image* _image;
    unsigned _colMult;
    unsigned _rowMult;
    unsigned _imageSize;
    bool     _normalized;

    unsigned char* data(int s=0, int t=0, int r=0, int m=0) const {
        return static_cast<unsigned char*>(_image->data()) + s*_colMult + t*_rowMult + r*_imageSize;
    }

    typedef void (*WriterFunc)(const PixelWriter* iw, const Image::Pixel& c, int s, int t, int r, int m);
    WriterFunc _writer;
};

/**
 * Functor that visits every pixel in an image
 */
template<typename T>
struct PixelVisitor : public T
{
    /**
     * Traverse an image, and call this method on the superclass:
     *
     *   bool operator(osg::Vec4& pixel);
     *
     * If that method returns true, write the value back at the same location.
     */
    void accept( Image* image ) {
        PixelReader _reader( image );
        PixelWriter _writer( image );
        for( int r=0; r<image->depth(); ++r ) {
            for( int t=0; t<image->height(); ++t ) {
                for( int s=0; s<image->width(); ++s ) {
                    Image::Pixel pixel = _reader(s,t,r);
                    if ( (*this)(pixel) )
                        _writer(pixel,s,t,r);
                }
            }
        }
    }

    /**
     * Traverse an image, and call this method on the superclass:
     *
     *   bool operator(const osg::Vec4& srcPixel, osg::Vec4& destPixel);
     *
     * If that method returns true, write destPixel back at the same location
     * in the destination image.
     */
    void accept( const Image* src, Image* dest ) {
        PixelReader _readerSrc( src );
        PixelReader _readerDest( dest );
        PixelWriter _writerDest( dest );
        for( int r=0; r<src->depth(); ++r ) {
            for( int t=0; t<src->height(); ++t ) {
                for( int s=0; s<src->width(); ++s ) {
                    const Image::Pixel pixelSrc = _readerSrc(s,t,r);
                    Image::Pixel pixelDest = _readerDest(s,t,r);
                    if ( (*this)(pixelSrc, pixelDest) )
                        _writerDest(pixelDest,s,t,r);
                }
            }
        }
    }
};

/**
 * Simple functor to copy pixels from one image to another.
 *
 * Usage:
 *    PixelVisitor<CopyImage>().accept( fromImage, toImage );
 */
struct CopyImage {
    bool operator()( const Image::Pixel& src, Image::Pixel& dest ) {
        dest = src;
        return true;
    }
};


} // namespace sgi

namespace std {
inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, sgi::Image::ImageFormat f)
{
    return os << sgi::Image::imageFormatToString(f);
}
} // namespace std
