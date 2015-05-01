// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemOsgEarth.h"

namespace sgi {
namespace support {

class OsgEarthConfig
{
public:
    typedef osgEarth::Config object_type;
    typedef SGIHostItemOsgEarthConfig host_item_type;
};

class OsgEarthConfigOptions
{
public:
    typedef osgEarth::ConfigOptions object_type;
    typedef SGIHostItemOsgEarthConfigOptions host_item_type;
};

} // namespace support
} // namespace sgi
