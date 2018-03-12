#include "stdafx.h"
#include "swscale.h"

#ifdef SGI_USE_FFMPEG
#include "dds.h"
#include "dxt.h"
#include "colorconversion.h"
#include <iostream>
#include <sgi/helpers/qt>

namespace ffmpeg {
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
}

#include <QLibrary>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace {

static ColorGradient s_defaultColorGradient(ColorGradient::MonochromeInverse);

static Image::ImageFormat ImageFormatFromAVCodec(ffmpeg::AVPixelFormat pix_fmt)
{
    Image::ImageFormat ret = Image::ImageFormatInvalid;
    switch (pix_fmt)
    {
    case ffmpeg::AV_PIX_FMT_NONE: ret = Image::ImageFormatInvalid; break;
    case ffmpeg::AV_PIX_FMT_RGB24: ret = Image::ImageFormatRGB24; break;
    case ffmpeg::AV_PIX_FMT_BGR24: ret = Image::ImageFormatBGR24; break;
    //case ffmpeg::AV_PIX_FMT_RGB32: ret = Image::ImageFormatRGB32; break;
    //case ffmpeg::AV_PIX_FMT_BGR32: ret = Image::ImageFormatBGR32; break;

    case ffmpeg::AV_PIX_FMT_PAL8:
    case ffmpeg::AV_PIX_FMT_ARGB:
        ret = Image::ImageFormatARGB32;
        break;
    case ffmpeg::AV_PIX_FMT_RGBA:
        ret = Image::ImageFormatRGBA32;
        break;
    case ffmpeg::AV_PIX_FMT_ABGR:
        ret = Image::ImageFormatABGR32;
        break;
    case ffmpeg::AV_PIX_FMT_BGRA:
        ret = Image::ImageFormatBGRA32;
        break;

    case ffmpeg::AV_PIX_FMT_YUVJ420P:
    case ffmpeg::AV_PIX_FMT_YUV420P: ret = Image::ImageFormatYUV420; break;
    case ffmpeg::AV_PIX_FMT_YUVJ422P:
    case ffmpeg::AV_PIX_FMT_YUV422P: ret = Image::ImageFormatYUV422; break;
    case ffmpeg::AV_PIX_FMT_YUVJ444P:
    case ffmpeg::AV_PIX_FMT_YUV444P: ret = Image::ImageFormatYUV444; break;
    case ffmpeg::AV_PIX_FMT_YUYV422: ret = Image::ImageFormatYUYV; break;
    case ffmpeg::AV_PIX_FMT_UYVY422: ret = Image::ImageFormatUYVY; break;
    case ffmpeg::AV_PIX_FMT_GRAY8:
    case ffmpeg::AV_PIX_FMT_GRAY16BE:
    case ffmpeg::AV_PIX_FMT_GRAY16LE:
        ret = Image::ImageFormatGray;
        break;
    case ffmpeg::AV_PIX_FMT_YA8: ret = Image::ImageFormatLuminanceAlpha; break;
    default:
        Q_ASSERT_X(false, __FUNCTION__, "Unhandled frame format from avcodec");
        break;
    }
    return ret;
}
static ffmpeg::AVPixelFormat ImageFormatToAVCodec(Image::ImageFormat format, Image::DataType dataType)
{
    ffmpeg::AVPixelFormat ret = ffmpeg::AV_PIX_FMT_NONE;
    switch (format)
    {
    case Image::ImageFormatRGB24: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
    case Image::ImageFormatRGB32: ret = ffmpeg::AV_PIX_FMT_RGB32; break;
    case Image::ImageFormatRGBA32: ret = ffmpeg::AV_PIX_FMT_RGBA; break;
    case Image::ImageFormatARGB32: ret = ffmpeg::AV_PIX_FMT_ARGB; break;
    case Image::ImageFormatMono: ret = ffmpeg::AV_PIX_FMT_MONOBLACK; break;
    case Image::ImageFormatMonoLSB: ret = ffmpeg::AV_PIX_FMT_MONOBLACK; break;
    case Image::ImageFormatIndexed8: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
    //case Image::ImageFormatFloat: ret = ffmpeg::AV_PIX_FMT_FLOAT; break;
    case Image::ImageFormatBGR24: ret = ffmpeg::AV_PIX_FMT_BGR24; break;
    case Image::ImageFormatBGR32: ret = ffmpeg::AV_PIX_FMT_BGR32; break;
    case Image::ImageFormatBGRA32: ret = ffmpeg::AV_PIX_FMT_BGRA; break;
    case Image::ImageFormatABGR32: ret = ffmpeg::AV_PIX_FMT_ABGR; break;
    case Image::ImageFormatYUV420: ret = ffmpeg::AV_PIX_FMT_YUV420P; break;
    case Image::ImageFormatYUV422: ret = ffmpeg::AV_PIX_FMT_YUV422P; break;
    case Image::ImageFormatYUV444: ret = ffmpeg::AV_PIX_FMT_YUV444P; break;
    case Image::ImageFormatYUYV: ret = ffmpeg::AV_PIX_FMT_YUYV422; break;
    case Image::ImageFormatUYVY: ret = ffmpeg::AV_PIX_FMT_UYVY422; break;
    case Image::ImageFormatGray:
    case Image::ImageFormatRed:
    case Image::ImageFormatGreen:
    case Image::ImageFormatBlue:
    case Image::ImageFormatLuminance:
    case Image::ImageFormatLuminanceAlpha:
        switch (dataType)
        {
        case Image::DataTypeSignedShort:
            ret = ffmpeg::AV_PIX_FMT_GRAY16LE;
            break;
        default:
            ret = ffmpeg::AV_PIX_FMT_GRAY8;
            break;
        }
        break;
    case Image::ImageFormatAlpha: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
    case Image::ImageFormatDepth: ret = ffmpeg::AV_PIX_FMT_RGB24; break;
    case Image::ImageFormatInvalid:
    default: ret = ffmpeg::AV_PIX_FMT_NONE; break;
    }
    return ret;
}

static DDS_COMPRESSION_TYPE ImageFormatToDDS(Image::ImageFormat format)
{
    DDS_COMPRESSION_TYPE ret = DDS_COMPRESS_NONE;
    switch (format)
    {
    case Image::ImageFormatDXT1: ret = DDS_COMPRESS_BC1; break;
    case Image::ImageFormatDXT1Alpha: ret = DDS_COMPRESS_BC1; break;
    case Image::ImageFormatDXT3: ret = DDS_COMPRESS_BC2; break;
    case Image::ImageFormatDXT5: ret = DDS_COMPRESS_BC3; break;
    default: ret = DDS_COMPRESS_NONE; break;
    }
    return ret;
}

} // namespace

bool SWScale::load()
{
    if(!loadAttempted)
    {
        library = new QLibrary(SGI_SWSCALE_LIBRARYNAME);
        if(library->load())
        {
            sws_getContext = (pfn_sws_getContext)library->resolve("sws_getContext");
            sws_scale = (pfn_sws_scale)library->resolve("sws_scale");
            sws_freeContext = (pfn_sws_freeContext)library->resolve("sws_freeContext");
            ready = (sws_getContext != NULL && sws_scale != NULL && sws_freeContext != NULL);
            std::cout << "Loaded swscale library " << library->fileName() << " ready=" << ready << std::endl;
        }
        else
        {
            std::cerr << "Failed to load swscale library: " << library->errorString() << std::endl;
        }
        loadAttempted = true;
    }
    return ready;
}


bool SWScale::convert_with_avcodec(const sgi::Image& src, sgi::Image& dest)
{
    ffmpeg::AVPixelFormat srcPixelFormat = ImageFormatToAVCodec(src.format(), src.dataType());
    ffmpeg::AVPixelFormat destPixelFormat = ImageFormatToAVCodec(dest.format(), src.dataType());
    if (srcPixelFormat == ffmpeg::AV_PIX_FMT_NONE || src.width() == 0 || src.height() == 0 || dest.width() == 0 || dest.height() == 0)
        return false;

    const uint8_t* srcdata = reinterpret_cast<const uint8_t*>(src.data());
    const int srcPitch[4] = { (int)src.pitch(0), (int)src.pitch(1), (int)src.pitch(2), (int)src.pitch(3) };
    const unsigned srcPlaneOffset[4] = { src.planeOffset(0), src.planeOffset(1), src.planeOffset(2), src.planeOffset(3) };
    const uint8_t* srcPlanes[4] = { srcdata + srcPlaneOffset[0], srcdata + srcPlaneOffset[1], srcdata + srcPlaneOffset[2], srcdata + srcPlaneOffset[3] };

    uint8_t* destdata = reinterpret_cast<uint8_t*>(dest.data());
    const int destPitch[4] = { (int)dest.pitch(0), (int)dest.pitch(1), (int)dest.pitch(2), (int)dest.pitch(3) };
    const unsigned destPlaneOffset[4] = { dest.planeOffset(0), dest.planeOffset(1), dest.planeOffset(2), dest.planeOffset(3) };
    uint8_t* destPlanes[4] = { destdata + destPlaneOffset[0], destdata + destPlaneOffset[1], destdata + destPlaneOffset[2], destdata + destPlaneOffset[3] };

    Q_ASSERT(sws_getContext != NULL);
    ffmpeg::SwsContext * ctx = sws_getContext(src.width(), src.height(), srcPixelFormat,
        dest.width(), dest.height(), destPixelFormat,
        SWS_BILINEAR, NULL, NULL, NULL);
    sws_scale(ctx, srcPlanes, srcPitch, 0, src.height(), destPlanes, destPitch);
    sws_freeContext(ctx);
    return true;
}

bool SWScale::to_qimage_argb32_with_avcodec(const sgi::Image& src, QImage& dest, bool horizontalFlip)
{
    ffmpeg::AVPixelFormat srcPixelFormat = ImageFormatToAVCodec(src.format(), src.dataType());
    if(srcPixelFormat == ffmpeg::AV_PIX_FMT_NONE || src.width() == 0 || src.height() == 0)
        return false;

    if(!horizontalFlip)
    {
        const uint8_t* srcdata = reinterpret_cast<const uint8_t*>(src.data());
        const int srcPitch[4] = { (int)src.pitch(0), (int)src.pitch(1), (int)src.pitch(2), (int)src.pitch(3) };
        const unsigned srcPlaneOffset[4] = { src.planeOffset(0), src.planeOffset(1), src.planeOffset(2), src.planeOffset(3) };
        const uint8_t* srcPlanes[4] = { srcdata + srcPlaneOffset[0], srcdata + srcPlaneOffset[1], srcdata + srcPlaneOffset[2], srcdata + srcPlaneOffset[3] };

        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);

        uint8_t* dstdata = reinterpret_cast<uint8_t*>(dest.bits());
        int dstPitch[4] = { (int)dest.bytesPerLine(), 0, 0, 0 };
        const unsigned dstPlaneOffset[4] = { 0, 0, 0, 0 };
        uint8_t * dstPlanes[4] = { dstdata + dstPlaneOffset[0], NULL, NULL, NULL };

        Q_ASSERT(sws_getContext != NULL);
        ffmpeg::SwsContext * ctx = sws_getContext(src.width(), src.height(), srcPixelFormat,
                                                    dest.width(), dest.height(),
                                                    // The QImage::Format_ARGB32 is the BGRA format on ffmpeg side, with the more
                                                    // obvious ffmpeg::AV_PIX_FMT_ARGB the red-blue colors are swapped.
                                                    ffmpeg::AV_PIX_FMT_BGRA,
                                                    SWS_BILINEAR, NULL, NULL, NULL);
        sws_scale(ctx, srcPlanes, srcPitch, 0, src.height(), dstPlanes, dstPitch);
        sws_freeContext(ctx);
    }
    else
    {
        const uint8_t* srcdata = reinterpret_cast<const uint8_t*>(src.data());
        const int srcPitch[4] = { -(int)src.pitch(0), -(int)src.pitch(1), -(int)src.pitch(2), -(int)src.pitch(3) };
        const unsigned srcPlaneOffset[4] = { src.planeEndOffset(0), src.planeEndOffset(1), src.planeEndOffset(2), src.planeEndOffset(3) };
        const uint8_t* srcPlanes[4] = { srcdata + srcPlaneOffset[0], srcdata + srcPlaneOffset[1], srcdata + srcPlaneOffset[2], srcdata + srcPlaneOffset[3] };

        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);

        uint8_t* dstdata = reinterpret_cast<uint8_t*>(dest.bits());
        int dstPitch[4] = { (int)dest.bytesPerLine(), 0, 0, 0 };
        const unsigned dstPlaneOffset[4] = { 0, 0, 0, 0 };
        uint8_t * dstPlanes[4] = { dstdata + dstPlaneOffset[0], NULL, NULL, NULL };

        Q_ASSERT(sws_getContext != NULL);
        ffmpeg::SwsContext * ctx = sws_getContext(src.width(), src.height(), srcPixelFormat,
                                                    dest.width(), dest.height(),
                                                    // The QImage::Format_ARGB32 is the BGRA format on ffmpeg side, with the more
                                                    // obvious ffmpeg::AV_PIX_FMT_ARGB the red-blue colors are swapped.
                                                    ffmpeg::AV_PIX_FMT_BGRA,
                                                    SWS_BILINEAR, NULL, NULL, NULL);
        sws_scale(ctx, srcPlanes, srcPitch, 0, src.height(), dstPlanes, dstPitch);
        sws_freeContext(ctx);
    }
    return true;
}

bool SWScale::convert_dxt(const sgi::Image& src, sgi::Image& dest)
{
    bool ret;
    sgi::Image tmp;
    ret = tmp.allocate(src.width(), src.height(), Image::ImageFormatBGRA32, src.origin());
    if (ret)
    {
        int format = ImageFormatToDDS(src.format());
        const int bytes_per_pixel = 4;
        const int normalize_blocks = 0;
        // actually the image is decoded into BGRA32
        ret = (dxt_decompress((unsigned char*)tmp.data(), (unsigned char*)src.data(), format, src.length(), src.width(), src.height(), bytes_per_pixel, normalize_blocks) != 0);
        if (ret)
        {
            ret = convert_with_avcodec(tmp, dest);
            tmp.free();
        }
    }
    return ret;
}

bool SWScale::to_qimage_argb32_dxt(const sgi::Image& src, QImage& dest, bool horizontalFlip)
{
    bool ret;
    sgi::Image tmp;
    ret = tmp.allocate(src.width(), src.height(), Image::ImageFormatBGRA32, src.origin());
    if (ret)
    {
        int format = ImageFormatToDDS(src.format());
        const int bytes_per_pixel = 4;
        const int normalize_blocks = 0;
        // actually the image is decoded into BGRA32
        ret = (dxt_decompress((unsigned char*)tmp.data(), (unsigned char*)src.data(), format, src.length(), src.width(), src.height(), bytes_per_pixel, normalize_blocks) != 0);
        if (ret)
        {
            ret = to_qimage_argb32_with_avcodec(tmp, dest, !horizontalFlip);
        }
    }
    return ret;
}

#define QIMAGE_RGB32(a,r,g,b) qRgba(r,g,b,a)
#define QIMAGE_RGB24(r,g,b) QIMAGE_RGB32(0xff,r,g,b)
#define QIMAGE_GRAY(gr) QIMAGE_RGB32(0xff,gr,gr,gr)

    template<typename ELEM_TYPE>
    static inline QRgb compute_pixel(const ColorGradient & colorGradient, const ELEM_TYPE & p, const ELEM_TYPE & min, const ELEM_TYPE & max, const ELEM_TYPE & range)
    {
        if (range != 0)
        {
            float value = float(p - min) / float(range);
            return colorGradient.getColor(value);
        }
        else
            return colorGradient.getInvalidColor();
    }
#if 0
    template<>
    static inline uint32_t compute_pixel<short>(const short & p, const short & min, const short & max, const short & range)
    {
        return QIMAGE_GRAY(p);
    }
    template<>
    static inline uint32_t compute_pixel<unsigned short>(const unsigned short & p, const unsigned short & min, const unsigned short & max, const unsigned short & range)
    {
        return QIMAGE_GRAY(p);
    }
    template<>
    static inline uint32_t compute_pixel<int>(const int & p, const int & min, const int & max, const int & range)
    {
        return QIMAGE_GRAY(p);
    }
    template<>
    static inline uint32_t compute_pixel<unsigned int>(const unsigned int & p, const unsigned int & min, const unsigned int & max, const unsigned int & range)
    {
        return QIMAGE_GRAY(p);
    }
#endif



    template<typename ELEM_TYPE>
    static bool to_qimage_argb32_single_channel_impl(const sgi::Image& src, QImage& dest, const ColorGradient & colorGradient, bool horizontalFlip)
    {
        dest = QImage(src.width(), src.height(), QImage::Format_ARGB32);

        size_t src_elem_size = sizeof(ELEM_TYPE);
        const uint8_t * src_data = reinterpret_cast<const uint8_t *>(src.data());
        uint8_t * dest_data = reinterpret_cast<uint8_t *>(dest.bits());
        const size_t dest_elem_size = 4; // RGB32
        unsigned src_pitch0 = src.pitch(0);
        unsigned dest_pitch0 = dest.bytesPerLine();
        ELEM_TYPE elem_min = std::numeric_limits<ELEM_TYPE>::max();
        ELEM_TYPE elem_max = std::numeric_limits<ELEM_TYPE>::min();
        for (unsigned x = 0; x < src.width(); ++x)
        {
            for (unsigned y = 0; y < src.height(); ++y)
            {
                size_t src_offset = (y * src_pitch0) + (x * src_elem_size);
                const ELEM_TYPE * src_pixel = reinterpret_cast<const ELEM_TYPE *>(src_data + src_offset);
                elem_min = std::min(elem_min, *src_pixel);
                elem_max = std::max(elem_max, *src_pixel);
            }
        }
        ELEM_TYPE elem_range = elem_max - elem_min;
        for (unsigned x = 0; x < src.width(); ++x)
        {
            for (unsigned y = 0; y < src.height(); ++y)
            {
                size_t src_offset = (y * src_pitch0) + (x * src_elem_size);
                size_t dest_offset = (y * dest_pitch0) + (x * dest_elem_size);
                const ELEM_TYPE * src_pixel = reinterpret_cast<const ELEM_TYPE *>(src_data + src_offset);
                QRgb* dest_pixel = reinterpret_cast<QRgb*>(dest_data + dest_offset);
                *dest_pixel = compute_pixel<ELEM_TYPE>(colorGradient, *src_pixel, elem_min, elem_max, elem_range);
            }
        }
        return true;
    }

bool SWScale::to_qimage_argb32_single_channel(const sgi::Image& src, QImage& dest, const ColorGradient & colorGradient, bool horizontalFlip)
{
    bool ret = false;
    switch (src.format())
    {
    case Image::ImageFormatGray:
    case Image::ImageFormatRed:
    case Image::ImageFormatGreen:
    case Image::ImageFormatBlue:
    case Image::ImageFormatLuminance:
    case Image::ImageFormatLuminanceAlpha:
    case Image::ImageFormatDepth:
        switch (src.dataType())
        {
        case Image::DataTypeSignedByte:
            ret = to_qimage_argb32_single_channel_impl<char>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeUnsignedByte:
            ret = to_qimage_argb32_single_channel_impl<unsigned char>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeSignedShort:
            ret = to_qimage_argb32_single_channel_impl<short>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeUnsignedShort:
            ret = to_qimage_argb32_single_channel_impl<unsigned short>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeSignedInt:
            ret = to_qimage_argb32_single_channel_impl<int>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeUnsignedInt:
            ret = to_qimage_argb32_single_channel_impl<unsigned int>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeFloat32:
            ret = to_qimage_argb32_single_channel_impl<float>(src, dest, colorGradient, horizontalFlip);
            break;
        case Image::DataTypeFloat64:
            ret = to_qimage_argb32_single_channel_impl<double>(src, dest, colorGradient, horizontalFlip);
            break;
        }
        break;
    }
    return ret;
}

bool SWScale::to_qimage_argb32_indexed8(const sgi::Image& src, QImage& dest, bool horizontalFlip)
{
    bool ret = false;
    QImage * src_qimg = src.originalImageQt();
    if(src_qimg)
    {
        dest = src_qimg->convertToFormat(QImage::Format_ARGB32);
        ret = !dest.isNull();
        if(ret && horizontalFlip)
            dest = dest.mirrored(false, true);
    }
    return ret;
}

bool SWScale::convert(const sgi::Image& src, sgi::Image& dest)
{
    bool ret;
    switch (src.format())
    {
    case Image::ImageFormatInvalid:
        ret = false;
        break;
    case Image::ImageFormatDXT1:
    case Image::ImageFormatDXT1Alpha:
    case Image::ImageFormatDXT3:
    case Image::ImageFormatDXT5:
        ret = convert_dxt(src, dest);
        break;
    case Image::ImageFormatIndexed8:
        ret = false;
        break;
    default:
        ret = convert_with_avcodec(src, dest);
        break;
    }
    return ret;
}

    bool SWScale::to_qimage_argb32(const sgi::Image& src, QImage& dest, bool horizontalFlip)
    {
        bool ret = false;
        switch (src.format())
        {
        case Image::ImageFormatDXT1:
        case Image::ImageFormatDXT1Alpha:
        case Image::ImageFormatDXT3:
        case Image::ImageFormatDXT5:
            ret = to_qimage_argb32_dxt(src, dest, horizontalFlip);
            break;
        case Image::ImageFormatGray:
            ret = to_qimage_argb32_single_channel(src, dest, ColorGradient(ColorGradient::Monochrome), horizontalFlip);
            break;
        case Image::ImageFormatRed:
        case Image::ImageFormatGreen:
        case Image::ImageFormatBlue:
        case Image::ImageFormatLuminance:
        case Image::ImageFormatLuminanceAlpha:
            ret = to_qimage_argb32_single_channel(src, dest, s_defaultColorGradient, horizontalFlip);
            break;
        case Image::ImageFormatIndexed8:
            ret = to_qimage_argb32_indexed8(src, dest, horizontalFlip);
            break;
#if defined(_WIN32) && 1
        case Image::ImageFormatRGB24:
            ret = qt_helpers::convertImageToQImage_RGB24(&src, dest);
            if (horizontalFlip)
                dest = dest.mirrored(false, true);
            break;
        case Image::ImageFormatBGR24:
            ret = qt_helpers::convertImageToQImage_BGR24(&src, dest);
            if (horizontalFlip)
                dest = dest.mirrored(false, true);
            break;
        case Image::ImageFormatRGB32:
            ret = qt_helpers::convertImageToQImage_RGB32(&src, dest);
            if (horizontalFlip)
                dest = dest.mirrored(false, true);
            break;
        case Image::ImageFormatBGR32:
            ret = qt_helpers::convertImageToQImage_BGR32(&src, dest);
            if (horizontalFlip)
                dest = dest.mirrored(false, true);
            break;
        case Image::ImageFormatRGBA32:
            ret = qt_helpers::convertImageToQImage_RGBA32(&src, dest);
            if (horizontalFlip)
                dest = dest.mirrored(false, true);
            break;
#endif
        default:
            ret = to_qimage_argb32_with_avcodec(src, dest, horizontalFlip);
            break;
        }
        return ret;
    }

bool SWScale::convert(const sgi::Image& src, Image::ImageFormat destFormat, sgi::Image& dest)
{
    bool ret;
    if(destFormat == src.format() || destFormat == Image::ImageFormatAutomatic)
    {
        dest = src;
        ret = true;
    }
    else
    {
        ret = load();
        if(ret)
        {
            ret = dest.allocate(src.width(), src.height(), destFormat);
            if(ret)
                ret = convert(src, dest);
        }
    }
    return ret;
}

bool SWScale::convertToQImage(const sgi::Image& src, Image::ImageFormat destFormat, QImage & dest)
{
    bool ret;
    sgi::Image tmpImage;
    ret = convert(src, destFormat, tmpImage);
    if(ret)
    {
        ret = load();
        if(ret)
        {
            bool horizontalFlip = src.origin() == sgi::Image::OriginBottomLeft;
            ret = to_qimage_argb32(tmpImage, dest, horizontalFlip);
        }
    }
    return ret;
}

bool SWScale::convertToQImage(const sgi::Image& src, QImage & dest)
{
    bool ret;
    ret = load();
    if(ret)
    {
        bool horizontalFlip = src.origin() == sgi::Image::OriginBottomLeft;
        ret = to_qimage_argb32(src, dest, horizontalFlip);
    }
    return ret;
}

template<>
inline QRgb compute_pixel<float>(const ColorGradient & colorGradient, const float & p, const float & min, const float & max, const float & range)
{
    if (range != 0)
    {
        if (std::isnan(p))
            return colorGradient.getInvalidColor();
        else
        {
            float value = float(p - min) / float(range);
            if (std::isnan(value))
                return colorGradient.getInvalidColor();
            else
                return colorGradient.getColor(value);
        }
    }
    else
        return colorGradient.getInvalidColor();
}

QLibrary * SWScale::library = NULL;
bool SWScale::loadAttempted = false;
bool SWScale::ready = false;
SWScale::pfn_sws_getContext SWScale::sws_getContext = NULL;
SWScale::pfn_sws_scale SWScale::sws_scale = NULL;
SWScale::pfn_sws_freeContext SWScale::sws_freeContext = NULL;

} // namespace sgi

#endif // defined(SGI_USE_FFMPEG)
