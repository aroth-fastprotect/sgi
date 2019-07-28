// kate: syntax C++11;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

// tricking the SGI_IMPL_EXPORT defined to switch to export
#define SGI_IMPL_LIBRARY
#include <sgi/plugins/SGIImage.h>
#include <sgi/helpers/qt>
#include <sstream>
#include <cmath>
#include <osg/Referenced>

#define ALIGN_SIZE(x,n)       ((size_t)((~(n-1))&((x)+(n-1))))
#define ALIGN_BY_16(x) ALIGN_SIZE(x, 16)
#define ALIGN_BY_8(x) ALIGN_SIZE(x, 8)
#define ALIGN_BY_4(x) ALIGN_SIZE(x, 4)

namespace sgi {

Image::Image(ImageFormat format, DataType type, Origin origin, void * data, size_t length,
    unsigned width, unsigned height, unsigned depth, unsigned bytesPerLine,
    const osg::Referenced * originalImage, bool copyData)
    : _format(format), _dataType(type), _origin(origin), _data(copyData ? malloc(length) : data), _length(length)
    , _width(width), _height(height), _depth(depth)
    , _allocatedWidth(width), _allocatedHeight(height)
    , _pitch{ bytesPerLine, 0, 0, 0 }, _lines{ height, 0, 0, 0 }
    , _planeOffset{ 0, 0, 0, 0 }
    , _originalImage(originalImage), _originalImageQt(nullptr), _freeQt(nullptr), _copyQt(nullptr)
    , _allocated(copyData)
{
    if (copyData)
        memcpy(_data, data, length);
    else if (_originalImage)
        const_cast<osg::Referenced*>(_originalImage)->ref();
    loadPitchAndPlaneOffsets();
}


} // namespace sgi
