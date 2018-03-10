#pragma once

#if defined(SGI_USE_FFMPEG)
#include <sgi/plugins/SGIImage.h>

class QLibrary;
namespace ffmpeg {
    struct SwsContext;
    struct SwsFilter;
} // namespace ffmpeg

namespace sgi {

class Image;
class ColorGradient;

class SWScale
{
private:
    static QLibrary * library;
    static bool loadAttempted;
    static bool ready;
    typedef struct ffmpeg::SwsContext * (*pfn_sws_getContext)(int srcW, int srcH, int srcFormat,
                                  int dstW, int dstH, int dstFormat,
                                  int flags, ffmpeg::SwsFilter *srcFilter,
                                  ffmpeg::SwsFilter *dstFilter, const double *param);
    typedef int (*pfn_sws_scale)(struct ffmpeg::SwsContext *c, const uint8_t *const srcSlice[],
              const int srcStride[], int srcSliceY, int srcSliceH,
              uint8_t *const dst[], const int dstStride[]);
    typedef void (*pfn_sws_freeContext)(struct ffmpeg::SwsContext *swsContext);
    static pfn_sws_getContext sws_getContext;
    static pfn_sws_scale sws_scale;
    static pfn_sws_freeContext sws_freeContext;

private:
    static bool convert_with_avcodec(const sgi::Image& src, sgi::Image& dest);
    static bool convert_dxt(const sgi::Image& src, sgi::Image& dest);

    static bool to_qimage_argb32_with_avcodec(const sgi::Image& src, QImage& dest, bool horizontalFlip=false);
    static bool to_qimage_argb32_dxt(const sgi::Image& src, QImage& dest, bool horizontalFlip = false);
    static bool to_qimage_argb32_single_channel(const sgi::Image& src, QImage& dest, const ColorGradient & colorGradient, bool horizontalFlip = false);
    static bool to_qimage_argb32_indexed8(const sgi::Image& src, QImage& dest, bool horizontalFlip = false);
    static bool to_qimage_argb32(const sgi::Image& src, QImage& dest, bool horizontalFlip = false);

    static bool convert(const sgi::Image& src, sgi::Image& dest);

public:
    static bool load();

    static bool convert(const sgi::Image& src, Image::ImageFormat destFormat, sgi::Image& dest);
    static bool convertToQImage(const sgi::Image& src, Image::ImageFormat destFormat, QImage & dest);
    static bool convertToQImage(const sgi::Image& src, QImage & dest);
};
} // namespace sgi

#endif // defined(SGI_USE_FFMPEG)
