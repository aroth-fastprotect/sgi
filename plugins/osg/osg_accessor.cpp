#include "stdafx.h"
#include <osg/Texture>
#include <osg/Stats>
#include "osg_accessor.h"

#include <sgi/details/functionImplBase>
#include <sgi/helpers/osg>

#include <sstream>

namespace sgi {
namespace osg_plugin {

TextureAccess::TextureObjectList TextureAccess::getTextureObjectList() const
{
    unsigned size = _textureObjectBuffer.size();
    TextureObjectList ret(size);
    for (unsigned n = 0; n < size; ++n)
        ret[n] = _textureObjectBuffer[n];
    return ret;
}

unsigned TextureObjectManagerAccess::hash(const osg::Texture::TextureProfile & profile)
{
    return (unsigned)(profile._target << 16) ^ (profile._width << 8) ^ profile._height ^ profile._internalFormat;
}

std::string TextureObjectManagerAccess::shortName(const osg::Texture::TextureProfile & profile)
{
    std::stringstream ss;
    switch (profile._target)
    {
    case GL_TEXTURE_1D: ss << "1D"; break;
    case GL_TEXTURE_2D: ss << "2D"; break;
    case GL_TEXTURE_3D: ss << "3D"; break;
    default: ss << sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(profile._target); break;
    }
    ss << ' ' << profile._width << 'x' << profile._height;
    if (profile._depth != 1)
        ss << 'x' << profile._depth;

    std::string format = sgi::castToEnumValueString<sgi::osg_helpers::GLEnum>(profile._internalFormat);
    if (format.compare(0, 14, "GL_COMPRESSED_") == 0)
        ss << ' ' << format.substr(14);
    else if(format.compare(0, 3, "GL_") == 0)
        ss << ' ' << format.substr(3);
    else
        ss << ' ' << format;
    std::string ret = ss.str();
    return ret;
}

TextureObjectSetAccess::TextureObjectList TextureObjectSetAccess::getTextureObjects()
{
    TextureObjectList ret;
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    for (TextureObject* to = _head; to != NULL; to = to->_next)
    {
        ret.push_back(to);
    }
    return ret;
}

} // namespace osg_plugin
} // namespace sgi

