// kate: syntax C++;
// SGI - Copyright (C) 2012-2023 FAST Protect, Andreas Roth

#include <sgi/helpers/string>
#include <sgi/helpers/rtti>
#include <sgi/helpers/vsg>

#include <sgi/plugins/SGIImage.h>

#include <vsg/core/Object.h>
#include <vsg/core/Auxiliary.h>
#include <vsg/core/Data.h>

namespace sgi {

    namespace vsg_helpers {

std::string getObjectTypename(const vsg::Auxiliary * object)
{
    return object?(helpers::getRTTITypename_html(object)):"(null)";
}

std::string getObjectTypename(const vsg::Object * object)
{
    return object?(std::string(object->className())):"(null)";
}


std::string getObjectName(const vsg::Auxiliary * object, bool includeAddr)
{
    std::string ret;
    if(object) {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectName(const vsg::Object * object, bool includeAddr)
{
    std::string ret;
    if(object) {
        std::stringstream buf;
        buf << (void*)object;
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectNameAndType(const vsg::Auxiliary * object, bool includeAddr)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << (void*)object;
        buf << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

std::string getObjectNameAndType(const vsg::Object * object, bool includeAddr)
{
    std::string ret;
    if(object)
    {
        std::stringstream buf;
        buf << getObjectName(object, includeAddr) << " (" << getObjectTypename(object) << ")";
        ret = buf.str();
    }
    else
        ret = "(null)";
    return ret;
}

const sgi::Image * convertImage(const vsg::Data * image)
{
    if (!image)
        return nullptr;

    const vsg::Data::Properties & properties = image->properties;
    sgi::Image * ret = nullptr;
    sgi::Image::ImageFormat imageFormat;
    sgi::Image::DataType dataType;
    switch(properties.format)
    {
    case 0:imageFormat = sgi::Image::ImageFormatInvalid; break;
    case VK_FORMAT_R8G8_UNORM:
        imageFormat = sgi::Image::ImageFormatRGB24; break;
    case VK_FORMAT_R16G16_UNORM:
        imageFormat = sgi::Image::ImageFormatRGB24; break;
    default: imageFormat = sgi::Image::ImageFormatInvalid; break;
    }
    dataType = sgi::Image::DataTypeInvalid;

    sgi::Image::Origin origin = (properties.origin == vsg::TOP_LEFT) ? sgi::Image::OriginTopLeft : sgi::Image::OriginBottomLeft;
    if(imageFormat != sgi::Image::ImageFormatInvalid)
    {
        ret = new sgi::Image(imageFormat, dataType, origin,
                             const_cast<void*>(image->dataPointer()), image->dataSize(),
                             image->width(), image->height(), image->depth(), image->stride(),
                             image);
    }
    return ret;
}

    } // namespace vsg_helpers
} // namespace sgi
