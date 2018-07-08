// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <osgEarth/Config>

namespace osg {
    class Referenced;
}

namespace osgEarth {
    class Config;
    class ConfigOptions;
}

namespace sgi {

typedef SGIItemInfoPlainPtr<osgEarth::Config> SGIItemInfoOsgEarthConfig;
typedef SGIHostItemImpl<SGIItemInfoOsgEarthConfig> SGIHostItemOsgEarthConfig;
typedef SGIItemInfoPlainPtr<osgEarth::ConfigOptions> SGIItemInfoOsgEarthConfigOptions;
typedef SGIHostItemImpl<SGIItemInfoOsgEarthConfigOptions> SGIHostItemOsgEarthConfigOptions;

} // namespace sgi
