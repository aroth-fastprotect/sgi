#include "stdafx.h"
#include <osgText/Text>

#include "std_ostream_osgtext.h"

namespace std {

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::AxisAlignment & t)
{
    switch(t)
    {
    case osgText::TextBase::XY_PLANE: os << "XY_PLANE"; break;
    case osgText::TextBase::REVERSED_XY_PLANE: os << "REVERSED_XY_PLANE"; break;
    case osgText::TextBase::XZ_PLANE: os << "XZ_PLANE"; break;
    case osgText::TextBase::REVERSED_XZ_PLANE: os << "REVERSED_XZ_PLANE"; break;
    case osgText::TextBase::YZ_PLANE: os << "YZ_PLANE"; break;
    case osgText::TextBase::REVERSED_YZ_PLANE: os << "REVERSED_YZ_PLANE"; break;
    case osgText::TextBase::SCREEN: os << "SCREEN"; break;
    case osgText::TextBase::USER_DEFINED_ROTATION: os << "USER_DEFINED_ROTATION"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::Layout & t)
{
    switch(t)
    {
    case osgText::TextBase::LEFT_TO_RIGHT: os << "LEFT_TO_RIGHT"; break;
    case osgText::TextBase::RIGHT_TO_LEFT: os << "RIGHT_TO_LEFT"; break;
    case osgText::TextBase::VERTICAL: os << "VERTICAL"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::KerningType & t)
{
    switch(t)
    {
    case osgText::KERNING_DEFAULT: os << "KERNING_DEFAULT"; break;
    case osgText::KERNING_UNFITTED: os << "KERNING_UNFITTED"; break;
    case osgText::KERNING_NONE: os << "KERNING_NONE"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::BackdropType & t)
{
    switch(t)
    {
    case osgText::Text::DROP_SHADOW_BOTTOM_RIGHT: os << "DROP_SHADOW_BOTTOM_RIGHT"; break;
    case osgText::Text::DROP_SHADOW_CENTER_RIGHT: os << "DROP_SHADOW_CENTER_RIGHT"; break;
    case osgText::Text::DROP_SHADOW_TOP_RIGHT: os << "DROP_SHADOW_TOP_RIGHT"; break;
    case osgText::Text::DROP_SHADOW_BOTTOM_CENTER: os << "DROP_SHADOW_BOTTOM_CENTER"; break;
    case osgText::Text::DROP_SHADOW_TOP_CENTER: os << "DROP_SHADOW_TOP_CENTER"; break;
    case osgText::Text::DROP_SHADOW_BOTTOM_LEFT: os << "DROP_SHADOW_BOTTOM_LEFT"; break;
    case osgText::Text::DROP_SHADOW_CENTER_LEFT: os << "DROP_SHADOW_CENTER_LEFT"; break;
    case osgText::Text::DROP_SHADOW_TOP_LEFT: os << "DROP_SHADOW_TOP_LEFT"; break;
    case osgText::Text::OUTLINE: os << "OUTLINE"; break;
    case osgText::Text::NONE: os << "NONE"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::BackdropImplementation & t)
{
    switch(t)
    {
    case osgText::Text::POLYGON_OFFSET: os << "POLYGON_OFFSET"; break;
    case osgText::Text::NO_DEPTH_BUFFER: os << "NO_DEPTH_BUFFER"; break;
    case osgText::Text::DEPTH_RANGE: os << "DEPTH_RANGE"; break;
    case osgText::Text::STENCIL_BUFFER: os << "STENCIL_BUFFER"; break;
    case osgText::Text::DELAYED_DEPTH_WRITES: os << "DELAYED_DEPTH_WRITES"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::ColorGradientMode & t)
{
    switch(t)
    {
    case osgText::Text::SOLID: os << "SOLID"; break;
    case osgText::Text::PER_CHARACTER: os << "PER_CHARACTER"; break;
    case osgText::Text::OVERALL: os << "OVERALL"; break;
    default: os << (int)t; break;
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::CharacterSizeMode & t)
{
    switch(t)
    {
    case osgText::Text::OBJECT_COORDS: os << "OBJECT_COORDS"; break;
    case osgText::Text::SCREEN_COORDS: os << "SCREEN_COORDS"; break;
    case osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT: os << "OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT"; break;
    default: os << (int)t; break;
    }
    return os;
}
} // namespace std

